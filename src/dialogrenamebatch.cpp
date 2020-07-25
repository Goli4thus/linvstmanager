// This file is part of LinVstManager.

#include "dialogrenamebatch.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include "horizontalline.h"
#include <QMessageBox>
#include <QPushButton>
#include <QKeyEvent>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QAction>
#include <QShortcut>


DialogRenameBatch::DialogRenameBatch(const QVector<VstBucket> &pVstBuckets)
    : mVstBuckets(pVstBuckets)
{
    this->setWindowTitle("Batch rename VSTs");
    this->setWindowIcon(QIcon(":/icons/linvstmanager_72.png"));

    mLayoutVMain = new QVBoxLayout(this);
    mLayoutHMode = new QHBoxLayout();
    mLayoutHLocation = new QHBoxLayout();
    mLayoutHNameNew = new QHBoxLayout();
    mLayoutHBottom = new QHBoxLayout();
    mTextEdit = new QTextEdit();
    mRBModeAdd = new QRadioButton("Add");
    mRBModeRemove = new QRadioButton("Remove");
    mButtonGroupMode = new QButtonGroup();
    mRBAtStart = new QRadioButton("Start");
    mRBAtEnd = new QRadioButton("End");
    mButtonGroupLocation = new QButtonGroup();
    mLineEditPhrase = new QLineEdit();
    auto *hLine = new HorizontalLine();
    mButtonAccept = new QPushButton("Accept");
    mButtonCancel = new QPushButton("Cancel");
    auto *mLabelMode = new QLabel("Mode: ");
    auto *mLabelLocation = new QLabel("Location: ");
    auto *mLabelNew = new QLabel("Phrase: ");

    mTextEdit->setReadOnly(true);

    mLabelMode->setFixedWidth(45);
    mLabelMode->setToolTip("'Add': Phrase will be added.\n" \
                           "'Remove': Length of phrase will be removed.\n" \
                           "Toggle mode with 'Alt-m'");
    mButtonGroupMode->addButton(mRBModeRemove);
    mButtonGroupMode->addButton(mRBModeAdd);
    mLayoutHMode->addWidget(mLabelMode);
    mLayoutHMode->addWidget(mRBModeAdd);
    mLayoutHMode->addWidget(mRBModeRemove);
    mLabelLocation->setFixedWidth(45);
    mLabelLocation->setToolTip("Toggle location with 'Alt-l'");
    mButtonGroupLocation->addButton(mRBAtStart);
    mButtonGroupLocation->addButton(mRBAtEnd);
    mLayoutHLocation->addWidget(mLabelLocation);
    mLayoutHLocation->addWidget(mRBAtStart);
    mLayoutHLocation->addWidget(mRBAtEnd);
    mRBModeAdd->setChecked(true);
    mRBAtEnd->setChecked(true);
    mModeAdd = true;
    mAtEnd = true;

    mLabelNew->setFixedWidth(45);
    mLineEditPhrase->setMinimumWidth(130);
    mLayoutHNameNew->addWidget(mLabelNew);
    mLayoutHNameNew->addWidget(mLineEditPhrase);

    mLayoutHBottom->addStretch();
    mLayoutHBottom->addWidget(mButtonCancel);
    mLayoutHBottom->addWidget(mButtonAccept);

    mLayoutVMain->addWidget(mTextEdit);
    mLayoutVMain->addLayout(mLayoutHMode);
    mLayoutVMain->addLayout(mLayoutHLocation);
    mLayoutVMain->addLayout(mLayoutHNameNew);
    mLayoutVMain->addWidget(hLine);
    mLayoutVMain->addLayout(mLayoutHBottom);

    this->setLayout(mLayoutVMain);

    new QShortcut(QKeySequence(Qt::ALT + Qt::Key_M), this, SLOT(slotModeToggle()));
    new QShortcut(QKeySequence(Qt::ALT + Qt::Key_L), this, SLOT(slotLocationToggle()));

    connect(mButtonAccept, &QPushButton::pressed, this, &DialogRenameBatch::slotButtonAccept);
    connect(mButtonCancel, &QPushButton::pressed, this, &DialogRenameBatch::slotButtonCancel);
    connect(mLineEditPhrase, &QLineEdit::textChanged, this, &DialogRenameBatch::slotTextChanged);
    connect(mButtonGroupMode, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, &DialogRenameBatch::slotModeChanged);
    connect(mButtonGroupLocation, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, &DialogRenameBatch::slotLocationChanged);
}

void DialogRenameBatch::init(QVector<int> indices)
{
    mIndices = indices;
    mTextEdit->clear();
    QString html;
    for (const auto &i : mIndices) {
        html.append(mVstBuckets.at(i).name + "<br>");
    }
    mTextEdit->setHtml(html);
    mLineEditPhrase->clear();
    mLineEditPhrase->setFocus();
}

void DialogRenameBatch::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        slotButtonAccept();
    QDialog::keyPressEvent(event);
}

void DialogRenameBatch::slotButtonAccept()
{
    emit(signalRenameBatchAccept(mIndices, mModeAdd, mAtEnd, mLineEditPhrase->text()));
    QVector<VstBridge> tmp;
    emit(signalConfigDataChanged(false, tmp));
    this->close();
}

void DialogRenameBatch::slotButtonCancel()
{
    this->close();
}

void DialogRenameBatch::slotTextChanged()
{
    QString html;
    QString fontHlAddStart = "<font color=\"Cyan\">";
    QString fontHlAddEnd = "</font>";
    QString fontHlRmStart = "<font color=\"Red\">";
    QString fontHlRmEnd = "</font>";

    html.append("<p>");
    for (const auto i : mIndices) {
        if (mModeAdd) {
            if (mAtEnd) {
                html.append(mVstBuckets.at(i).name +
                            fontHlAddStart +
                            mLineEditPhrase->text() +
                            fontHlAddEnd +
                            "<br>");
            } else {
                html.append(fontHlAddStart +
                            mLineEditPhrase->text() +
                            fontHlAddEnd +
                            mVstBuckets.at(i).name +
                            "<br>");
            }
        } else { // Remove
            int len = mLineEditPhrase->text().length();
            QString name = mVstBuckets.at(i).name;
            if (len > name.length()) {
                len = name.length();
            }
            if (mAtEnd) {
                name.insert(name.length() - len, fontHlRmStart);
                name.append(fontHlRmEnd);
                html.append(name + "<br>");
            } else {
                name.insert(len, fontHlRmEnd);
                name.prepend(fontHlRmStart);
                html.append(name + "<br>");
            }
        }
    }
    html.append("</p>");

    mTextEdit->setHtml(html);
}

void DialogRenameBatch::slotModeChanged(int id)
{
    Q_UNUSED(id)
    mModeAdd = mRBModeAdd->isChecked();
    slotTextChanged();
}

void DialogRenameBatch::slotLocationChanged(int id)
{
    Q_UNUSED(id)
    mAtEnd = mRBAtEnd->isChecked();
    slotTextChanged();
}

void DialogRenameBatch::slotModeToggle()
{
    if (mRBModeAdd->isChecked()) {
        mRBModeRemove->setChecked(true);
    } else {
        mRBModeAdd->setChecked(true);
    }
    slotModeChanged(0);
}

void DialogRenameBatch::slotLocationToggle()
{
    if (mRBAtEnd->isChecked()) {
        mRBAtStart->setChecked(true);
    } else {
        mRBAtEnd->setChecked(true);
    }
    slotLocationChanged(0);
}
