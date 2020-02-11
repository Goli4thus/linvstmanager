// This file is part of LinVstManager.

#include "dialogrename.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include "horizontalline.h"
#include <QMessageBox>
#include <QPushButton>
#include <QKeyEvent>


DialogRename::DialogRename(const QVector<VstBucket> &pVstBuckets)
    : mVstBuckets(pVstBuckets)
{
    this->setWindowTitle("Rename VST");
    this->setWindowIcon(QIcon(":/icons/linvstmanager_72.png"));

    mLayoutVMain = new QVBoxLayout(this);
    mLayoutHNameConflict = new QHBoxLayout();
    mLayoutHNameNew = new QHBoxLayout();
    mLayoutHBottom = new QHBoxLayout();
    mLineEditNameConflict = new QLineEdit();
    mLineEditNameNew = new QLineEdit();
    auto *hLine = new HorizontalLine();
    mButtonAccept = new QPushButton("Accept");
    mButtonCancel = new QPushButton("Cancel");
    auto *mLabelConflict = new QLabel("Name (conflict): ");
    auto *mLabelNew = new QLabel("Name (new): ");

    mLabelConflict->setFixedWidth(90);
    mLabelNew->setFixedWidth(90);
    mLineEditNameConflict->setReadOnly(true);
    mLineEditNameConflict->setMinimumWidth(130);
    mLineEditNameNew->setMinimumWidth(130);

    mLayoutHNameConflict->addWidget(mLabelConflict);
    mLayoutHNameConflict->addWidget(mLineEditNameConflict);

    mLayoutHNameNew->addWidget(mLabelNew);
    mLayoutHNameNew->addWidget(mLineEditNameNew);

    mLayoutHBottom->addStretch();
    mLayoutHBottom->addWidget(mButtonCancel);
    mLayoutHBottom->addWidget(mButtonAccept);

    mLayoutVMain->addLayout(mLayoutHNameConflict);
    mLayoutVMain->addLayout(mLayoutHNameNew);
    mLayoutVMain->addWidget(hLine);
    mLayoutVMain->addLayout(mLayoutHBottom);

    this->setLayout(mLayoutVMain);


    connect(mButtonAccept, &QPushButton::pressed, this, &DialogRename::slotButtonAccept);
    connect(mButtonCancel, &QPushButton::pressed, this, &DialogRename::slotButtonCancel);
    connect(mLineEditNameNew, &QLineEdit::textChanged, this, &DialogRename::slotTextChanged);
}

void DialogRename::init(int indexNameConflict)
{
    mIndexNameOld = indexNameConflict;
    QString nameOld = mVstBuckets.at(mIndexNameOld).name;

    // pre-fill lineEdits
    if (mVstBuckets.at(mIndexNameOld).status == VstStatus::Conflict) {
        mLineEditNameConflict->setText(nameOld);
        mLineEditNameConflict->setEnabled(true);
        mLineEditNameNew->setStyleSheet("color: red");
    } else {
        // VST can't be in conflict with itself. It's a simple rename.
        mLineEditNameConflict->setText("");
        mLineEditNameConflict->setEnabled(false);
    }
    mLineEditNameNew->setText(nameOld);
    mLineEditNameNew->setFocus();
}

void DialogRename::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        slotButtonAccept();
    QDialog::keyPressEvent(event);
}

void DialogRename::slotButtonAccept()
{
    bool alreadyExists = false;
    QString nameConflict;
    QString pathConflict;
    QString nameNew = mLineEditNameNew->text();
    for (const auto &vstBucket : mVstBuckets) {
        if (vstBucket.name == nameNew) {
            alreadyExists = true;
            nameConflict = vstBucket.name;
            pathConflict = vstBucket.vstPath;
        }
    }

    if (alreadyExists) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Name already taken");
        msgBox.setText("The newly entered name is already taken.");

        QStringList details = QStringList() << "Conflicting VST:\n"
                                            << "Name: \"" << nameConflict << "\"\n"
                                            << "Path: \"" << pathConflict << "\"\n";

        msgBox.setDetailedText(details.join(""));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    } else {
        emit(signalRenameAccept(mIndexNameOld, nameNew));
        QVector<VstBridge> tmp;
        emit(signalConfigDataChanged(true, tmp));
        this->close();
    }
}

void DialogRename::slotButtonCancel()
{
    this->close();
}

void DialogRename::slotTextChanged()
{
    // TODO: Color new name lineedit text red whenever it matches the current conflicting name
    if (mLineEditNameNew->text() == mLineEditNameConflict->text()) {
        mLineEditNameNew->setStyleSheet("color: red");
    } else {
        mLineEditNameNew->setStyleSheet("");
    }
}
