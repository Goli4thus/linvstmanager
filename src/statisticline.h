// This file is part of LinVstManager.

#ifndef STATISTICLINE_H
#define STATISTICLINE_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QHBoxLayout;
class QPushButton;
#include "enums.h"
class QColor;

class StatisticLine : public QWidget
{
    Q_OBJECT
public:
    StatisticLine(const VstStatus &pVstStatus,
                  const QString &pLabelText,
                  const QColor &pButtonColor,
                  QWidget *parent = nullptr);
    void setCount(int count);

private:
    QHBoxLayout *mLayoutH;
    QPushButton *mButtonFilter;
    QLabel *mLabel;
    QLineEdit *mLineEditCount;
    const VstStatus mVstStatus;
    const QString mLabelText;

public slots:
    void slotSetActive(const bool &setActive);
    void slotButtonFilterPressed();

signals:
    void signalFilerRequest(VstStatus status);
};

#endif // STATISTICLINE_H
