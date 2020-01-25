#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QHeaderView>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include "defines.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("LinVstManager");

    prf = new Preferences();
    cfg = new ConfigHandler();
    configFileInfo = new QFileInfo(D_CONFIG_FILE_PATH);
    configNeedsSaving = false;

    // Load config file
    QList<VstBucket> tmpVstBuckets;
    RvConfFile retVal = cfg->loadConfig(*prf, tmpVstBuckets);
    if (retVal == RvConfFile::NotExists) {
        QMessageBox::critical(this,
                              "LinVstManager: Config file doesn't exist",
                              "Seems like the config file isn't where it is supposed to be:\n"
                              "  " + configFileInfo->filePath() + "\n\n"
                              "This could mean one of two things:\n"
                              "  a) it got moved/deleted for some reason\n"
                              "  b) it was never there to begin with (i.e. first time starting this app)\n\n"
                              "In case of 'b)': It's fine.",
                              QMessageBox::Ok, QMessageBox::Ok);
    } else if (retVal == RvConfFile::ErrorLoad) {
        QMessageBox::critical(this,
                              "LinVstManager: Error on config load",
                              "There was a problem with opening the config file.\n"
                              "Therefore nothing could be restored.",
                              QMessageBox::Ok, QMessageBox::Ok);
    } else if (retVal == RvConfFile::ParsingError) {
        QMessageBox::critical(this,
                              "LinVstManager: Parsing error during config load",
                              "There was a problem with parsing the config file.\n"
                              "Therefore nothing could be restored.",
                              QMessageBox::Ok, QMessageBox::Ok);
    }

    mDialogPreferences = new DialogPreferences(prf);

    mTableview = new QTableView(this);
    mSortFilter = new QSortFilterProxyModel(mTableview);
    mLogOutput = new LogOutput;
    mSplitter = new QSplitter(Qt::Vertical, this);

    QString tooltipFilterBar("Filter works on all columns. Close filter bar by pressing 'Ctrl-F' once again.");
    mFilterBarLineEdit = new QLineEdit(this);
    mFilterBarLineEdit->setToolTip(tooltipFilterBar);
    mFilterBarCloseButton = new QPushButton("X");
    mFilterBarCloseButton->setFixedWidth(28);
    connect(mFilterBarCloseButton, &QPushButton::pressed, this, &MainWindow::slotFilterBarClose);

    mFilterBarLayout = new QHBoxLayout();
    mFilterBarLabel = new QLabel("Filter:");
    mFilterBarLabel->setToolTip(tooltipFilterBar);
    mFilterBarLayout->addWidget(mFilterBarLabel);
    mFilterBarLayout->addWidget(mFilterBarLineEdit);
    mFilterBarLayout->addWidget(mFilterBarCloseButton);

    mFilterBar = new QWidget();
    mFilterBar->setLayout(mFilterBarLayout);
    mFilterBar->hide();


    mLayoutTop = new QVBoxLayout();
    mLayoutTop->addWidget(mTableview);
    mLayoutTop->addWidget(mFilterBar);

    mWidgetTop = new QWidget();
    mWidgetTop->setLayout(mLayoutTop);

    mSplitter->addWidget(mWidgetTop);
    mSplitter->addWidget(mLogOutput);
    this->setCentralWidget(mSplitter);


    mModelVstBuckets = new ModelVstBuckets(mTableview, tmpVstBuckets, prf);
    mSortFilter->setSourceModel(mModelVstBuckets);
    mTableview->setModel(mSortFilter);

    mTableview->verticalHeader()->hide();
    mTableview->setSortingEnabled(true);
    mSortFilter->sort(1, Qt::AscendingOrder);
    mSortFilter->setDynamicSortFilter(false);
    mSortFilter->setFilterKeyColumn(-1); // -1: filter based on all columns
    mSortFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(mFilterBarLineEdit, &QLineEdit::textChanged, mSortFilter, &QSortFilterProxyModel::setFilterFixedString);

    mTableview->setSelectionBehavior(QAbstractItemView::SelectRows);

    mTableview->setShowGrid(false);
    mTableview->horizontalHeader()->setHighlightSections(false);
    mTableview->horizontalHeader()->setStretchLastSection(true);
    mTableview->horizontalHeader()->setSectionsMovable(true);
    mTableview->horizontalHeader()->setDragEnabled(true);
    mTableview->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);

    setupMenuBar();
    mDialogScan = new DialogScan(mModelVstBuckets->getBufferVstBuckets());

    connect(mModelVstBuckets, &ModelVstBuckets::signalConfigDataChanged, this, &MainWindow::slotConfigDataChanged);
    connect(mDialogPreferences, &DialogPreferences::signalConfigDataChanged, this, &MainWindow::slotConfigDataChanged);
    connect(mDialogScan, &DialogScan::signalScanSelection, this, &MainWindow::slotAddScannedVst);

    mLogOutput->appendLog("Setup done.");

    // perform UI resize within event loop started with a.exec()
    QTimer::singleShot(0, this, SLOT(slotResizeMainUi()));
    QTimer::singleShot(0, this, SLOT(slotResizeTableToContent()));
    QTimer::singleShot(100, this, SLOT(slotPostSetupInfo()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupMenuBar()
{
    // Create menus
    QMenu *menuFile = menuBar()->addMenu(tr("&File"));
    QMenu *menuEdit = menuBar()->addMenu(tr("&Edit"));
    QMenu *menuView = menuBar()->addMenu(tr("&View"));
    QMenu *menuOptions = menuBar()->addMenu(tr("&Options"));
    QMenu *menuHelp = menuBar()->addMenu(tr("&Help"));

    // Create actions
    // menu: File
    QAction *actionSave = new QAction(tr("&Save"), this);
    QAction *actionExit = new QAction(tr("&Exit"), this);

    // menu: Edit
    QAction *actionEnable = new QAction(tr("&Enable"), this);
    QAction *actionDisable = new QAction(tr("&Disable"), this);
    QAction *actionUpdate = new QAction(tr("&Update"), this);
    QAction *actionBlacklist = new QAction(tr("&Blacklist"), this);
    QAction *actionUnblacklist = new QAction(tr("Un&blacklist"), this);
    QAction *actionAddVst = new QAction(tr("&Add VST"), this);
    QAction *actionRemoveVst = new QAction(tr("&Remove VST"), this);
    QAction *actionScan = new QAction(tr("&Scan"), this);

    // subMenu: Change bridge
    QMenu *subMenuChangeBridge = menuEdit->addMenu(tr("&Change bridge"));
    QMenu *subMenuOfVST2 = new QMenu(tr("of VST&2"), this);
    QAction *actionSetBridgeLinVst = new QAction(tr("&LinVst"), this);
    QAction *actionSetBridgeLinVstX = new QAction(tr("LinVst-&X"), this);
    QMenu *subMenuOfVST3 = new QMenu(tr("of VST&3"), this);
    QAction *actionSetBridgeLinVst3 = new QAction(tr("&LinVst3"), this);
    QAction *actionSetBridgeLinVst3X = new QAction(tr("LinVst3-&X"), this);

    // menu: View
    QAction *actionResizeTableToContent = new QAction(tr("&Resize table to content"), this);
    QAction *actionFilter = new QAction(tr("&Filter"), this);
    QMenu *subMenuDebug = new QMenu(tr("&Debug"));
    actionVerboseLogOutput = new QAction(tr("&Verbose log output"));
    actionVerboseLogOutput->setCheckable(true);
    actionVerboseLogOutput->setChecked(false);
    subMenuDebug->addAction(actionVerboseLogOutput);

    // menu: Options
    QAction *actionPreferences = new QAction(tr("&Preferences"), this);

    // menu: About
    QAction *actionAbout = new QAction(tr("&About"), this);

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
    actionResizeTableToContent->setShortcut(QKeySequence("Alt+R"));
    actionFilter->setShortcut(QKeySequence("Ctrl+F"));
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
    menuEdit->addSeparator();
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
    menuEdit->addAction(actionScan);

    menuView->addAction(actionFilter);
    menuView->addAction(actionResizeTableToContent);
    menuView->addMenu(subMenuDebug);

    menuOptions->addAction(actionPreferences);

    menuHelp->addAction(actionAbout);


    connect(actionSave, &QAction::triggered, this, &MainWindow::slotSave);
    connect(actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(actionResizeTableToContent, &QAction::triggered, this, &MainWindow::slotResizeTableToContent);
    connect(actionAddVst, &QAction::triggered, this, &MainWindow::slotAddVst);
    connect(actionRemoveVst, &QAction::triggered, this, &MainWindow::slotRemoveVst);

    // TODO: add connections once slots are implemented
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

    connect(actionVerboseLogOutput, &QAction::triggered, this, &MainWindow::slotVerboseLogOutput);

    connect(actionPreferences, &QAction::triggered, this, &MainWindow::slotDialogPreferences);
    connect(actionFilter, &QAction::triggered, this, &MainWindow::slotFilterBar);
    connect(actionAbout, &QAction::triggered, this, &MainWindow::slotDialogAbout);
    qDebug() << "setupMenuBar(): Done";

    setupMouseMenu(subMenuChangeBridge);
}

void MainWindow::setupMouseMenu(QMenu *subMenuChangeBridge)
{
    // Right click menu on table entries
    mTableview->setContextMenuPolicy(Qt::CustomContextMenu);
    mouseMenu = new QMenu(mTableview);
    QAction *mouseEnable = new QAction("Enable", this);
    QAction *mouseDisable = new QAction("Disable", this);
    mouseMenu->addAction(mouseEnable);
    mouseMenu->addAction(mouseDisable);
    mouseMenu->addSeparator();
    mouseMenu->addMenu(subMenuChangeBridge);

    connect(mTableview, &QTableView::customContextMenuRequested, this, &MainWindow::slotMouseRightClickOnVst);
    connect(mouseEnable, &QAction::triggered, this, &MainWindow::slotEnableVst);
    connect(mouseDisable, &QAction::triggered, this, &MainWindow::slotDisableVst);
}

void MainWindow::slotMouseRightClickOnVst(QPoint point)
{
    Q_UNUSED(point);
    mouseMenu->exec(QCursor::pos());
}

void MainWindow::slotFilterBarClose()
{
    mFilterBar->hide();
    mFilterBarLineEdit->clear();
}

void MainWindow::slotFilterBar()
{
    if (mFilterBar->isHidden()) {
        mFilterBar->show();
        mFilterBarLineEdit->setFocus();
    } else {
        mFilterBar->hide();
        mFilterBarLineEdit->clear();
    }
}

void MainWindow::slotDialogPreferences()
{
    mDialogPreferences->exec();
}

void MainWindow::slotDialogScan()
{
    mDialogScan->exec();
}

void MainWindow::slotDialogAbout()
{
    // TODO: Implement 'About' dialog (or just MessageBox?)
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

void MainWindow::slotConfigDataChanged()
{
    this->setWindowTitle("* LinVstManager");
    configNeedsSaving = true;
    mModelVstBuckets->refreshStatus();
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

void MainWindow::slotResizeMainUi()
{
//    mLogOutput->resize(mLogOutput->height(), 200);
    int size = mSplitter->height();
    mSplitter->setSizes(QList<int>() << static_cast<int>(size * 0.70) << static_cast<int>(size * 0.05) << static_cast<int>(size * 0.25));
}

void MainWindow::slotResizeTableToContent()
{
    mTableview->resizeColumnsToContents();
    mTableview->resizeRowsToContents();

    // Set rows to sensible with; some fixed width, some based on content
    mTableview->setColumnWidth(TableColumnPosType::NewlyAdded, 30);
    mTableview->setColumnWidth(TableColumnPosType::Status, 91);
    mTableview->setColumnWidth(TableColumnPosType::Name, mTableview->columnWidth(TableColumnPosType::Name) + 10);
    mTableview->setColumnWidth(TableColumnPosType::Type, 45);
    mTableview->setColumnWidth(TableColumnPosType::Bridge, 65);
    mTableview->setColumnWidth(TableColumnPosType::Path, mTableview->columnWidth(TableColumnPosType::Path) + 10);
    mTableview->setColumnWidth(TableColumnPosType::Index, 20);
}

void MainWindow::slotAddVst()
{
    static QString lastAddVstDir = QDir::homePath();
    QStringList filepaths_VstDll = QFileDialog::getOpenFileNames(this,
                                     "Select to be added VST-dll file", lastAddVstDir,
                                     "VST (*.dll *.vst3)");
    if (!filepaths_VstDll.isEmpty()) {
        lastAddVstDir = QFileInfo(filepaths_VstDll.at(0)).path();
            slotTableOperationStart();
            mModelVstBuckets->addVstBucket(filepaths_VstDll);
            slotTableOperationFinished();
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
            slotTableOperationStart();
            QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
            qDebug() << "indexList" << indexList;
            QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
            mModelVstBuckets->removeVstBucket(indexOfVstBuckets);
            slotTableOperationFinished();
            slotResizeTableToContent();
        }
    }
}

void MainWindow::slotTableOperationStart()
{
    enableViewUpdate(false);
}

void MainWindow::slotTableOperationFinished()
{
    enableViewUpdate(true);
    repaintTableview();
}

void MainWindow::slotEnableVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        slotTableOperationStart();
        mModelVstBuckets->enableVstBucket(indexOfVstBuckets);
        slotTableOperationFinished();
    }
}

void MainWindow::slotDisableVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        slotTableOperationStart();
        mModelVstBuckets->disableVstBucket(indexOfVstBuckets);
        slotTableOperationFinished();
    }
}

void MainWindow::slotBlacklistVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        slotTableOperationStart();
        mModelVstBuckets->blacklistVstBucket(indexOfVstBuckets);
        slotTableOperationFinished();
    }
}

void MainWindow::slotUnblacklistVst()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        slotTableOperationStart();
        mModelVstBuckets->unblacklistVstBucket(indexOfVstBuckets);
        slotTableOperationFinished();
    }
}

void MainWindow::slotUpdate()
{
    slotTableOperationStart();
    mModelVstBuckets->updateVsts();
    slotTableOperationFinished();
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

void MainWindow::slotVerboseLogOutput()
{
    if (actionVerboseLogOutput->isChecked()) {
        mLogOutput->enableVerboseLog(true);
    } else {
        mLogOutput->enableVerboseLog(false);
    }
}

void MainWindow::slotAddScannedVst(QStringList scanSelection)
{

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
        QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        slotTableOperationStart();
        QList<int> skippedIndices =  mModelVstBuckets->changeBridges(indexOfVstBuckets, bridgeType);
        slotTableOperationFinished();

        if (!skippedIndices.isEmpty()) {
            mLogOutput->appendLog("For some of the selected VSTs, the requested bridge type doesn't fit "
                                  "the selected VST's type or is not enabled in preferences.\n"
                                  "Therefore nothing has been change for the respective VSTs.");
            QString tmpStr;
            for (int i=0; i < skippedIndices.size(); i++) {
                tmpStr.append(QString::number(skippedIndices.at(i)));
                tmpStr.append(", ");
            }
            tmpStr.truncate(tmpStr.size() - 2);
            mLogOutput->appendLog("Indices of affected VSTs: " + tmpStr, true);
        }
    }
}

QList<int> MainWindow::getSelectionOrigIdx(QModelIndexList indexList)
{
    QList<int> indexOfVstBuckets;
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
