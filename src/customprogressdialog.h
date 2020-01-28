#ifndef CUSTOMPROGRESSDIALOG_H
#define CUSTOMPROGRESSDIALOG_H

#include <QDialog>
class QVBoxLayout;
class QHBoxLayout;
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
    QHBoxLayout *mLayoutHBottom;
    QLabel *mLabel;
    QPushButton *mButtonCancel;
    QTimer *mProgressTimer;

signals:
    void signalCancelPressed();

private slots:
    void slotProgressTimerElapsed();

public slots:
    void slotButtonCancel();
};

#endif // CUSTOMPROGRESSDIALOG_H
