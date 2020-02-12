// This file is part of LinVstManager.

#include "dialogpreferences.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpacerItem>
#include <QPushButton>
#include <QGroupBox>
#include <QSizePolicy>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QRadioButton>
#include <QButtonGroup>
#include <QMessageBox>

#include "lineeditbridge.h"
#include "preferences.h"
#include "horizontalline.h"
#include "verticalline.h"

DialogPreferences::DialogPreferences(Preferences *t_prf)
{
    this->setWindowIcon(QIcon(":/icons/linvstmanager_72.png"));
    prf = t_prf;

    mMapBridgeStr = new QMap<VstBridge, QString>;
    mMapBridgeStr->insert(VstBridge::LinVst, "LinVst");
    mMapBridgeStr->insert(VstBridge::LinVstX, "LinVst-X");
    mMapBridgeStr->insert(VstBridge::LinVst3, "LinVst3");
    mMapBridgeStr->insert(VstBridge::LinVst3X, "LinVst3-X");

    mMapBridgeIdx = new QMap<VstBridge, quint8>;
    mMapBridgeIdx->insert(VstBridge::LinVst, 0);
    mMapBridgeIdx->insert(VstBridge::LinVstX, 1);
    mMapBridgeIdx->insert(VstBridge::LinVst3, 0);
    mMapBridgeIdx->insert(VstBridge::LinVst3X, 1);

    lastDirCheckTool = QDir::homePath();

    setupUI();
    fillPreferences();
}

DialogPreferences::~DialogPreferences()
{
    delete mMapBridgeStr;
    delete mMapBridgeIdx;
}

void DialogPreferences::setupUI()
{
    // Allocate starting from parent to children
    mLayoutVMain = new QVBoxLayout();
    mGroupBoxBridges = new QGroupBox("Bridges:");
    mGroupBoxDefaults = new QGroupBox("Defaults:");
    mGroupBoxGeneral = new QGroupBox("General:");

    mLayoutVBridges = new QVBoxLayout();
    mLayoutHDefaults = new QHBoxLayout();
    mLayoutVDefaultsVst2 = new QVBoxLayout();
    mLayoutVDefaultsVst3 = new QVBoxLayout();
    mLayoutVGeneral = new QVBoxLayout();
    mLayoutHLinkFolder = new QHBoxLayout();
    mLayoutHCheckTool64 = new QHBoxLayout();
    mLayoutHCheckTool32 = new QHBoxLayout();

    mLineEditLinVst = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVst), this);
    mLineEditLinVstX = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVstX), this);
    mLineEditLinVst3 = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVst3), this);
    mLineEditLinVst3X = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVst3X), this);

    mRadioButtonLinVst = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVst));
    mRadioButtonLinVstX = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVstX));
    mRadioButtonLinVst3 = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVst3));
    mRadioButtonLinVst3X = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVst3X));
    mButtonGroupDefaultVst2 = new QButtonGroup();
    mButtonGroupDefaultVst3 = new QButtonGroup();

    auto *vLine1 = new VerticalLine();
    labelLinkFolder = new QLabel("Link folder:");
    mLineEditLinkFolder = new QLineEdit();
    mPushButtonLinkFolder = new QPushButton("Select");
    labelCheckTool64 = new QLabel("'VstDllCheck64.exe':");
    labelCheckTool32 = new QLabel("'VstDllCheck32.exe':");
    mLineEditCheckTool64 = new QLineEdit();
    mLineEditCheckTool32 = new QLineEdit();
    mPushButtonCheckTool64 = new QPushButton("Select");
    mPushButtonCheckTool32 = new QPushButton("Select");
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                      QDialogButtonBox::Cancel);
    auto *line1 = new HorizontalLine();
    auto *line2 = new HorizontalLine();


    // ===========
    // = Bridges =
    // ===========
//    mLineEditLinVst->setStyleSheet("background-color:blue;");
    mLayoutVBridges->addWidget(mLineEditLinVst);
    mLayoutVBridges->addWidget(mLineEditLinVstX);
    mLayoutVBridges->addWidget(mLineEditLinVst3);
    mLayoutVBridges->addWidget(mLineEditLinVst3X);
    mLayoutVBridges->setSpacing(-5);
    mLayoutVBridges->update();
    this->adjustSize();
    mGroupBoxBridges->setLayout(mLayoutVBridges);

    // ============
    // = Defaults =
    // ============
    mLayoutVDefaultsVst2->addWidget(mRadioButtonLinVst);
    mLayoutVDefaultsVst2->addWidget(mRadioButtonLinVstX);
    mLayoutVDefaultsVst3->addWidget(mRadioButtonLinVst3);
    mLayoutVDefaultsVst3->addWidget(mRadioButtonLinVst3X);

    mButtonGroupDefaultVst2->addButton(mRadioButtonLinVst);
    mButtonGroupDefaultVst2->addButton(mRadioButtonLinVstX);
    mButtonGroupDefaultVst3->addButton(mRadioButtonLinVst3);
    mButtonGroupDefaultVst3->addButton(mRadioButtonLinVst3X);

    mLayoutHDefaults->addWidget(new QLabel("VST2:"));
    mLayoutHDefaults->addSpacing(5);
    mLayoutHDefaults->addLayout(mLayoutVDefaultsVst2, 0);
    mLayoutHDefaults->addSpacing(40);
    mLayoutHDefaults->addWidget(vLine1);
    mLayoutHDefaults->addSpacing(30);
    mLayoutHDefaults->addWidget(new QLabel("VST3:"));
    mLayoutHDefaults->addSpacing(5);
    mLayoutHDefaults->addLayout(mLayoutVDefaultsVst3, 0);
    mLayoutHDefaults->addStretch();

    mGroupBoxDefaults->setLayout(mLayoutHDefaults);

    // ===========
    // = General =
    // ===========
    // link folder
    mLineEditLinkFolder->setReadOnly(true);
    mLineEditLinkFolder->setToolTip("Specify the folder that shall contain all the bridged *.so files.");
    labelLinkFolder->setMinimumWidth(110);
    labelLinkFolder->setToolTip("Specify the folder that shall contain all the bridged *.so files.");
    mLayoutHLinkFolder->addWidget(labelLinkFolder);
    mLayoutHLinkFolder->addWidget(mLineEditLinkFolder);
    mLayoutHLinkFolder->addWidget(mPushButtonLinkFolder);

    // CheckTool
    QString toolTipCheckTool64("Locate the \'VstDllCheck64.exe\" utility.\n"
                             "Hint: Should be located in {repo-folder}/src/VstDllCheck/");
    mLineEditCheckTool64->setReadOnly(true);
    mLineEditCheckTool64->setToolTip(toolTipCheckTool64);
    labelCheckTool64->setMinimumWidth(110);
    labelCheckTool64->setToolTip(toolTipCheckTool64);
    mLayoutHCheckTool64->addWidget(labelCheckTool64);
    mLayoutHCheckTool64->addWidget(mLineEditCheckTool64);
    mLayoutHCheckTool64->addWidget(mPushButtonCheckTool64);

    QString toolTipCheckTool32("Locate the \'VstDllCheck32.exe\" utility.\n"
                             "Hint: Should be located in {repo-folder}/src/VstDllCheck/");
    mLineEditCheckTool32->setReadOnly(true);
    mLineEditCheckTool32->setToolTip(toolTipCheckTool32);
    labelCheckTool32->setMinimumWidth(110);
    labelCheckTool32->setToolTip(toolTipCheckTool32);
    mLayoutHCheckTool32->addWidget(labelCheckTool32);
    mLayoutHCheckTool32->addWidget(mLineEditCheckTool32);
    mLayoutHCheckTool32->addWidget(mPushButtonCheckTool32);

    mLayoutVGeneral->addLayout(mLayoutHLinkFolder);
    mLayoutVGeneral->addLayout(mLayoutHCheckTool64);
    mLayoutVGeneral->addLayout(mLayoutHCheckTool32);

    mGroupBoxGeneral->setLayout(mLayoutVGeneral);

    mLayoutVMain->addWidget(mGroupBoxBridges);
    mLayoutVMain->addWidget(mGroupBoxDefaults);
    mLayoutVMain->addWidget(mGroupBoxGeneral);


    mLayoutVMain->addWidget(line1);
    mLayoutVMain->addSpacing(10);
    mLayoutVMain->addWidget(new QLabel("Hint: The following things need to be set for full functionality:\n"
                                       "                - one or more bridges (mandatory)\n"
                                       "                - the link folder (mandatory)\n"
                                       "                - the 'VstDllCheck' utility (optional: used during VST scanning)"));
    mLayoutVMain->addSpacing(10);
    mLayoutVMain->addWidget(line2);
    mLayoutVMain->addSpacing(10);
    mLayoutVMain->addStretch();
    mLayoutVMain->addWidget(mButtonBox);

    this->setLayout(mLayoutVMain);

    setMinimumWidth(450);
//    setFixedHeight(400);
//    resize(500, 400);

    connect(mPushButtonLinkFolder, &QPushButton::pressed, this, &DialogPreferences::slotButtonSelectLinkFolder);
    connect(mPushButtonCheckTool64, &QPushButton::pressed, this, &DialogPreferences::slotButtonSelectCheckTool64);
    connect(mPushButtonCheckTool32, &QPushButton::pressed, this, &DialogPreferences::slotButtonSelectCheckTool32);
    connect(mButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::pressed, this, &DialogPreferences::slotButtonCancel);
    connect(mButtonBox->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, &DialogPreferences::slotButtonOk);

    connect(mLineEditLinVst, &LineEditBridge::signalCheckBoxBridge, this, &DialogPreferences::slotCheckBoxLinVst);
    connect(mLineEditLinVstX, &LineEditBridge::signalCheckBoxBridge, this, &DialogPreferences::slotCheckBoxLinVstX);
    connect(mLineEditLinVst3, &LineEditBridge::signalCheckBoxBridge, this, &DialogPreferences::slotCheckBoxLinVst3);
    connect(mLineEditLinVst3X, &LineEditBridge::signalCheckBoxBridge, this, &DialogPreferences::slotCheckBoxLinVst3X);
}

void DialogPreferences::fillPreferences()
{
    // Bridges
    mLineEditLinVst->setBridgeEnabled(prf->bridgeEnabled(VstBridge::LinVst));
    mLineEditLinVstX->setBridgeEnabled(prf->bridgeEnabled(VstBridge::LinVstX));
    mLineEditLinVst3->setBridgeEnabled(prf->bridgeEnabled(VstBridge::LinVst3));
    mLineEditLinVst3X->setBridgeEnabled(prf->bridgeEnabled(VstBridge::LinVst3X));

    mLineEditLinVst->setPath(prf->getPathSoTmplBridge(VstBridge::LinVst));
    mLineEditLinVstX->setPath(prf->getPathSoTmplBridge(VstBridge::LinVstX));
    mLineEditLinVst3->setPath(prf->getPathSoTmplBridge(VstBridge::LinVst3));
    mLineEditLinVst3X->setPath(prf->getPathSoTmplBridge(VstBridge::LinVst3X));

    // Defaults
    mRadioButtonLinVst->setEnabled(prf->bridgeEnabled(VstBridge::LinVst));
    mRadioButtonLinVstX->setEnabled(prf->bridgeEnabled(VstBridge::LinVstX));
    mRadioButtonLinVst3->setEnabled(prf->bridgeEnabled(VstBridge::LinVst3));
    mRadioButtonLinVst3X->setEnabled(prf->bridgeEnabled(VstBridge::LinVst3X));

    if (prf->getBridgeDefaultVst2IsX()) {
        mRadioButtonLinVstX->setChecked(true);
    } else {
        mRadioButtonLinVst->setChecked(true);
    }

    if (prf->getBridgeDefaultVst3IsX()) {
        mRadioButtonLinVst3X->setChecked(true);
    } else {
        mRadioButtonLinVst3->setChecked(true);
    }

    // General
    mLineEditLinkFolder->setText(prf->getPathLinkFolder());
    mLineEditCheckTool64->setText(prf->getPathCheckTool64());
    mLineEditCheckTool32->setText(prf->getPathCheckTool32());
}

void DialogPreferences::slotButtonSelectLinkFolder()
{
    QString lastDir = QDir::homePath();

    if (mLineEditLinkFolder->text() != "") {
        lastDir = QFileInfo(mLineEditLinkFolder->text()).path();
    }

    QString path_linkFolder = QFileDialog::getExistingDirectory(this,
                                         "Specify the folder that shall contain all the bridged *.so files.",
                                         lastDir);
    if (!path_linkFolder.isEmpty()) {
        mLineEditLinkFolder->setText(path_linkFolder);
    }
}

void DialogPreferences::slotButtonSelectCheckTool64()
{
    if (mLineEditCheckTool64->text() != "") {
        lastDirCheckTool = QFileInfo(mLineEditCheckTool64->text()).path();
    }

    QString path_checkTool64 = QFileDialog::getOpenFileName(this,
                                         "Locate the \'VstDllCheck64.exe\" utility.",
                                         lastDirCheckTool, "(VstDllCheck64.exe)");
    if (!path_checkTool64.isEmpty()) {
        mLineEditCheckTool64->setText(path_checkTool64);
    }
}

void DialogPreferences::slotButtonSelectCheckTool32()
{
    if (mLineEditCheckTool32->text() != "") {
        lastDirCheckTool = QFileInfo(mLineEditCheckTool32->text()).path();
    }

    QString path_checkTool32 = QFileDialog::getOpenFileName(this,
                                         "Locate the \'VstDllCheck32.exe\" utility.",
                                         lastDirCheckTool, "(VstDllCheck32.exe)");
    if (!path_checkTool32.isEmpty()) {
        mLineEditCheckTool32->setText(path_checkTool32);
    }}

void DialogPreferences::slotButtonCancel()
{
    this->close();
}

void DialogPreferences::slotButtonOk()
{
    if ((mLineEditLinVst->getBridgeEnabled() && !mLineEditLinVst->pathIsSet())
            || (mLineEditLinVstX->getBridgeEnabled() && !mLineEditLinVstX->pathIsSet())
            || (mLineEditLinVst3->getBridgeEnabled() && !mLineEditLinVst3->pathIsSet())
            || (mLineEditLinVst3X->getBridgeEnabled() && !mLineEditLinVst3X->pathIsSet())) {
        QMessageBox::information(this,
                                 "Inconsistent settings",
                                 "If a bridge is enabled, it's respective *.so template file needs to be selected as well.",
                                 QMessageBox::Ok);
    } else {
        bool prefsChanged;
        QVector<VstBridge> changedBridges;
        prefsChanged = prf->updatePreferences(mLineEditLinVst->getBridgeEnabled(),
                                              mLineEditLinVstX->getBridgeEnabled(),
                                              mLineEditLinVst3->getBridgeEnabled(),
                                              mLineEditLinVst3X->getBridgeEnabled(),
                                              mRadioButtonLinVstX->isChecked(),
                                              mRadioButtonLinVst3X->isChecked(),
                                              prf->getHideBlacklisted(),
                                              mLineEditLinVst->getPath(),
                                              mLineEditLinVstX->getPath(),
                                              mLineEditLinVst3->getPath(),
                                              mLineEditLinVst3X->getPath(),
                                              mLineEditLinkFolder->text(),
                                              mLineEditCheckTool64->text(),
                                              mLineEditCheckTool32->text(),
                                              changedBridges);
        if (prefsChanged) {
            emit(signalConfigDataChanged(true, changedBridges));
        }
        this->close();
    }
}

void DialogPreferences::slotCheckBoxLinVst(bool isChecked)
{
    if (isChecked) {
        mRadioButtonLinVst->setEnabled(true);
        if (!mRadioButtonLinVstX->isEnabled()) {
            mRadioButtonLinVst->setChecked(true);
        }
    } else {
        mRadioButtonLinVst->setEnabled(false);
        if (mRadioButtonLinVstX->isEnabled()) {
            mRadioButtonLinVstX->setChecked(true);
        }
    }
}

void DialogPreferences::slotCheckBoxLinVstX(bool isChecked)
{
    if (isChecked) {
        mRadioButtonLinVstX->setEnabled(true);
        if (!mRadioButtonLinVst->isEnabled()) {
            mRadioButtonLinVstX->setChecked(true);
        }
    } else {
        mRadioButtonLinVstX->setEnabled(false);
        if (mRadioButtonLinVst->isEnabled()) {
            mRadioButtonLinVst->setChecked(true);
        }
    }
}

void DialogPreferences::slotCheckBoxLinVst3(bool isChecked)
{
    if (isChecked) {
        mRadioButtonLinVst3->setEnabled(true);
        if (!mRadioButtonLinVst3X->isEnabled()) {
            mRadioButtonLinVst3->setChecked(true);
        }
    } else {
        mRadioButtonLinVst3->setEnabled(false);
        if (mRadioButtonLinVst3X->isEnabled()) {
            mRadioButtonLinVst3X->setChecked(true);
        }
    }
}

void DialogPreferences::slotCheckBoxLinVst3X(bool isChecked)
{
    if (isChecked) {
        mRadioButtonLinVst3X->setEnabled(true);
        if (!mRadioButtonLinVst3->isEnabled()) {
            mRadioButtonLinVst3X->setChecked(true);
        }
    } else {
        mRadioButtonLinVst3X->setEnabled(false);
        if (mRadioButtonLinVst3->isEnabled()) {
            mRadioButtonLinVst3->setChecked(true);
        }
    }
}
