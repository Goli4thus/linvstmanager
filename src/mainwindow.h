// This file is part of LinVstManager.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTableView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVector>

#include "logoutput.h"
#include "modelvstbuckets.h"
#include "dialogpreferences.h"
#include "preferences.h"
#include "confighandler.h"
#include "dialogscan.h"
#include "scanresult.h"
#include "customsortfilterproxymodel.h"
#include "legacyconfigparser.h"
#include "datahasher.h"
#include "enums.h"
#include "dialogrename.h"
#include "dialogrenamebatch.h"
#include "sidebar.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    LogOutput *mLogOutput;
    QSplitter *mSplitterVert;
    QSplitter *mSplitterHori;
    QTableView *mTableview;
    CustomSortFilterProxyModel *mSortFilter;
    QVBoxLayout *mLayoutTop;
    QWidget *mFilterBar;
    QHBoxLayout *mFilterBarLayout;
    QLabel *mFilterBarLabel;
    QLineEdit *mFilterBarLineEdit;
    QPushButton *mFilterBarCloseButton;
    QWidget *mWidgetTop;
    ModelVstBuckets *mModelVstBuckets;
    void setupMenuBar();
    void setupMouseMenu(QMenu *subMenuChangeBridge);
    void repaintTableview();
    void enableViewUpdate(bool f_enable);
    void changeBridge(VstBridge bridgeType);
    QVector<int> getSelectionOrigIdx(const QModelIndexList &indexList);
    QMenu *mouseMenu;
    DialogPreferences *mDialogPreferences;
    DialogScan *mDialogScan;
    DialogRename *mDialogRename;
    DialogRenameBatch *mDialogRenameBatch;
    Preferences *prf;
    ConfigHandler *cfg;
    LegacyConfigParser *legacyConfigParser;
    void closeEvent(QCloseEvent *event);
    QFileInfo *configFileInfo;
    bool configNeedsSaving;
    QAction *actionDebugInfo;
    QAction *actionHideBlacklisted;
    DataHasher *mDataHasher;
    void updateSoTmplHashes(const QVector<VstBridge> &changedBridges);
    SideBar *mSideBar;
    void writeSettings();
    void readSettings();

private slots:
    void slotResizeMainUi();
    void slotResizeTableToContent();
    void slotAddVst();
    void slotRemoveVst();
    void slotEnableVst();
    void slotDisableVst();
    void slotBlacklistVst();
    void slotUnblacklistVst();
    void slotUpdate();
    void slotSetBridgeLinVst();
    void slotSetBridgeLinVstX();
    void slotSetBridgeLinVst3();
    void slotSetBridgeLinVst3X();
    void slotDebugInfo();
    void slotFilterBarTextChanged();
    void slotHideBlacklisted();
    void slotAddScannedVst(QVector<ScanResult> scanSelection);
    void slotImportLegacyConfig();

    void slotMouseRightClickOnVst(QPoint point);
    void slotFilterBar();
    void slotFilterBarApplyFilter(VstStatus status);
    void slotFilterBarClose();
    void slotDialogPreferences();
    void slotDialogScan();
    void slotRenameVst();
    void slotRenameAccepted(int indexNameConflict, QString nameNew);
    void slotRenameBatchVst();
    void slotRenameBatchAccepted(QVector<int> indices, bool modeAdd, bool atEnd, QString phrase);
    void slotDialogAbout();
    void slotDialogAboutQt();
    void slotSave();
    void slotConfigDataChanged(bool needsRefresh = false, QVector<VstBridge> changedBridges = {});
    void slotPostSetupInfo();
    void slotOrphanDetection();
    void slotFeedbackLogOutput(const QString &msg, bool isVerboseInfo);
    void slotFeedbackUpdateDone();
    void slotTableSelectionChanged();
};

#endif // MAINWINDOW_H
