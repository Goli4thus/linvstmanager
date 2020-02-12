// This file is part of LinVstManager.

#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include <QDialog>
#include <enums.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QCheckBox;
class QSpacerItem;
class QGroupBox;
class LineEditBridge;
class QDialogButtonBox;
class Preferences;
class QRadioButton;
class QButtonGroup;


class DialogPreferences : public QDialog
{
    Q_OBJECT
public:
    DialogPreferences(Preferences *t_prf);
    ~DialogPreferences();
private:
    QVBoxLayout *mLayoutVMain;
    QVBoxLayout *mLayoutVGeneral;
    QVBoxLayout *mLayoutVBridges;
    QGroupBox *mGroupBoxBridges;
    QGroupBox *mGroupBoxGeneral;
    QHBoxLayout *mLayoutHLinkFolder;
    QHBoxLayout *mLayoutHCheckTool64;
    QHBoxLayout *mLayoutHCheckTool32;
    LineEditBridge *mLineEditLinVst;
    LineEditBridge *mLineEditLinVstX;
    LineEditBridge *mLineEditLinVst3;
    LineEditBridge *mLineEditLinVst3X;
    QDialogButtonBox *mButtonBox;
    QLineEdit *mLineEditLinkFolder;
    QPushButton *mPushButtonLinkFolder;
    QLabel *labelLinkFolder;
    QLineEdit *mLineEditCheckTool64;
    QLineEdit *mLineEditCheckTool32;
    QPushButton *mPushButtonCheckTool64;
    QPushButton *mPushButtonCheckTool32;
    QLabel *labelCheckTool64;
    QLabel *labelCheckTool32;
    Preferences *prf;
    QRadioButton *mRadioButtonLinVst;
    QRadioButton *mRadioButtonLinVstX;
    QRadioButton *mRadioButtonLinVst3;
    QRadioButton *mRadioButtonLinVst3X;
    QButtonGroup *mButtonGroupDefaultVst2;
    QButtonGroup *mButtonGroupDefaultVst3;
    QVBoxLayout *mLayoutVDefaultsVst2;
    QVBoxLayout *mLayoutVDefaultsVst3;
    QHBoxLayout *mLayoutHDefaults;
    QGroupBox *mGroupBoxDefaults;
    QMap<VstBridge, QString> *mMapBridgeStr;
    QMap<VstBridge, quint8> *mMapBridgeIdx;
    void setupUI();
    void fillPreferences();
    QString lastDirCheckTool;
private slots:
    void slotButtonSelectLinkFolder();
    void slotButtonSelectCheckTool64();
    void slotButtonSelectCheckTool32();
    void slotButtonCancel();
    void slotButtonOk();
    void slotCheckBoxLinVst(bool isChecked);
    void slotCheckBoxLinVstX(bool isChecked);
    void slotCheckBoxLinVst3(bool isChecked);
    void slotCheckBoxLinVst3X(bool isChecked);

signals:
    void signalConfigDataChanged(bool needsRefresh, QVector<VstBridge> changedBridges);
};

#endif // DIALOGPREFERENCES_H
