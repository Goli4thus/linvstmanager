// This file is part of LinVstManager.

#ifndef DIALOGSCAN_H
#define DIALOGSCAN_H

#include <QObject>
#include <QDialog>
#include <enums.h>
#include <QModelIndexList>
#include "scanresult.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QSpacerItem;
class QDialogButtonBox;
class QTableView;
class QComboBox;
class QSortFilterProxyModel;
class ModelScan;
class VstBucket;
class QMenu;
class QShortcut;
class CustomProgressDialog;
class QCheckBox;
class Preferences;

class DialogScan : public QDialog
{
    Q_OBJECT
public:
    DialogScan(Preferences *t_prf, const QList<VstBucket> *pVstBuckets);
    int exec() override;

private:
    const QList<VstBucket> *mVstBuckets;
    QVBoxLayout *mLayoutVMain;
    QHBoxLayout *mLayoutHScanFolder;
    QHBoxLayout *mLayoutHListView;
    QVBoxLayout *mLayoutVListViewLeft;
    QVBoxLayout *mLayoutVListViewRight;
    QHBoxLayout *mLayoutHBottom;
    QLabel *mLabelInfo;
    QLabel *mLabelScanFolder;
    QLineEdit *mLineEditScanFolder;
    QPushButton *mPushButtonSelectFolder;
    QPushButton *mPushButtonScan;
    QPushButton *mPushButtonFilter;
    QPushButton *mPushButtonCancel;
    QPushButton *mPushButtonAdd;
    QCheckBox *mCheckBoxCheckTool;
    QTableView *mTableview;
    QWidget *mFilterBar;
    QHBoxLayout *mFilterBarLayout;
    QLabel *mFilterBarLabel;
    QLineEdit *mFilterBarLineEdit;
    QPushButton *mFilterBarCloseButton;
    ModelScan *mModelScan;
    void setupUI();
    QStringList scanFindings;
    QSortFilterProxyModel *mSortFilter;
    QMenu *mouseMenu;
    QAction *actionSelect;
    QAction *actionUnselect;
    QAction *actionFilter;
    QAction *actionResize;
    QShortcut *shortcutSelect;
    QShortcut *shortcutUnselect;
    QShortcut *shortcutFilter;
    CustomProgressDialog *mProgressDialog;
    Preferences *prf;
    void setupMouseMenu();
    void enableViewUpdate(bool enable);
    void repaintTableview();
    QList<int> getSelectionOrigIdx(QModelIndexList indexList);
    void reject() override;
private slots:
    void slotSelectScanFolder();
    void slotScan();
    void slotScanDone(bool findings);
    void slotCancel();
    void slotAdd();
    void slotScanCancel();
    void slotScanCanceled();

    void slotMouseRightClickOnEntry(QPoint point);
    void slotFilterBarClose();
    void slotFilterBar();
    void slotSelectEntry();
    void slotUnselectEntry();
    void slotFilterBarOpen();
    void slotResizeTableToContent();
signals:
    void signalScanSelection(QList<ScanResult> scanSelection);
};

#endif // DIALOGSCAN_H
