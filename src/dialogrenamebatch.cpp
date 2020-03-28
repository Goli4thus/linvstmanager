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
    mLayoutHNameNew = new QHBoxLayout();
    mLayoutHBottom = new QHBoxLayout();
    mTextEdit = new QTextEdit();
    mRBModePrepend = new QRadioButton("Prepend");
    mRBModeAppend = new QRadioButton("Append");
    mButtonGroupMode = new QButtonGroup();
    mLineEditPhrase = new QLineEdit();
    auto *hLine = new HorizontalLine();
    mButtonAccept = new QPushButton("Accept");
    mButtonCancel = new QPushButton("Cancel");
    auto *mLabelMode = new QLabel("Mode: ");
    auto *mLabelNew = new QLabel("Phrase: ");

    mTextEdit->setReadOnly(true);

    mLabelMode->setFixedWidth(45);
    mLabelMode->setToolTip("Toggle with 'Alt-m'");
    mButtonGroupMode->addButton(mRBModePrepend);
    mButtonGroupMode->addButton(mRBModeAppend);
    mLayoutHMode->addWidget(mLabelMode);
    mLayoutHMode->addWidget(mRBModePrepend);
    mLayoutHMode->addWidget(mRBModeAppend);
    mRBModeAppend->setChecked(true);
    mModeAppend = true;

    mLabelNew->setFixedWidth(45);
    mLineEditPhrase->setMinimumWidth(130);
    mLayoutHNameNew->addWidget(mLabelNew);
    mLayoutHNameNew->addWidget(mLineEditPhrase);

    mLayoutHBottom->addStretch();
    mLayoutHBottom->addWidget(mButtonCancel);
    mLayoutHBottom->addWidget(mButtonAccept);

    mLayoutVMain->addWidget(mTextEdit);
    mLayoutVMain->addLayout(mLayoutHMode);
    mLayoutVMain->addLayout(mLayoutHNameNew);
    mLayoutVMain->addWidget(hLine);
    mLayoutVMain->addLayout(mLayoutHBottom);

    this->setLayout(mLayoutVMain);

    new QShortcut(QKeySequence(Qt::ALT + Qt::Key_M), this, SLOT(slotModeToggle()));

    connect(mButtonAccept, &QPushButton::pressed, this, &DialogRenameBatch::slotButtonAccept);
    connect(mButtonCancel, &QPushButton::pressed, this, &DialogRenameBatch::slotButtonCancel);
    connect(mLineEditPhrase, &QLineEdit::textChanged, this, &DialogRenameBatch::slotTextChanged);
    connect(mButtonGroupMode, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &DialogRenameBatch::slotModeChanged);
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
//    bool alreadyExists = false;
//    QString nameConflict;
//    QString pathConflict;
//    QString nameNew = mLineEditPhrase->text();
//    for (const auto &vstBucket : mVstBuckets) {
//        if (vstBucket.name == nameNew) {
//            alreadyExists = true;
//            nameConflict = vstBucket.name;
//            pathConflict = vstBucket.vstPath;
//        }
//    }

//    if (alreadyExists) {
//        QMessageBox msgBox(this);
//        msgBox.setIcon(QMessageBox::Information);
//        msgBox.setWindowTitle("Name already taken");
//        msgBox.setText("The newly entered name is already taken.");

//        QStringList details = QStringList() << "Conflicting VST:\n"
//                                            << "Name: \"" << nameConflict << "\"\n"
//                                            << "Path: \"" << pathConflict << "\"\n";

//        msgBox.setDetailedText(details.join(""));
//        msgBox.setStandardButtons(QMessageBox::Ok);
//        msgBox.exec();
//    } else {
//        emit(signalRenameAccept(mIndexNameOld, nameNew));
//        QVector<VstBridge> tmp;
//        emit(signalConfigDataChanged(false, tmp));
//        this->close();
//    }
}

void DialogRenameBatch::slotButtonCancel()
{
    this->close();
}

void DialogRenameBatch::slotTextChanged()
{
    QString html;
    QString fontRedStart = "<font color=\"Red\">";
    QString fontRedEnd = "</font>";

    html.append("<p>");
    for (const auto i : mIndices) {
        if (mModeAppend) {
            html.append(mVstBuckets.at(i).name +
                        fontRedStart +
                        mLineEditPhrase->text() +
                        fontRedEnd +
                        "<br>");
        } else {
            html.append(fontRedStart +
                        mLineEditPhrase->text() +
                        fontRedEnd +
                        mVstBuckets.at(i).name +
                        "<br>");
        }
    }
    html.append("</p>");

    mTextEdit->setHtml(html);
}

void DialogRenameBatch::slotModeChanged(int id)
{
    Q_UNUSED(id)
    mModeAppend = mRBModeAppend->isChecked();
    slotTextChanged();
}

void DialogRenameBatch::slotModeToggle()
{
    if (mRBModeAppend->isChecked()) {
        mRBModePrepend->setChecked(true);
    } else {
        mRBModeAppend->setChecked(true);
    }
    slotModeChanged(0);
}
