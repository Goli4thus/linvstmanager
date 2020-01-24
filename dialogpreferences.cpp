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

    setupUI();
    fillPreferences();
}

void DialogPreferences::setupUI()
{
    // ===========
    // = Bridges =
    // ===========
    mLineEditLinVst = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVst), this);
    mLineEditLinVstX = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVstX), this);
    mLineEditLinVst3 = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVst3), this);
    mLineEditLinVst3X = new LineEditBridge(mMapBridgeStr->value(VstBridge::LinVst3X), this);
//    mLineEditLinVst->setStyleSheet("background-color:blue;");

    mLayoutVBridges = new QVBoxLayout();
    mLayoutVBridges->addWidget(mLineEditLinVst);
    mLayoutVBridges->addWidget(mLineEditLinVstX);
    mLayoutVBridges->addWidget(mLineEditLinVst3);
    mLayoutVBridges->addWidget(mLineEditLinVst3X);
    mLayoutVBridges->setSpacing(-5);
    mLayoutVBridges->update();
    this->adjustSize();

    mGroupBoxBridges = new QGroupBox("Bridges:");
    mGroupBoxBridges->setLayout(mLayoutVBridges);

    // ============
    // = Defaults =
    // ============

    mRadioButtonLinVst = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVst));
    mRadioButtonLinVstX = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVstX));
    mRadioButtonLinVst3 = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVst3));
    mRadioButtonLinVst3X = new QRadioButton(mMapBridgeStr->value(VstBridge::LinVst3X));
    mLayoutVDefaultsVst2 = new QVBoxLayout();
    mLayoutVDefaultsVst2->addWidget(mRadioButtonLinVst);
    mLayoutVDefaultsVst2->addWidget(mRadioButtonLinVstX);
    mLayoutVDefaultsVst3 = new QVBoxLayout();
    mLayoutVDefaultsVst3->addWidget(mRadioButtonLinVst3);
    mLayoutVDefaultsVst3->addWidget(mRadioButtonLinVst3X);

    mButtonGroupDefaultVst2 = new QButtonGroup();
    mButtonGroupDefaultVst2->addButton(mRadioButtonLinVst);
    mButtonGroupDefaultVst2->addButton(mRadioButtonLinVstX);
    mButtonGroupDefaultVst3 = new QButtonGroup();
    mButtonGroupDefaultVst3->addButton(mRadioButtonLinVst3);
    mButtonGroupDefaultVst3->addButton(mRadioButtonLinVst3X);

    mLayoutHDefaults = new QHBoxLayout();
    mLayoutHDefaults->addWidget(new QLabel("VST2:"));
    mLayoutHDefaults->addSpacing(5);
    mLayoutHDefaults->addLayout(mLayoutVDefaultsVst2, 0);
    mLayoutHDefaults->addSpacing(40);
    VerticalLine *vLine1 = new VerticalLine();
    mLayoutHDefaults->addWidget(vLine1);
    mLayoutHDefaults->addSpacing(30);
    mLayoutHDefaults->addWidget(new QLabel("VST3:"));
    mLayoutHDefaults->addSpacing(5);
    mLayoutHDefaults->addLayout(mLayoutVDefaultsVst3, 0);
    mLayoutHDefaults->addStretch();

    mGroupBoxDefaults = new QGroupBox("Defaults:");
    mGroupBoxDefaults->setLayout(mLayoutHDefaults);

    // ===========
    // = General =
    // ===========
    // link folder
    mLineEditLinkFolder = new QLineEdit();
    mLineEditLinkFolder->setReadOnly(true);
    mLineEditLinkFolder->setToolTip("Specify the folder that shall contain all the bridged *.so files.");
    mPushButtonLinkFolder = new QPushButton("Select");
    mLayoutHLinkFolder = new QHBoxLayout();
    labelLinkFolder = new QLabel("Link folder:");
    labelLinkFolder->setMinimumWidth(80);
    mLayoutHLinkFolder->addWidget(labelLinkFolder);
    mLayoutHLinkFolder->addWidget(mLineEditLinkFolder);
    mLayoutHLinkFolder->addWidget(mPushButtonLinkFolder);

    mLayoutVGeneral = new QVBoxLayout();
    mLayoutVGeneral->addLayout(mLayoutHLinkFolder);

    mGroupBoxGeneral = new QGroupBox("General:");
    mGroupBoxGeneral->setLayout(mLayoutVGeneral);

    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                      QDialogButtonBox::Cancel);

    mLayoutVMain = new QVBoxLayout();
    mLayoutVMain->addWidget(mGroupBoxBridges);
    mLayoutVMain->addWidget(mGroupBoxDefaults);
    mLayoutVMain->addWidget(mGroupBoxGeneral);

    HorizontalLine *line1 = new HorizontalLine();
    HorizontalLine *line2 = new HorizontalLine();

    mLayoutVMain->addWidget(line1);
    mLayoutVMain->addSpacing(10);
    mLayoutVMain->addWidget(new QLabel("Hint: At least the following things need to be set for this to work:\n"
                                       "                - one or more bridges\n"
                                       "                - the link folder\n"));
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
    mLineEditLinVst->setBridgeEnabled(prf->getEnabledLinVst());
    mLineEditLinVstX->setBridgeEnabled(prf->getEnabledLinVstX());
    mLineEditLinVst3->setBridgeEnabled(prf->getEnabledLinVst3());
    mLineEditLinVst3X->setBridgeEnabled(prf->getEnabledLinVst3X());

    mLineEditLinVst->setPath(prf->getPathSoLinVst());
    mLineEditLinVstX->setPath(prf->getPathSoLinVstX());
    mLineEditLinVst3->setPath(prf->getPathSoLinVst3());
    mLineEditLinVst3X->setPath(prf->getPathSoLinVst3X());

    // Defaults
    mRadioButtonLinVst->setEnabled(prf->getEnabledLinVst());
    mRadioButtonLinVstX->setEnabled(prf->getEnabledLinVstX());
    mRadioButtonLinVst3->setEnabled(prf->getEnabledLinVst3());
    mRadioButtonLinVst3X->setEnabled(prf->getEnabledLinVst3X());

    if (prf->getBridgeDefaultVst2IsX()) {
        mRadioButtonLinVstX->setChecked(true);
    } else {
        mRadioButtonLinVst->setChecked(true);
    }

    if (prf->getBridgeDefaultVst3IsX()) {
        mRadioButtonLinVst3X->setChecked(true);
    } else {
        mRadioButtonLinVstX->setChecked(true);
    }

    // General
    mLineEditLinkFolder->setText(prf->getPathLinkFolder());
}

void DialogPreferences::slotButtonSelectLinkFolder()
{
    QString path_linkFolder = QFileDialog::getExistingDirectory(this,
                                         "Specify the folder that shall contain all the bridged *.so files.",
                                         QDir::homePath());
    if (!path_linkFolder.isEmpty()) {
        mLineEditLinkFolder->setText(path_linkFolder);
    }
}

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
        prefsChanged = prf->updatePreferences(mLineEditLinVst->getBridgeEnabled(),
                                              mLineEditLinVstX->getBridgeEnabled(),
                                              mLineEditLinVst3->getBridgeEnabled(),
                                              mLineEditLinVst3X->getBridgeEnabled(),
                                              mLineEditLinVst->getPath(),
                                              mLineEditLinVstX->getPath(),
                                              mLineEditLinVst3->getPath(),
                                              mLineEditLinVst3X->getPath(),
                                              mLineEditLinkFolder->text(),
                                              mRadioButtonLinVstX->isChecked(),
                                              mRadioButtonLinVst3X->isChecked());
        if (prefsChanged) {
            emit(signalConfigDataChanged());
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
