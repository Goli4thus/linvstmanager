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

    mLayoutVMain = new QVBoxLayout(this);
    mLayoutHBottom = new QHBoxLayout(this);
    mLabel = new QLabel("Scanning for VSTs...", this);
    mProgressBar = new QProgressBar(this);
    HorizontalLine *hline = new HorizontalLine();
    mButtonCancel = new QPushButton("Cancel", this);

    mLayoutVMain->addWidget(mLabel);
    mLayoutVMain->addWidget(mProgressBar);
    mLayoutVMain->addSpacing(5);
    mLayoutVMain->addWidget(hline);
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
        mLabel->setText("Well, this seems to take longer than expected (> 45s).");
    }
}

void CustomProgressDialog::slotButtonCancel()
{
    emit(signalCancelPressed());
}
