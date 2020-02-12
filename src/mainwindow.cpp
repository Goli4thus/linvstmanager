// This file is part of LinVstManager.

#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QHeaderView>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include "defines.h"
#include <QPixmap>
#include "config.h"
#include "datastore.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("LinVstManager");
    this->setWindowIcon(QIcon(":/icons/linvstmanager_72.png"));

    mDataHasher = new DataHasher();
    prf = new Preferences();
    cfg = new ConfigHandler();
    legacyConfigParser = new LegacyConfigParser();
    configFileInfo = new QFileInfo(D_CONFIG_FILE_PATH);
    configNeedsSaving = false;

    // Load config file
    QVector<VstBucket> tmpVstBuckets;
    RvConfFile retVal = cfg->loadConfig(*prf, tmpVstBuckets);
    if (retVal == RvConfFile::NotExists) {
        QMessageBox::warning(this,
                             "LinVstManager: Config file doesn't exist",
                             "Seems like the config file isn't where it is supposed to be:\n"
                             "  " + configFileInfo->filePath() + "\n\n"
                             "This could mean one of two things:\n"
                             "  a) it got moved/deleted for some reason\n"
                             "  b) it was never there to begin with (i.e. first time starting this app)\n",
                             QMessageBox::Ok, QMessageBox::Ok);
    } else if (retVal == RvConfFile::ErrorLoad) {
        QMessageBox::critical(this,
                              "LinVstManager: Error on config load",
                              "There was a problem with loading the config file.\n"
                              "Therefore nothing could be restored.",
                              QMessageBox::Ok, QMessageBox::Ok);
    } else if (retVal == RvConfFile::ParsingError) {
        QMessageBox::critical(this,
                              "LinVstManager: Parsing error during config load",
                              "There was a problem with loading the config file.\n"
                              "Therefore nothing could be restored.",
                              QMessageBox::Ok, QMessageBox::Ok);
    } else if (retVal == RvConfFile::VersionError) {
        QMessageBox::critical(this,
                              "LinVstManager: Version error during config load",
                              "There was a problem with loading the config file.\n"
                              "Therefore nothing could be restored.",
                              QMessageBox::Ok, QMessageBox::Ok);
    }

    // Update soTmplHash values
    QVector<VstBridge> bridgeTypes = {VstBridge::LinVst,
                                      VstBridge::LinVstX,
                                      VstBridge::LinVst3,
                                      VstBridge::LinVst3X};
    QVector<VstBridge> changedBridges;
    QString soTmplPath;
    for (const auto &bridge : bridgeTypes) {
        soTmplPath = prf->getPathSoTmplBridge(bridge);
        if (soTmplPath != "") {
            changedBridges.append(bridge);
        }
    }
    updateSoTmplHashes(changedBridges);

    mDialogPreferences = new DialogPreferences(prf);

    // Allocate starting from parent to children
    mSplitterVert = new QSplitter(Qt::Vertical, this);
    mSplitterHori = new QSplitter(Qt::Horizontal, this);
    mWidgetTop = new QWidget();
    mLayoutTop = new QVBoxLayout();
    mTableview = new QTableView(this);
    mSortFilter = new CustomSortFilterProxyModel(mTableview);
    mLogOutput = new LogOutput;

    mFilterBar = new QWidget(this);
    mFilterBarLayout = new QHBoxLayout();
    mFilterBarLabel = new QLabel("Filter:");
    mFilterBarLineEdit = new QLineEdit(this);
    QString tooltipFilterBar("Filter works on a combined, space separated string of all columns. \nClose filter bar by pressing 'Ctrl-F' once again.");
    mFilterBarLineEdit->setToolTip(tooltipFilterBar);
    mFilterBarCloseButton = new QPushButton("X", this);
    mFilterBarCloseButton->setToolTip("Close filter bar by pressing 'Ctrl-F' once again.");

    mFilterBarCloseButton->setFixedWidth(28);
    connect(mFilterBarCloseButton, &QPushButton::pressed, this, &MainWindow::slotFilterBarClose);

    mFilterBarLabel->setToolTip(tooltipFilterBar);
    mFilterBarLayout->addWidget(mFilterBarLabel);
    mFilterBarLayout->addWidget(mFilterBarLineEdit);
    mFilterBarLayout->addWidget(mFilterBarCloseButton);

    mFilterBar->setLayout(mFilterBarLayout);
    mFilterBar->hide();


    mLayoutTop->addWidget(mTableview);
    mLayoutTop->addWidget(mFilterBar);

    mWidgetTop->setLayout(mLayoutTop);


    mModelVstBuckets = new ModelVstBuckets(mTableview, tmpVstBuckets, prf, *mDataHasher);
    mSortFilter->setSourceModel(mModelVstBuckets);
    mTableview->setModel(mSortFilter);

    mTableview->verticalHeader()->hide();
    mTableview->setSortingEnabled(true);
    mSortFilter->sort(2, Qt::AscendingOrder); // Initially sort by name
    mSortFilter->setDynamicSortFilter(true);
    mSortFilter->setFilterKeyColumn(-1); // -1: filter based on all columns
    mSortFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mSortFilter->setSortCaseSensitivity(Qt::CaseInsensitive);
    connect(mFilterBarLineEdit, &QLineEdit::textChanged, this, &MainWindow::slotFilterBarTextChanged);


    mTableview->setSelectionBehavior(QAbstractItemView::SelectRows);

    mTableview->setShowGrid(false);
    mTableview->horizontalHeader()->setHighlightSections(false);
    mTableview->horizontalHeader()->setStretchLastSection(false);
    mTableview->horizontalHeader()->setSectionsMovable(true);
    mTableview->horizontalHeader()->setDragEnabled(true);
    mTableview->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);


    mSideBar = new SideBar(*(mModelVstBuckets->getBufferVstBuckets()), this);
    mSideBar->setFixedWidth(160);
    mSplitterHori->setHandleWidth(1);
    mSplitterHori->addWidget(mWidgetTop);
    mSplitterHori->addWidget(mSideBar);
    mSplitterVert->addWidget(mSplitterHori);
    mSplitterVert->addWidget(mLogOutput);
    this->setCentralWidget(mSplitterVert);


    setupMenuBar();
    mDialogScan = new DialogScan(prf, mModelVstBuckets->getBufferVstBuckets());
    mDialogRename = new DialogRename(*mModelVstBuckets->getBufferVstBuckets());

    connect(mModelVstBuckets, &ModelVstBuckets::signalConfigDataChanged, this, &MainWindow::slotConfigDataChanged);
    connect(mModelVstBuckets, &ModelVstBuckets::signalFeedbackLogOutput, this, &MainWindow::slotFeedbackLogOutput);
    connect(mModelVstBuckets, &ModelVstBuckets::signalFeedbackUpdateDone, this, &MainWindow::slotFeedbackUpdateDone);
    connect(mDialogPreferences, &DialogPreferences::signalConfigDataChanged, this, &MainWindow::slotConfigDataChanged);
    connect(mDialogScan, &DialogScan::signalScanSelection, this, &MainWindow::slotAddScannedVst);
    connect(mDialogRename, &DialogRename::signalRenameAccept, this, &MainWindow::slotRenameAccepted);
    connect(mDialogRename, &DialogRename::signalConfigDataChanged, this, &MainWindow::slotConfigDataChanged);

    connect(mSideBar, &SideBar::signalFilerRequest, this, &MainWindow::slotFilterBarApplyFilter);
    connect(mTableview->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::slotTableSelectionChanged);

    mSideBar->slotUpdateSelection(0, mModelVstBuckets->getSizeVstBuckets());
    mSideBar->slotUpdateCounts();

    mLogOutput->appendLog("Setup done.");

    // perform UI resize within event loop started with a.exec()
    QTimer::singleShot(0, this, SLOT(slotResizeMainUi()));
    QTimer::singleShot(0, this, SLOT(slotResizeTableToContent()));
    QTimer::singleShot(100, this, SLOT(slotPostSetupInfo()));
}

MainWindow::~MainWindow()
{
    delete prf;
    delete cfg;
    delete mDataHasher;
    delete mLogOutput;
    delete mModelVstBuckets;
    delete mSortFilter;
    delete legacyConfigParser;
}

void MainWindow::setupMenuBar()
{
    // Create menus
    auto *menuFile = menuBar()->addMenu(tr("&File"));
    auto *menuEdit = menuBar()->addMenu(tr("&Edit"));
    auto *menuView = menuBar()->addMenu(tr("&View"));
    auto *menuOptions = menuBar()->addMenu(tr("&Options"));
    auto *menuHelp = menuBar()->addMenu(tr("&Help"));

    // Create actions
    // menu: File
    auto *actionSave = new QAction(tr("&Save"), this);
    auto *actionImport = new QAction(tr("&Import legacy config..."), this);
    auto *actionExit = new QAction(tr("&Exit"), this);

    // menu: Edit
    auto *actionEnable = new QAction(tr("&Enable"), this);
    auto *actionDisable = new QAction(tr("&Disable"), this);
    auto *actionUpdate = new QAction(tr("&Update"), this);
    auto *actionBlacklist = new QAction(tr("&Blacklist"), this);
    auto *actionUnblacklist = new QAction(tr("Un&blacklist"), this);
    auto *actionAddVst = new QAction(tr("&Add VST"), this);
    auto *actionRemoveVst = new QAction(tr("&Remove VST"), this);
    auto *actionRenameVst = new QAction(tr("Re&name VST"), this);
    auto *actionScan = new QAction(tr("&Scan"), this);

    // subMenu: Change bridge
    auto *subMenuChangeBridge = menuEdit->addMenu(tr("&Change bridge"));
    auto *subMenuOfVST2 = new QMenu(tr("of VST&2"), this);
    auto *actionSetBridgeLinVst = new QAction(tr("&LinVst"), this);
    auto *actionSetBridgeLinVstX = new QAction(tr("LinVst-&X"), this);
    auto *subMenuOfVST3 = new QMenu(tr("of VST&3"), this);
    auto *actionSetBridgeLinVst3 = new QAction(tr("&LinVst3"), this);
    auto *actionSetBridgeLinVst3X = new QAction(tr("LinVst3-&X"), this);

    // menu: View
    auto *actionResizeTableToContent = new QAction(tr("&Resize table to content"), this);
    auto *actionFilter = new QAction(tr("&Filter"), this);
    actionDebugInfo = new QAction(tr("Debug &info"), this);
    actionDebugInfo->setCheckable(true);
    actionDebugInfo->setChecked(false);
    actionHideBlacklisted = new QAction(tr("&Hide blacklisted"), this);
    actionHideBlacklisted->setCheckable(true);
    bool initVal = prf->getHideBlacklisted();
    // Restore preference
    actionHideBlacklisted->setChecked(initVal);
    // Init proxy model
    mSortFilter->setHideBlacklisted(initVal);
    mSortFilter->setShowIndexColumn(false);

    // menu: Options
    auto *actionPreferences = new QAction(tr("&Preferences"), this);

    // menu: About
    auto *actionAbout = new QAction(tr("&About"), this);
    auto *actionAboutQt = new QAction(tr("About&Qt"), this);

    // Setup shortcuts
    actionSave->setShortcut(QKeySequence("Ctrl+S"));
    actionExit->setShortcut(QKeySequence("Ctrl+Q"));
    actionEnable->setShortcut(QKeySequence("E"));
    actionDisable->setShortcut(QKeySequence("D"));
    actionUpdate->setShortcut(QKeySequence("U"));
    actionBlacklist->setShortcut(QKeySequence("B"));
    actionUnblacklist->setShortcut(QKeySequence("Shift+B"));
    actionAddVst->setShortcut(QKeySequence("A"));
    actionRemoveVst->setShortcut(QKeySequence("Shift+R"));
    actionRenameVst->setShortcut(QKeySequence("Alt+Shift+R"));
    actionResizeTableToContent->setShortcut(QKeySequence("Alt+R"));
    actionFilter->setShortcut(QKeySequence("Ctrl+F"));
    actionScan->setShortcut(QKeySequence("Alt+S"));
    actionDebugInfo->setShortcut(QKeySequence("Ctrl+I"));
    actionHideBlacklisted->setShortcut(QKeySequence("H"));
    actionPreferences->setShortcut(QKeySequence("Ctrl+P"));

    // 'Change bridge' sub menus
    subMenuChangeBridge->addMenu(subMenuOfVST2);
    subMenuChangeBridge->addMenu(subMenuOfVST3);
    subMenuOfVST2->addAction(actionSetBridgeLinVst);
    subMenuOfVST2->addAction(actionSetBridgeLinVstX);
    subMenuOfVST3->addAction(actionSetBridgeLinVst3);
    subMenuOfVST3->addAction(actionSetBridgeLinVst3X);

    // Add actions to menus
    menuFile->addAction(actionSave);
    menuFile->addAction(actionImport);
    menuFile->addSeparator();
    menuFile->addAction(actionExit);

    menuEdit->addAction(actionEnable);
    menuEdit->addAction(actionDisable);
    menuEdit->addAction(actionUpdate);
    menuEdit->addMenu(subMenuChangeBridge);
    menuEdit->addSeparator();
    menuEdit->addAction(actionBlacklist);
    menuEdit->addAction(actionUnblacklist);
    menuEdit->addSeparator();
    menuEdit->addAction(actionAddVst);
    menuEdit->addAction(actionRemoveVst);
    menuEdit->addSeparator();
    menuEdit->addAction(actionRenameVst);
    menuEdit->addSeparator();
    menuEdit->addAction(actionScan);

    menuView->addAction(actionFilter);
    menuView->addAction(actionResizeTableToContent);
    menuView->addSeparator();
    menuView->addAction(actionDebugInfo);
    menuView->addAction(actionHideBlacklisted);

    menuOptions->addAction(actionPreferences);

    menuHelp->addAction(actionAbout);
    menuHelp->addAction(actionAboutQt);


    connect(actionSave, &QAction::triggered, this, &MainWindow::slotSave);
    connect(actionImport, &QAction::triggered, this, &MainWindow::slotImportLegacyConfig);
    connect(actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(actionResizeTableToContent, &QAction::triggered, this, &MainWindow::slotResizeTableToContent);
    connect(actionAddVst, &QAction::triggered, this, &MainWindow::slotAddVst);
    connect(actionRemoveVst, &QAction::triggered, this, &MainWindow::slotRemoveVst);
    connect(actionRenameVst, &QAction::triggered, this, &MainWindow::slotRenameVst);

    connect(actionEnable, &QAction::triggered, this, &MainWindow::slotEnableVst);
    connect(actionDisable, &QAction::triggered, this, &MainWindow::slotDisableVst);
    connect(actionBlacklist, &QAction::triggered, this, &MainWindow::slotBlacklistVst);
    connect(actionUnblacklist, &QAction::triggered, this, &MainWindow::slotUnblacklistVst);
    connect(actionUpdate, &QAction::triggered, this, &MainWindow::slotUpdate);
    connect(actionScan, &QAction::triggered, this, &MainWindow::slotDialogScan);

    connect(actionSetBridgeLinVst, &QAction::triggered, this, &MainWindow::slotSetBridgeLinVst);
    connect(actionSetBridgeLinVstX, &QAction::triggered, this, &MainWindow::slotSetBridgeLinVstX);
    connect(actionSetBridgeLinVst3, &QAction::triggered, this, &MainWindow::slotSetBridgeLinVst3);
    connect(actionSetBridgeLinVst3X, &QAction::triggered, this, &MainWindow::slotSetBridgeLinVst3X);

    connect(actionDebugInfo, &QAction::triggered, this, &MainWindow::slotDebugInfo);
    connect(actionHideBlacklisted, &QAction::triggered, this, &MainWindow::slotHideBlacklisted);

    connect(actionPreferences, &QAction::triggered, this, &MainWindow::slotDialogPreferences);
    connect(actionFilter, &QAction::triggered, this, &MainWindow::slotFilterBar);
    connect(actionAbout, &QAction::triggered, this, &MainWindow::slotDialogAbout);
    connect(actionAboutQt, &QAction::triggered, this, &MainWindow::slotDialogAboutQt);

    setupMouseMenu(subMenuChangeBridge);
}

void MainWindow::setupMouseMenu(QMenu *subMenuChangeBridge)
{
    // Right click menu on table entries
    mTableview->setContextMenuPolicy(Qt::CustomContextMenu);
    mouseMenu = new QMenu(mTableview);
    auto *mouseEnable = new QAction("Enable", this);
    auto *mouseDisable = new QAction("Disable", this);
    auto *mouseRename = new QAction("Rename", this);
    mouseMenu->addAction(mouseEnable);
    mouseMenu->addAction(mouseDisable);
    mouseMenu->addSeparator();
    mouseMenu->addMenu(subMenuChangeBridge);
    mouseMenu->addSeparator();
    mouseMenu->addAction(mouseRename);

    connect(mTableview, &QTableView::customContextMenuRequested, this, &MainWindow::slotMouseRightClickOnVst);
    connect(mouseEnable, &QAction::triggered, this, &MainWindow::slotEnableVst);
    connect(mouseDisable, &QAction::triggered, this, &MainWindow::slotDisableVst);
    connect(mouseRename, &QAction::triggered, this, &MainWindow::slotRenameVst);
}

void MainWindow::slotMouseRightClickOnVst(QPoint point)
{
    Q_UNUSED(point)
    mouseMenu->exec(QCursor::pos());
}

void MainWindow::slotFilterBarClose()
{
    mFilterBar->hide();
    mFilterBarLineEdit->clear();
    slotResizeTableToContent();
}

void MainWindow::slotFilterBar()
{
    if (mFilterBar->isHidden()) {
        mFilterBar->show();
        mFilterBarLineEdit->setFocus();
    } else {
        mFilterBar->hide();
        mFilterBarLineEdit->clear();
        slotResizeTableToContent();
    }
}

void MainWindow::slotFilterBarApplyFilter(VstStatus status)
{
    if (mFilterBarLineEdit->text() == DataStore::getStatusStr(status)) {
        mFilterBar->hide();
        mFilterBarLineEdit->clear();
    } else {
        if (mFilterBar->isHidden()) {
            mFilterBar->show();
        }
        mFilterBarLineEdit->setText(DataStore::getStatusStr(status));
    }

    slotResizeTableToContent();
}

void MainWindow::slotDialogPreferences()
{
    mDialogPreferences->exec();
}

void MainWindow::slotDialogScan()
{
    mDialogScan->exec();
}

void MainWindow::slotRenameVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QVector<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);

        // Take the first entry and ignore the rest (rename is done one at a time)
        mDialogRename->init(indexOfVstBuckets.first());
        mDialogRename->exec();
    }
}

void MainWindow::slotRenameAccepted(int indexNameConflict, QString nameNew)
{
    enableViewUpdate(false);
    mModelVstBuckets->renameVstBucket(indexNameConflict, nameNew);
    enableViewUpdate(true);
    slotResizeTableToContent();
}

void MainWindow::slotDialogAbout()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About");
    msgBox.setIconPixmap(QPixmap(":/icons/linvstmanager_72.png"));
    msgBox.setTextFormat(Qt::RichText);

    QString msg;
    QTextStream tmp(&msg);
    tmp << "<font size=\"4\"><b>LinVstManager</b></font><br>"
        << "<br>"
        << "Companion application that allows managing VSTs in conjunction <br>"
        << "with various VST bridges created by osxmidi (<a href='https://github.com/osxmidi/LinVst'>LinVst</a>, ...).<br>"
        << "<br>"
        << "Version: " << D_VERSION_MAJOR << "." << D_VERSION_MINOR << "." << D_VERSION_PATCH << "<br>"
        << "Created by: Goli4thus <br>"
        << "<br>"
        << "<a href='https://github.com/Goli4thus/linvstmanager'>LinVstManager on github</a>";
    msgBox.setText(msg);
    msgBox.exec();
}

void MainWindow::slotDialogAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::slotSave()
{
    RvConfFile retVal = cfg->saveConfig(*prf, *(mModelVstBuckets->getBufferVstBuckets()));
    if(retVal == RvConfFile::ErrorWriteDir) {
        QMessageBox::information(this,
                                 "Saving config not possible",
                                 "Couldn't create folder for config file:\n"
                                 "  " + configFileInfo->path(),
                                 QMessageBox::Ok);
    } else if(retVal == RvConfFile::ErrorWriteDir) {
        QMessageBox::information(this,
                                 "Saving config not possible",
                                 "Couldn't write config file:\n"
                                 "  " + configFileInfo->filePath(),
                                 QMessageBox::Ok);
    } else {
        // Mark config as saved
        this->setWindowTitle("LinVstManager");
        configNeedsSaving = false;
    }
}

void MainWindow::slotConfigDataChanged(bool needsRefresh, QVector<VstBridge> changedBridges)
{
    this->setWindowTitle("* LinVstManager");
    configNeedsSaving = true;
    if (!changedBridges.isEmpty()) {
        updateSoTmplHashes(changedBridges);
        /* A refresh is needed after this due to possible
         * bridge version mismatch after bridge change. */
    }
    if (needsRefresh) {
        mModelVstBuckets->refreshStatus();
    }

    mSideBar->slotUpdateCounts();
}

void MainWindow::slotPostSetupInfo()
{
    QMessageBox::information(this,
                             "Please be aware",
                             "If your DAW is running while you're are removing or modifying VSTs in here, "
                             "your DAW might actually crash. Therefore make sure to close it beforehand.",
                             QMessageBox::Ok);

//    QTimer::singleShot(100, this, &MainWindow::slotOrphanDetection);
    slotOrphanDetection();
}

void MainWindow::slotOrphanDetection()
{
    QStringList orphansList = mModelVstBuckets->checkForOrphans();
    if (!orphansList.isEmpty()) {
        QString orphansFormatted;
        for (int i=0; i < orphansList.size(); i++) {
            orphansFormatted.append("  " + orphansList.at(i) + "\n");
        }

        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle("Orphans detected");
        msgBox.setText("Some orphaned links in the link folder have been detected.\n"
                                  "Should they be cleaned up?");
        msgBox.setDetailedText(orphansFormatted);
        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int retVal = msgBox.exec();

        if (retVal == QMessageBox::Yes) {
            mModelVstBuckets->removeOrphans(orphansList);
        }
    }
}

void MainWindow::slotFeedbackLogOutput(const QString &msg, bool isVerboseInfo = false)
{
    mLogOutput->appendLog(msg, isVerboseInfo);
}

void MainWindow::slotFeedbackUpdateDone()
{
//    QMessageBox::information(this, "Update done", "Update for all VSTs is done.");
    mLogOutput->appendLog("Update is done.");
}

void MainWindow::slotTableSelectionChanged()
{
    QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
    mSideBar->slotUpdateSelection(indexList.count(), mModelVstBuckets->getSizeVstBuckets());
}

void MainWindow::slotResizeMainUi()
{
//    mLogOutput->resize(mLogOutput->height(), 200);
    int size = mSplitterVert->height();
    mSplitterVert->setSizes(QList<int>() << static_cast<int>(size * 0.70) << static_cast<int>(size * 0.05) << static_cast<int>(size * 0.25));
}

void MainWindow::slotResizeTableToContent()
{
    mTableview->resizeColumnsToContents();
    mTableview->resizeRowsToContents();

    // Set rows to sensible with; some fixed width, some based on content
    mTableview->setColumnWidth(nsMW::TableColumnPosType::NewlyAdded, 30);
    mTableview->setColumnWidth(nsMW::TableColumnPosType::Status, 91);
    mTableview->setColumnWidth(nsMW::TableColumnPosType::Name, mTableview->columnWidth(nsMW::TableColumnPosType::Name) + 10);
    mTableview->setColumnWidth(nsMW::TableColumnPosType::VstType, 45);
    mTableview->setColumnWidth(nsMW::TableColumnPosType::BitType, 35);
    mTableview->setColumnWidth(nsMW::TableColumnPosType::Bridge, 65);
    mTableview->setColumnWidth(nsMW::TableColumnPosType::Path, mTableview->columnWidth(nsMW::TableColumnPosType::Path) + 10);
    mTableview->setColumnWidth(nsMW::TableColumnPosType::Index, 20);
}

void MainWindow::slotAddVst()
{
    static QString lastAddVstDir = QDir::homePath();
    QStringList filepaths_VstDll = QFileDialog::getOpenFileNames(this,
                                     "Select to be added VST-dll file", lastAddVstDir,
                                     "VST (*.dll *.vst3)");
    if (!filepaths_VstDll.isEmpty()) {
        lastAddVstDir = QFileInfo(filepaths_VstDll.at(0)).path();
            enableViewUpdate(false);
            mModelVstBuckets->addVstBucket(filepaths_VstDll);
            enableViewUpdate(true);
            slotResizeTableToContent();
    }
}

void MainWindow::slotRemoveVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QMessageBox::StandardButton retVal;
        retVal = QMessageBox::question(this, "Remove bridges",
                                       "Remove selected Vst bridges?",
                                       QMessageBox::No | QMessageBox::Yes,
                                       QMessageBox::Yes);

        if (retVal == QMessageBox::Yes) {
            enableViewUpdate(false);
            QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
//            qDebug() << "indexList" << indexList;
            QVector<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
            mModelVstBuckets->removeVstBucket(indexOfVstBuckets);
            enableViewUpdate(true);
            slotResizeTableToContent();
        }
    }
}

void MainWindow::slotEnableVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QVector<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        enableViewUpdate(false);
        mModelVstBuckets->enableVstBucket(indexOfVstBuckets);
        enableViewUpdate(true);
    }
}

void MainWindow::slotDisableVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QVector<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        enableViewUpdate(false);
        mModelVstBuckets->disableVstBucket(indexOfVstBuckets);
        enableViewUpdate(true);
    }
}

void MainWindow::slotBlacklistVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QVector<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        enableViewUpdate(false);
        mModelVstBuckets->blacklistVstBucket(indexOfVstBuckets);
        enableViewUpdate(true);
    }
}

void MainWindow::slotUnblacklistVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QVector<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        enableViewUpdate(false);
        mModelVstBuckets->unblacklistVstBucket(indexOfVstBuckets);
        enableViewUpdate(true);
    }
}

void MainWindow::slotUpdate()
{
    mLogOutput->appendLog("Performing update...");
    enableViewUpdate(false);
    mModelVstBuckets->updateVsts();
    enableViewUpdate(true);
}

void MainWindow::slotSetBridgeLinVst()
{
    changeBridge(VstBridge::LinVst);
}

void MainWindow::slotSetBridgeLinVstX()
{
    changeBridge(VstBridge::LinVstX);
}

void MainWindow::slotSetBridgeLinVst3()
{
    changeBridge(VstBridge::LinVst3);
}

void MainWindow::slotSetBridgeLinVst3X()
{
    changeBridge(VstBridge::LinVst3X);
}

void MainWindow::slotDebugInfo()
{
    if (actionDebugInfo->isChecked()) {
        mLogOutput->enableVerboseLog(true);
        mSortFilter->setShowIndexColumn(true);
    } else {
        mLogOutput->enableVerboseLog(false);
        mSortFilter->setShowIndexColumn(false);
    }
    slotResizeTableToContent();
//    QTimer::singleShot(10, this, SLOT(slotResizeTableToContent()));
}

void MainWindow::slotFilterBarTextChanged()
{
    QRegExp regExp(mFilterBarLineEdit->text(),
                   Qt::CaseInsensitive,
                   QRegExp::RegExp);
    mSortFilter->setFilterRegExp(regExp);
}

void MainWindow::slotHideBlacklisted()
{
    if (actionHideBlacklisted->isChecked()) {
        mSortFilter->setHideBlacklisted(true);
        if(prf->setHideBlacklisted(true)) {
            slotConfigDataChanged(false);
        }
    } else {
        mSortFilter->setHideBlacklisted(false);
        if(prf->setHideBlacklisted(false)) {
            slotConfigDataChanged(false);
        }
    }
    slotResizeTableToContent();
}

void MainWindow::slotAddScannedVst(QVector<ScanResult> scanSelection)
{
    enableViewUpdate(false);
    mModelVstBuckets->addScanSelection(&scanSelection);
    enableViewUpdate(true);
    slotResizeTableToContent();
}

void MainWindow::slotImportLegacyConfig()
{
    QString filepathLegacyConfig = QFileDialog::getOpenFileName(
                this,
                "Locate the legacy config file of linvstmanage-cli to import previous VSTs.",
                QDir::homePath(),
                "linvstmanage.ini");
    if (!filepathLegacyConfig.isEmpty()) {
        // Parse config to get a list of dll paths
        QStringList dllPaths = legacyConfigParser->parseLegacyConfig(filepathLegacyConfig);
        if (dllPaths.isEmpty()) {
            QMessageBox::information(
                        this,
                        "Import failed",
                        "Either linvstmanage.ini is empty or \n"
                        "there was a problem reading it.",
                        QMessageBox::Ok, QMessageBox::Ok);
        } else {
            enableViewUpdate(false);
            mModelVstBuckets->addVstBucket(dllPaths);
            enableViewUpdate(true);
            slotResizeTableToContent();
        }
    }
}

void MainWindow::repaintTableview()
{
    QModelIndex indexTop = mModelVstBuckets->index(0,0);
    QModelIndex indexBottom = mModelVstBuckets->index(mModelVstBuckets->rowCount()-1, mModelVstBuckets->columnCount()-1);
    emit mModelVstBuckets->dataChanged(indexTop, indexBottom);
    mTableview->viewport()->repaint();
}

void MainWindow::enableViewUpdate(bool f_enable)
{
    if (f_enable) {
        mModelVstBuckets->mUpdateView = true;
        repaintTableview();
    } else {
        mModelVstBuckets->mUpdateView = false;
    }
}

void MainWindow::changeBridge(VstBridge bridgeType)
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QVector<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        enableViewUpdate(false);
        QVector<int> skippedIndices =  mModelVstBuckets->changeBridges(indexOfVstBuckets, bridgeType);
        enableViewUpdate(true);

        if (!skippedIndices.isEmpty()) {
            mLogOutput->appendLog("For some of the selected VSTs, the requested bridge type doesn't fit "
                                  "the selected VST's type or is not enabled in preferences.\n"
                                  "Therefore nothing has been change for the respective VSTs.");
            QString tmpStr;
//            for (int i=0; i < skippedIndices.size(); i++) {
            for (int skippedIndice : skippedIndices) {
                tmpStr.append(QString::number(skippedIndice));
                tmpStr.append(", ");
            }
            tmpStr.truncate(tmpStr.size() - 2);
            mLogOutput->appendLog("Indices of affected VSTs: " + tmpStr, true);
        }
    }
}

QVector<int> MainWindow::getSelectionOrigIdx(const QModelIndexList &indexList)
{
    QVector<int> indexOfVstBuckets;
    foreach (QModelIndex index, indexList) {
        QModelIndex originalIndex = mSortFilter->mapToSource(index);
        indexOfVstBuckets.append(originalIndex.row());
    }
    return indexOfVstBuckets;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton retVal;
    if (configNeedsSaving) {
        retVal = QMessageBox::question(this, "About to exit...",
                                         "There are unsaved changes.\n"
                                         "\n"
                                         "Save: Save and Exit\n"
                                         "Discard: Exit without saving\n"
                                         "Cancel: Return to application",
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save);
    } else {
        retVal = QMessageBox::Yes;
    }

    if (retVal == QMessageBox::Save) {
        slotSave();
        QApplication::quit();
    } else if ((retVal == QMessageBox::Discard) || (retVal == QMessageBox::Yes)) {
        QApplication::quit();
    } else if ((retVal == QMessageBox::Cancel) || (retVal == QMessageBox::No)) {
        event->ignore();
    }
}

void MainWindow::updateSoTmplHashes(const QVector<VstBridge> &changedBridges)
{
    QString soTmplPath;
    for (const auto &bridge : changedBridges) {
        soTmplPath = prf->getPathSoTmplBridge(bridge);
        mDataHasher->updateHashSoTmplBridge(bridge, soTmplPath);
    }
}
