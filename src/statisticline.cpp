// This file is part of LinVstManager.

#include "statisticline.h"
#include <QLabel>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include "colors.h"

StatisticLine::StatisticLine(const VstStatus &pVstStatus,
                             const QString &pLabelText,
                             const QColor &pButtonColor,
                             QWidget *parent)
    : mVstStatus(pVstStatus), mLabelText(pLabelText)
{
    mLayoutH = new QHBoxLayout(parent);
    mButtonFilter = new QPushButton();
    mLabel = new QLabel(mLabelText + ": ");
    mLineEditCount = new QLineEdit();

    mButtonFilter->setFixedSize(20, 20);
    QStringList btnStyleSheet = QStringList() << "background-color: rgb("
                                              <<  QString::number(pButtonColor.red()) << ", "
                                              <<  QString::number(pButtonColor.green()) << ", "
                                              <<  QString::number(pButtonColor.blue()) << ");"
                                              << " border: none;";
    mButtonFilter->setStyleSheet(btnStyleSheet.join(""));
    mButtonFilter->setToolTip("Click to filter.\nClick again to clear.");
    mLineEditCount->setFixedWidth(41);
    mLineEditCount->setAlignment(Qt::AlignRight);
    mLineEditCount->setFixedHeight(22);
    mLineEditCount->setReadOnly(true);
    mLabel->setFixedWidth(60);

    mLayoutH->addWidget(mButtonFilter);
    mLayoutH->addWidget(mLabel);
    mLayoutH->addWidget(mLineEditCount);
    mLayoutH->addStretch();
    mLayoutH->setSpacing(5);
    mLayoutH->setMargin(0);
    mLayoutH->setAlignment(Qt::AlignVCenter);
    this->adjustSize();

    this->setLayout(mLayoutH);

    connect(mButtonFilter, &QPushButton::pressed, this, &StatisticLine::slotButtonFilterPressed);
}

void StatisticLine::slotSetActive(const bool &setActive)
{
    if (setActive) {
        mButtonFilter->setEnabled(true);
        mLabel->setEnabled(true);
        mLineEditCount->setEnabled(true);
    } else {
        mButtonFilter->setEnabled(false);
        mLabel->setEnabled(false);
        mLineEditCount->setEnabled(false);
    }
}

void StatisticLine::slotButtonFilterPressed()
{
    emit(signalFilerRequest(mVstStatus));
}

void StatisticLine::setCount(int count)
{
    mLineEditCount->setText(QString::number(count));
}
