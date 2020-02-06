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

#include "logoutput.h"
#include "modelvstbuckets.h"
#include "dialogpreferences.h"
#include "preferences.h"
#include "confighandler.h"
#include "dialogscan.h"
#include "scanresult.h"
#include "customsortfilterproxymodel.h"
#include "legacyconfigparser.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    LogOutput *mLogOutput;
    QSplitter *mSplitter;
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
    QList<int> getSelectionOrigIdx(const QModelIndexList &indexList);
    QMenu *mouseMenu;
    DialogPreferences *mDialogPreferences;
    DialogScan *mDialogScan;
    Preferences *prf;
    ConfigHandler *cfg;
    LegacyConfigParser *legacyConfigParser;
    void closeEvent(QCloseEvent *event);
    QFileInfo *configFileInfo;
    bool configNeedsSaving;
    QAction *actionDebugInfo;
    QAction *actionHideBlacklisted;

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
    void slotAddScannedVst(QList<ScanResult> scanSelection);
    void slotImportLegacyConfig();

    void slotMouseRightClickOnVst(QPoint point);
    void slotFilterBar();
    void slotFilterBarClose();
    void slotDialogPreferences();
    void slotDialogScan();
    void slotDialogAbout();
    void slotDialogAboutQt();
    void slotSave();
    void slotConfigDataChanged();
    void slotPostSetupInfo();
    void slotOrphanDetection();
    void slotFeedbackLogOutput(const QString &msg, bool isVerboseInfo);
};

#endif // MAINWINDOW_H
