#ifndef CUSTOMPROGRESSDIALOG_H
#define CUSTOMPROGRESSDIALOG_H

#include <QDialog>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;

class CustomProgressDialog : public QDialog
{
    Q_OBJECT
public:
    CustomProgressDialog();
    QProgressBar *mProgressBar;
    int exec();
    void closeEvent(QCloseEvent *e);

private:
    QVBoxLayout *mLayoutVMain;
    QGridLayout *mLayoutGridCounter;
    QHBoxLayout *mLayoutHBottom;
    QLabel *mLabelMain;
    QLabel *mLabelCounterVst2;
    QLabel *mLabelCounterVst3;
    QLabel *mLabelCounterDll;
    QPushButton *mButtonCancel;
    QTimer *mProgressTimer;
    quint16 mCntVst2;
    quint16 mCntVst3;
    quint16 mCntDll;

signals:
    void signalCancelPressed();

private slots:
    void slotProgressTimerElapsed();

public slots:
    void slotButtonCancel();
    void slotFoundVst3();
    void slotFoundVst2();
    void slotFoundDll();
};

#endif // CUSTOMPROGRESSDIALOG_H
