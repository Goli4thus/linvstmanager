// This file is part of LinVstManager.

#include "dialogscan.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QPushButton>
#include <QSizePolicy>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTableView>
#include <QComboBox>
#include "modelscan.h"
#include "vstbucket.h"
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QModelIndexList>
#include <QShortcut>
#include <customprogressdialog.h>
#include <QTimer>
#include <QCheckBox>
#include <QProcess>

#include "horizontalline.h"
#include "verticalline.h"
#include "preferences.h"
#include "customsortfilterproxymodel.h"

DialogScan::DialogScan(Preferences *t_prf, const QList<VstBucket> *pVstBuckets) : mVstBuckets(pVstBuckets)
{
    this->setWindowIcon(QIcon(":/icons/linvstmanager.png"));
    prf = t_prf;
    setupUI();

    connect(mModelScan, &ModelScan::signalScanFinished, this, &DialogScan::slotScanFinished);
    connect(mModelScan, &ModelScan::signalScanCanceled, this, &DialogScan::slotScanCanceled);
    connect(mProgressDialog, &CustomProgressDialog::signalCancelPressed, this, &DialogScan::slotScanCancel);
    connect(mModelScan, &ModelScan::signalFoundVst2, mProgressDialog, &CustomProgressDialog::slotFoundVst2);
    connect(mModelScan, &ModelScan::signalFoundVst3, mProgressDialog, &CustomProgressDialog::slotFoundVst3);
    connect(mModelScan, &ModelScan::signalFoundDll, mProgressDialog, &CustomProgressDialog::slotFoundDll);
}

int DialogScan::exec()
{
    // Update checkbox re CheckTool
    if (prf->checkToolEnabled()) {
        mCheckBoxCheckTool->setEnabled(true);
        mCheckBoxCheckTool->setChecked(true);
    } else {
        mCheckBoxCheckTool->setEnabled(false);
        mCheckBoxCheckTool->setChecked(false);
    }

    return QDialog::exec();
}

void DialogScan::setupUI()
{
    this->setWindowTitle("Scan for VSTs");

    // Allocate starting from parent to children
    mLayoutVMain = new QVBoxLayout();
    mLayoutHScanFolder = new QHBoxLayout();
    mLayoutHVerifyAndAmount = new QHBoxLayout();
    mLayoutHListView = new QHBoxLayout();
    mLayoutVListViewLeft = new QVBoxLayout();
    mLayoutVListViewRight = new QVBoxLayout();
    mFilterBarLayout = new QHBoxLayout();
    mLayoutHBottom = new QHBoxLayout();

    mLabelInfo = new QLabel("Hint: \n"
                            "Scanning will be recursive based on the selected folder. Folders starting with\n"
                            "a dot '.' (usually hidden in file browser by default) will be ignored though.\n"
                            "Therefore try to make an appropriate selection.\n"
                            "(i.e. navigate into the desired wine-prefix where the VSTs are)\n"
                            "\n"
                            "Try not to scan starting from the base of a wine prefix. There are A LOT of dll files\n"
                            "located in a wine prefix and the scan will take lots of time to complete, if it has to\n"
                            "check every single dll file (using the \"Verify\" option below).\n"
                            "\n"
                            "The \"Scan folder contains:\" section will show the amount of *.dll and *.vst3 files\n"
                            "that are located within the currently selected scan folder.");
    auto *hLineTop = new HorizontalLine();
    mLabelScanFolder = new QLabel("Scan folder:");
    mLineEditScanFolder = new QLineEdit();
    mPushButtonSelectFolder = new QPushButton("Select");
    mTableview = new QTableView(this);
    mSortFilter = new CustomSortFilterProxyModel(mTableview);
    mModelScan = new ModelScan(mVstBuckets);

    mFilterBar = new QWidget();
    mFilterBarLineEdit = new QLineEdit(this);
    mFilterBarCloseButton = new QPushButton("X");
    mFilterBarLabel = new QLabel("Filter:");

    mPushButtonScan = new QPushButton("Scan");
    mPushButtonFilter = new QPushButton("Filter");
    auto *hLine0 = new HorizontalLine();
    mPushButtonCancel = new QPushButton("Cancel");
    mPushButtonAdd = new QPushButton("Add");
    auto *hLine1 = new HorizontalLine();
    auto *hLine2 = new HorizontalLine();
    auto *vLine0 = new VerticalLine();
    mLineEditAmountDll = new QLineEdit();
    mLineEditAmountDll->setReadOnly(true);
    mLineEditAmountDll->setAlignment(Qt::AlignRight);
    mLineEditAmountDll->setFixedWidth(50);
    mLineEditAmountVst3 = new QLineEdit();
    mLineEditAmountVst3 ->setReadOnly(true);
    mLineEditAmountVst3->setAlignment(Qt::AlignRight);
    mLineEditAmountVst3->setFixedWidth(50);



    // ===================================
    // === First row: folder selection ===
    // ===================================
    mLabelScanFolder->setMinimumWidth(80);
    mLineEditScanFolder->setReadOnly(true);
    mLineEditScanFolder->setToolTip("The folder that shall be recursively scanned.");

    mLayoutHScanFolder->addWidget(mLabelScanFolder);
    mLayoutHScanFolder->addWidget(mLineEditScanFolder);
    mLayoutHScanFolder->addWidget(mPushButtonSelectFolder);
    mPushButtonScan->setEnabled(false);

    mCheckBoxCheckTool = new QCheckBox("Verify dll-files for being actual VST files.");
    mCheckBoxCheckTool->setToolTip("Requires 'VstDllCheck.exe' being setup in preferences.");

    // ============================
    // === Second row: listview ===
    // ============================
    // === Listview - left side ===
    mSortFilter->setSourceModel(mModelScan);
    mTableview->setModel(mSortFilter);

    mTableview->verticalHeader()->hide();
    mTableview->setSortingEnabled(true);
    mSortFilter->sort(1, Qt::AscendingOrder);
    mSortFilter->setDynamicSortFilter(true);
    mSortFilter->setFilterKeyColumn(-1); // -1: filter based on all columns
    mSortFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(mFilterBarLineEdit, &QLineEdit::textChanged, this, &DialogScan::slotFilterBarTextChanged);

    mTableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableview->setShowGrid(false);
    mTableview->horizontalHeader()->setHighlightSections(false);
    mTableview->horizontalHeader()->setStretchLastSection(true);
    mTableview->horizontalHeader()->setSectionsMovable(true);
    mTableview->horizontalHeader()->setDragEnabled(true);
    mTableview->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);


    // ==================
    // === filter bar ===
    QString tooltipFilterBar("Filter works on all columns. Close filter bar by using \n"
                             "that 'X' button or by pressing 'Ctrl-F' once again.");
    mFilterBarLineEdit->setToolTip(tooltipFilterBar);
    mFilterBarCloseButton->setFixedWidth(28);
    connect(mFilterBarCloseButton, &QPushButton::pressed, this, &DialogScan::slotFilterBarClose);

    mFilterBarLabel->setToolTip(tooltipFilterBar);
    mFilterBarLayout->addWidget(mFilterBarLabel);
    mFilterBarLayout->addWidget(mFilterBarLineEdit);
    mFilterBarLayout->addWidget(mFilterBarCloseButton);

    mFilterBar->setLayout(mFilterBarLayout);
    mFilterBar->hide();

    mLayoutVListViewLeft->addWidget(mTableview);
    mLayoutVListViewLeft->addWidget(mFilterBar);

    // =============================
    // === Listview - right side ===
    mPushButtonFilter->setToolTip("Ctrl-F");
    mLayoutVListViewRight->addWidget(mPushButtonScan);
//    mLayoutVListViewRight->addSpacing(10);
    mLayoutVListViewRight->addWidget(hLine0);
    mLayoutVListViewRight->addWidget(mPushButtonFilter);
    mLayoutVListViewRight->addStretch();

//    mLayoutHListView->setAlignment(Qt::AlignTop);
    mLayoutHListView->addLayout(mLayoutVListViewLeft);
    mLayoutHListView->addLayout(mLayoutVListViewRight);

    // ================================
    // === Last row: dialog buttons ===
    // ================================
    mLayoutHBottom->setAlignment(Qt::AlignRight);
    mLayoutHBottom->addWidget(mPushButtonCancel);
    mLayoutHBottom->addWidget(mPushButtonAdd);

    // ========================
    // === Add all together ===
    // ========================
    mLayoutVMain->addWidget(mLabelInfo);
    mLayoutVMain->addWidget(hLineTop);
    mLayoutVMain->addLayout(mLayoutHScanFolder);
    mLayoutVMain->addSpacing(5);

    mLayoutHVerifyAndAmount->addWidget(mCheckBoxCheckTool);
    mLayoutHVerifyAndAmount->addSpacing(40);
    mLayoutHVerifyAndAmount->addWidget(vLine0);
    mLayoutHVerifyAndAmount->addSpacing(20);
    mLayoutHVerifyAndAmount->addWidget(new QLabel("Scan folder contains: "));
    mLayoutHVerifyAndAmount->addWidget(new QLabel("*.dll: "));
    mLayoutHVerifyAndAmount->addWidget(mLineEditAmountDll);
    mLayoutHVerifyAndAmount->addSpacing(10);
    mLayoutHVerifyAndAmount->addWidget(new QLabel("*.vst3: "));
    mLayoutHVerifyAndAmount->addWidget(mLineEditAmountVst3);
    mLayoutHVerifyAndAmount->addStretch();
    mLayoutVMain->addLayout(mLayoutHVerifyAndAmount);

    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addWidget(hLine1);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addLayout(mLayoutHListView);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addWidget(hLine2);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addLayout(mLayoutHBottom);

    this->setLayout(mLayoutVMain);
    setMinimumWidth(590);
    resize(650, 600);

    connect(mPushButtonSelectFolder, &QPushButton::pressed, this, &DialogScan::slotSelectScanFolder);
    connect(mPushButtonScan, &QPushButton::pressed, this, &DialogScan::slotScan);
    connect(mPushButtonCancel, &QPushButton::pressed, this, &DialogScan::slotCancel);
    connect(mPushButtonAdd, &QPushButton::pressed, this, &DialogScan::slotAdd);
    connect(mPushButtonFilter, &QPushButton::pressed, this, &DialogScan::slotFilterBarOpen);

    shortcutSelect = new QShortcut(QKeySequence("S"), this);
    shortcutUnselect = new QShortcut(QKeySequence("D"), this);
    shortcutFilter = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    connect(shortcutSelect, &QShortcut::activated, this, &DialogScan::slotSelectEntry);
    connect(shortcutUnselect, &QShortcut::activated, this, &DialogScan::slotUnselectEntry);
    connect(shortcutFilter, &QShortcut::activated, this, &DialogScan::slotFilterBar);

    setupMouseMenu();

    mProgressDialog = new CustomProgressDialog();

    QTimer::singleShot(0, this, SLOT(slotResizeTableToContent()));
}

void DialogScan::setupMouseMenu()
{
    // Right click menu on table entries
    mTableview->setContextMenuPolicy(Qt::CustomContextMenu);
    mouseMenu = new QMenu(mTableview);
    actionSelect = new QAction("Select", this);
    actionUnselect = new QAction("Unselect", this);
    actionResize = new QAction("Resize", this);

    actionSelect->setShortcut(QKeySequence("S"));
    actionUnselect->setShortcut(QKeySequence("D"));
    mouseMenu->addAction(actionSelect);
    mouseMenu->addAction(actionUnselect);
    mouseMenu->addSeparator();
    mouseMenu->addAction(actionResize);

    connect(mTableview, &QTableView::customContextMenuRequested, this, &DialogScan::slotMouseRightClickOnEntry);
    connect(actionSelect, &QAction::triggered, this, &DialogScan::slotSelectEntry);
    connect(actionUnselect, &QAction::triggered, this, &DialogScan::slotUnselectEntry);
    connect(actionResize, &QAction::triggered, this, &DialogScan::slotResizeTableToContent);
}

void DialogScan::slotMouseRightClickOnEntry(QPoint point)
{
    Q_UNUSED(point);
    mouseMenu->exec(QCursor::pos());
}

void DialogScan::slotFilterBarOpen()
{
    mFilterBar->show();
    mFilterBarLineEdit->setFocus();
}

void DialogScan::slotFilterBarClose()
{
    mFilterBar->hide();
    mFilterBarLineEdit->clear();
}

void DialogScan::slotFilterBar()
{
    if (mFilterBar->isHidden()) {
        mFilterBar->show();
        mFilterBarLineEdit->setFocus();
    } else {
        mFilterBar->hide();
        mFilterBarLineEdit->clear();
    }
}

void DialogScan::slotFilterBarTextChanged()
{
    QRegExp regExp(mFilterBarLineEdit->text(),
                   Qt::CaseInsensitive,
                   QRegExp::RegExp);
    mSortFilter->setFilterRegExp(regExp);
}

void DialogScan::slotSelectEntry()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        enableViewUpdate(false);
        mModelScan->slotSelectEntry(indexOfVstBuckets);
        enableViewUpdate(true);
    }
}

void DialogScan::slotUnselectEntry()
{
    if (mTableview->selectionModel()->selectedRows().isEmpty()) {
        // No selection has been made. Therefore ignore it.
        return;
    } else {
        QModelIndexList indexList = mTableview->selectionModel()->selectedRows();
        QList<int> indexOfVstBuckets = getSelectionOrigIdx(indexList);
        enableViewUpdate(false);
        mModelScan->slotUnselectEntry(indexOfVstBuckets);
        enableViewUpdate(true);
    }
}

void DialogScan::slotSelectScanFolder()
{
    QString lastDir = QDir::homePath();

    if (mLineEditScanFolder->text() != "") {
        lastDir = QFileInfo(mLineEditScanFolder->text() + "/").path();
    }

    QString pathScanFolder = QFileDialog::getExistingDirectory(this,
                                         "Specify the folder that shall be recursively scanned.",
                                         lastDir);
    if (!pathScanFolder.isEmpty()) {
        mLineEditScanFolder->setText(pathScanFolder);
    }

    if (mLineEditScanFolder->text() != "") {
        mPushButtonScan->setEnabled(true);
        // update information on how many VSTs there are to be scanned.
        getScanAmount(mLineEditScanFolder->text(), mNumDll, mNumVst3);
        mLineEditAmountDll->setText(QString::number(mNumDll));
        mLineEditAmountVst3->setText(QString::number(mNumVst3));
    } else {
        mPushButtonScan->setEnabled(false);
        mLineEditAmountDll->setText("-");
        mLineEditAmountVst3->setText("-");
    }
}

void DialogScan::slotScan()
{
    mModelScan->triggerScan(mLineEditScanFolder->text(), prf->getPathCheckTool(), mCheckBoxCheckTool->isChecked());

    // Start progressbar dialog based on actual scan volume
    mProgressDialog->init(mNumDll + mNumVst3);
    mProgressDialog->exec();
}

void DialogScan::slotScanFinished(bool newFindings)
{
    slotResizeTableToContent();

    if (!newFindings) {
        QMessageBox::information(this, "No findings",
                                       "Nothing new could be found during the scan.");
    }

    // Close progress dialog
    mProgressDialog->close();
}

void DialogScan::slotCancel()
{
    this->close();
}

void DialogScan::slotAdd()
{
    QList<ScanResult> scanSelection = mModelScan->getScanSelection();

    if (scanSelection.isEmpty()) {
        // No selection has been made. Therefore ignore it.
        QMessageBox::information(this, "No selection",
                                       "No selection, therefore nothing can be added.\n\n"
                                       "Hint: Try mouse right click menu in table.");
    } else {
        emit(signalScanSelection(scanSelection));
        this->close();
    }
}

void DialogScan::slotScanCancel()
{
    mModelScan->slotScanCancel();
    mProgressDialog->close();
}

void DialogScan::slotScanCanceled()
{
    QMessageBox::information(this, "Scan canceled",
                             "The scan has been canceled.");
}

void DialogScan::enableViewUpdate(bool enable)
{
    if (enable) {
        mModelScan->mUpdateView = true;
        repaintTableview();
    } else {
        mModelScan->mUpdateView = false;
    }
}

void DialogScan::repaintTableview()
{
    QModelIndex indexTop = mModelScan->index(0,0);
    QModelIndex indexBottom = mModelScan->index(mModelScan->rowCount()-1, mModelScan->columnCount()-1);
    emit mModelScan->dataChanged(indexTop, indexBottom);
    mTableview->viewport()->repaint();
}

QList<int> DialogScan::getSelectionOrigIdx(const QModelIndexList &indexList)
{
    QList<int> indexOfScanResults;
    foreach (QModelIndex index, indexList) {
        QModelIndex originalIndex = mSortFilter->mapToSource(index);
        indexOfScanResults.append(originalIndex.row());
    }
    return indexOfScanResults;
}

void DialogScan::reject()
{
    // Empty the model, so it's a fresh the next time the scan dialog is opened.
    mModelScan->emptyModel();

    QDialog::reject();
}

void DialogScan::getScanAmount(const QString &path, int &numDll, int &numVst3)
{
    QProcess process;

    QString cmd = (QStringList() << "bash -c \"find " << path << " -iname '*.dll' | wc -l\"").join("");
    process.start(cmd);
    process.waitForFinished();
    QString retStr(process.readAllStandardOutput());
    numDll = retStr.toUInt();

    cmd = (QStringList() << "bash -c \"find " << path << " -iname '*.vst3' | wc -l\"").join("");
    process.start(cmd);
    process.waitForFinished();
    retStr = process.readAllStandardOutput();
    numVst3 = retStr.toUInt();
}

void DialogScan::slotResizeTableToContent()
{
    if (mModelScan->isModelEmpty()) {
        // No entires yet; resize to fixed width
        mTableview->setColumnWidth(0, 55);  // Selection
        mTableview->setColumnWidth(1, 60);  // Name
        mTableview->setColumnWidth(2, 55);  // Type
        mTableview->setColumnWidth(3, 300); // Path
        mTableview->setColumnWidth(4, 20);  // Index
    } else {
        mTableview->resizeColumnsToContents();
        mTableview->resizeRowsToContents();

        // Set rows to sensible with; some fixed width, some based on content
        mTableview->setColumnWidth(0, 55);  // Selection
        mTableview->setColumnWidth(1, mTableview->columnWidth(1) + 10); // Name
        mTableview->setColumnWidth(2, 55);  // Type
        mTableview->setColumnWidth(3, mTableview->columnWidth(3) + 10); // Path
        mTableview->setColumnWidth(4, 20);  // Index
    }
}
