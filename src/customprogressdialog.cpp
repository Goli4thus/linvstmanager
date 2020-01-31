// This file is part of LinVstManager.

#include "customprogressdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include "horizontalline.h"
#include <QPushButton>
#include <QTimer>

CustomProgressDialog::CustomProgressDialog()
{
    this->setWindowTitle("Scan");

    mLayoutVMain = new QVBoxLayout();
    mLayoutGridCounter = new QGridLayout();
    mLayoutHBottom = new QHBoxLayout();
    mLabelMain = new QLabel("Scanning for VSTs...", this);
    mProgressBar = new QProgressBar(this);
    HorizontalLine *hline = new HorizontalLine();
    HorizontalLine *hline2 = new HorizontalLine();
    mButtonCancel = new QPushButton("Cancel", this);

    QLabel *mLabelTextVst2 = new QLabel("Vst2:");
    QLabel *mLabelTextVst3 = new QLabel("Vst3:");
    QLabel *mLabelTextDll = new QLabel("Dll:");
    QLabel *mLabelTextInfo = new QLabel("Found so far:");
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

    mProgressTimer = new QTimer(this);

    connect(mButtonCancel, &QPushButton::pressed, this, &CustomProgressDialog::slotButtonCancel);
    connect(mProgressTimer, &QTimer::timeout, this, &CustomProgressDialog::slotProgressTimerElapsed);
}

int CustomProgressDialog::exec()
{
    mLabelCounterVst2->setText("0");
    mLabelCounterVst3->setText("0");
    mLabelCounterDll->setText("0");
    mCntVst2 = 0;
    mCntVst3 = 0;
    mCntDll = 0;
    mProgressBar->reset();
    mProgressTimer->start(500); // 500 ms interval timer
    return QDialog::exec();
}

void CustomProgressDialog::closeEvent(QCloseEvent *e)
{
    mProgressTimer->stop();
    return QDialog::closeEvent(e);
}

void CustomProgressDialog::slotProgressTimerElapsed()
{
    int currentValue = mProgressBar->value();
    if (currentValue < 90) {
        mProgressBar->setValue(currentValue + 1);
        // Let timer run
    } else {
        // Stop timer at approx. 90 percent and simply wait.
        mProgressTimer->stop();
        mLabelMain->setText("Well, this seems to take longer than expected (> 45s).");
    }
}

void CustomProgressDialog::slotButtonCancel()
{
    emit(signalCancelPressed());
}

void CustomProgressDialog::slotFoundVst3()
{
    mLabelCounterVst3->setText(QString::number(++mCntVst3));
}

void CustomProgressDialog::slotFoundVst2()
{
    mLabelCounterVst2->setText(QString::number(++mCntVst2));
}

void CustomProgressDialog::slotFoundDll()
{
    mLabelCounterDll->setText(QString::number(++mCntDll));
}
