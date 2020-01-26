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
#include <QSortFilterProxyModel>
#include "modelscan.h"
#include "vstbucket.h"
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QModelIndexList>
#include <QShortcut>
#include <QTimer>

#include "horizontalline.h"
#include "verticalline.h"

DialogScan::DialogScan(const QList<VstBucket> *pVstBuckets) : mVstBuckets(pVstBuckets)
{
    setupUI();

    connect(mModelScan, &ModelScan::signalScanDone, this, &DialogScan::slotScanDone);
}

void DialogScan::setupUI()
{
    this->setWindowTitle("Scan for VSTs");

    // Allocate starting from parent to children
    mLayoutVMain = new QVBoxLayout();
    mLayoutHScanFolder = new QHBoxLayout();
    mLayoutHVstType = new QHBoxLayout();
    mLayoutHListView = new QHBoxLayout();
    mLayoutVListViewLeft = new QVBoxLayout();
    mLayoutVListViewRight = new QVBoxLayout();
    mFilterBarLayout = new QHBoxLayout();
    mLayoutHBottom = new QHBoxLayout();

    mLabelScanFolder = new QLabel("Scan folder:");
    mLineEditScanFolder = new QLineEdit();
    mPushButtonSelectFolder = new QPushButton("Select");
    mLabelVstType = new QLabel("VST type:");
    mComboBoxVstType = new QComboBox();
    mTableview = new QTableView(this);
    mSortFilter = new QSortFilterProxyModel(mTableview);
    mModelScan = new ModelScan(mVstBuckets);

    mFilterBar = new QWidget();
    mFilterBarLineEdit = new QLineEdit(this);
    mFilterBarCloseButton = new QPushButton("X");
    mFilterBarLabel = new QLabel("Filter:");

    mPushButtonScan = new QPushButton("Scan");
    mPushButtonFilter = new QPushButton("Filter");
    HorizontalLine *hLine0 = new HorizontalLine();
    mPushButtonCancel = new QPushButton("Cancel");
    mPushButtonAdd = new QPushButton("Add");
    HorizontalLine *hLine1 = new HorizontalLine();
    HorizontalLine *hLine2 = new HorizontalLine();



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

    // ======================================
    // === Second row: VST type selection ===
    // ======================================
    mLabelVstType->setMinimumWidth(80);
    mComboBoxVstType->setMinimumWidth(65);
    mComboBoxVstType->addItem("VST2");
    mComboBoxVstType->addItem("VST3");

    mLayoutHVstType->addWidget(mLabelVstType);
    mLayoutHVstType->addWidget(mComboBoxVstType);
    mLayoutHVstType->addStretch();

    // ===========================
    // === Third row: listview ===
    // ===========================
    // === Listview - left side ===
    mSortFilter->setSourceModel(mModelScan);
    mTableview->setModel(mSortFilter);

    mTableview->verticalHeader()->hide();
    mTableview->setSortingEnabled(true);
    mSortFilter->sort(1, Qt::AscendingOrder);
    mSortFilter->setDynamicSortFilter(false);
    mSortFilter->setFilterKeyColumn(-1); // -1: filter based on all columns
    mSortFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);

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
    connect(mFilterBarLineEdit, &QLineEdit::textChanged, mSortFilter, &QSortFilterProxyModel::setFilterFixedString);

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
    mLayoutVMain->addLayout(mLayoutHScanFolder);
    mLayoutVMain->addLayout(mLayoutHVstType);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addWidget(hLine1);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addLayout(mLayoutHListView);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addWidget(hLine2);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addLayout(mLayoutHBottom);

    this->setLayout(mLayoutVMain);
    setMinimumWidth(800);

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
    } else {
    }

    if (mLineEditScanFolder->text() != "") {
        mPushButtonScan->setEnabled(true);
    } else {
        mPushButtonScan->setEnabled(false);
    }
}

void DialogScan::slotScan()
{
    /* TODO: slotScan: Basically:
     * X trigger the class that shall perform the scan (or rather the model, which itself then triggers theh scan)
     * - Q: Have some kind of "ongoing scan" visual feedback for user?
     * - allow user to cancel scan? (see "lock" usage)
     */

    VstType vstType;
    if (mComboBoxVstType->currentText() == "VST2") {
        vstType = VstType::VST2;
    } else {
        vstType = VstType::VST3;
    }

    mModelScan->triggerScan(mLineEditScanFolder->text(), vstType);
}

void DialogScan::slotScanDone()
{
    slotResizeTableToContent();
}

void DialogScan::slotCancel()
{
    this->close();
}

void DialogScan::slotAdd()
{
    /* TODO: slotAdd: Basically:
     * - get user selection of listview
     * - emit the signal
     * - close the dialog
     */
//    emit(signalScanSelection())
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

QList<int> DialogScan::getSelectionOrigIdx(QModelIndexList indexList)
{
    QList<int> indexOfScanResults;
    foreach (QModelIndex index, indexList) {
        QModelIndex originalIndex = mSortFilter->mapToSource(index);
        indexOfScanResults.append(originalIndex.row());
    }
    return indexOfScanResults;
}

void DialogScan::slotResizeTableToContent()
{
    if (mModelScan->isModelEmpty()) {
        // No entires yet; resize to fixed width
        mTableview->setColumnWidth(0, 55);  // Selection
        mTableview->setColumnWidth(1, 60);  // Name
        mTableview->setColumnWidth(2, 45);  // Type
        mTableview->setColumnWidth(3, 100); // Path
        mTableview->setColumnWidth(4, 20);  // Index
    } else {
        mTableview->resizeColumnsToContents();
        mTableview->resizeRowsToContents();

        // Set rows to sensible with; some fixed width, some based on content
        mTableview->setColumnWidth(0, 55);  // Selection
        mTableview->setColumnWidth(1, mTableview->columnWidth(1) + 10); // Name
        mTableview->setColumnWidth(2, 45);  // Type
        mTableview->setColumnWidth(3, mTableview->columnWidth(3) + 10); // Path
        mTableview->setColumnWidth(4, 20);  // Index
    }
}
