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
    DialogScan(Preferences *t_prf, const QVector<VstBucket> *pVstBuckets);
    int exec() override;

private:
    const QVector<VstBucket> *mVstBuckets;
    QVBoxLayout *mLayoutVMain;
    QHBoxLayout *mLayoutHScanFolder;
    QHBoxLayout *mLayoutHAmount;
    QHBoxLayout *mLayoutHVerifyAndAmount;
    QHBoxLayout *mLayoutHListView;
    QVBoxLayout *mLayoutVListViewLeft;
    QVBoxLayout *mLayoutVListViewRight;
    QHBoxLayout *mLayoutHBottom;
    QLabel *mLabelInfo;
    QLabel *mLabelScanFolder;
    QLineEdit *mLineEditAmountDll;
    QLineEdit *mLineEditAmountVst3;
    QLineEdit *mLineEditScanFolder;
    QPushButton *mPushButtonSelectFolder;
    QPushButton *mPushButtonScan;
    QPushButton *mPushButtonFilter;
    QPushButton *mPushButtonCancel;
    QPushButton *mPushButtonAdd;
    QCheckBox *mCheckBoxCheckBasic;
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
    QShortcut *shortcutResize;
    CustomProgressDialog *mProgressDialog;
    Preferences *prf;
    QLabel *mLabelSelected;
    void setupMouseMenu();
    void enableViewUpdate(bool enable);
    void repaintTableview();
    QVector<int> getSelectionOrigIdx(const QModelIndexList &indexList);
    void reject() override;
    void getScanAmount(const QString &path, int &numDll, int &numVst3);
    int mNumVst3, mNumDll;
private slots:
    void slotSelectScanFolder();
    void slotScan();
    void slotScanFinished(bool newFindings);
    void slotCancel();
    void slotAdd();
    void slotScanCancel();
    void slotScanCanceled();

    void slotMouseRightClickOnEntry(QPoint point);
    void slotFilterBarClose();
    void slotFilterBar();
    void slotFilterBarTextChanged();
    void slotSelectEntry();
    void slotUnselectEntry();
    void slotFilterBarOpen();
    void slotResizeTableToContent();
    void slotTableSelectionChanged();
signals:
    void signalScanSelection(QVector<ScanResult> scanSelection);
};

#endif // DIALOGSCAN_H
