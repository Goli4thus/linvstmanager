#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTableView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSortFilterProxyModel>
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
    QSortFilterProxyModel *mSortFilter;
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
    QList<int> getSelectionOrigIdx(QModelIndexList indexList);
    QMenu *mouseMenu;
    DialogPreferences *mDialogPreferences;
    Preferences *prf;
    ConfigHandler *cfg;
    void closeEvent(QCloseEvent *event);
    QFileInfo *configFileInfo;
    bool configNeedsSaving;
    QAction *actionVerboseLogOutput;

private slots:
    void slotResizeMainUi();
    void slotResizeTableToContent();
    void slotAddVst();
    void slotRemoveVst();
    void slotTableOperationStart();
    void slotTableOperationFinished();
    void slotEnableVst();
    void slotDisableVst();
    void slotBlacklistVst();
    void slotUnblacklistVst();
    void slotUpdate();
    void slotSetBridgeLinVst();
    void slotSetBridgeLinVstX();
    void slotSetBridgeLinVst3();
    void slotSetBridgeLinVst3X();
    void slotVerboseLogOutput();

    void slotMouseRightClickOnVst(QPoint point);
    void slotFilterBar();
    void slotFilterBarClose();
    void slotDialogPreferences();
    void slotDialogAbout();
    void slotSave();
    void slotConfigDataChanged();
    void slotPostSetupInfo();
};

#endif // MAINWINDOW_H
