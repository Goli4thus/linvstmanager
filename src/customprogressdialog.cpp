// This file is part of LinVstManager.

#include "customprogressdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include "horizontalline.h"
#include <QPushButton>

CustomProgressDialog::CustomProgressDialog()
{
    this->setWindowTitle("Scan");
    mCntVst2 = 0;
    mCntVst3 = 0;
    mCntDll = 0;
    mScanAmount = 0;
    mFoundSoFar = 0;

    mLayoutVMain = new QVBoxLayout();
    mLayoutGridCounter = new QGridLayout();
    mLayoutHBottom = new QHBoxLayout();
    mLabelMain = new QLabel("Scanning for VSTs...", this);
    mProgressBar = new QProgressBar(this);
    auto *hline = new HorizontalLine();
    auto *hline2 = new HorizontalLine();
    mButtonCancel = new QPushButton("Cancel", this);

    auto *mLabelTextVst2 = new QLabel("Vst2:");
    auto *mLabelTextVst3 = new QLabel("Vst3:");
    auto *mLabelTextDll = new QLabel("Dll:");
    auto *mLabelTextInfo = new QLabel("Found so far:");
    mLabelTextDll->setToolTip("Random dll files that are no VSTs.");
    mLabelCounterVst2 = new QLabel("0");
    mLabelCounterVst3 = new QLabel("0");
    mLabelCounterDll = new QLabel("0");

    mLayoutGridCounter->addWidget(mLabelTextVst2,    0, 2, Qt::AlignCenter);
    mLayoutGridCounter->addWidget(mLabelTextVst3,    0, 3, Qt::AlignCenter);
    mLayoutGridCounter->addWidget(mLabelTextDll,     0, 4, Qt::AlignCenter);
    mLayoutGridCounter->addWidget(mLabelTextInfo,    1, 0, 1, 2);
    mLayoutGridCounter->addWidget(mLabelCounterVst2, 1, 2, Qt::AlignCenter);
    mLayoutGridCounter->addWidget(mLabelCounterVst3, 1, 3, Qt::AlignCenter);
    mLayoutGridCounter->addWidget(mLabelCounterDll,  1, 4, Qt::AlignCenter);

    mLayoutVMain->addWidget(mLabelMain);
    mLayoutVMain->addWidget(mProgressBar);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addWidget(hline);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addLayout(mLayoutGridCounter);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addWidget(hline2);
    mLayoutVMain->addSpacing(5);
    mLayoutHBottom->addStretch();
    mLayoutHBottom->addWidget(mButtonCancel);
    mLayoutVMain->addLayout(mLayoutHBottom);
    this->setLayout(mLayoutVMain);

    mProgressBar->setMinimum(0);
    mProgressBar->setMaximum(100);
    mProgressBar->setTextVisible(true);


    connect(mButtonCancel, &QPushButton::pressed, this, &CustomProgressDialog::slotButtonCancel);
}

int CustomProgressDialog::exec()
{
    return QDialog::exec();
}

void CustomProgressDialog::closeEvent(QCloseEvent *e)
{
    return QDialog::closeEvent(e);
}

void CustomProgressDialog::init(int pScanAmount)
{
    if (pScanAmount == 0) {
        // Avoid later divion by zero (just for
        // robustness; should actually come to this)
        pScanAmount = 1;
    }
    mScanAmount = pScanAmount;
    mFoundSoFar = 0;
    mLabelCounterVst2->setText("0");
    mLabelCounterVst3->setText("0");
    mLabelCounterDll->setText("0");
    mCntVst2 = 0;
    mCntVst3 = 0;
    mCntDll = 0;
    mProgressBar->reset();
}

void CustomProgressDialog::updateProgress()
{
    mProgressBar->setValue((++mFoundSoFar * 100) / mScanAmount);
}

void CustomProgressDialog::slotButtonCancel()
{
    emit(signalCancelPressed());
}

void CustomProgressDialog::slotFoundVst3()
{
    updateProgress();
    mLabelCounterVst3->setText(QString::number(++mCntVst3));
}

void CustomProgressDialog::slotFoundVst2()
{
    updateProgress();
    mLabelCounterVst2->setText(QString::number(++mCntVst2));
}

void CustomProgressDialog::slotFoundDll()
{
    updateProgress();
    mLabelCounterDll->setText(QString::number(++mCntDll));
}
