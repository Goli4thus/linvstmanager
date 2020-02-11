#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
class StatisticLine;
class QVBoxLayout;
class QLabel;
#include "enums.h"

class SideBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = nullptr);

private:
    QVBoxLayout *mLayoutVMain;
    StatisticLine *mStatsLineEnabled;
    StatisticLine *mStatsLineDisabled;
    StatisticLine *mStatsLineMismatch;
    StatisticLine *mStatsLineNo_So;
    StatisticLine *mStatsLineNoBridge;
    StatisticLine *mStatsLineConflict;
    StatisticLine *mStatsLineNotFound;
    StatisticLine *mStatsLineOrphan;
    StatisticLine *mStatsLineNA;
    StatisticLine *mStatsLineBlacklisted;
    QLabel *mLabelSelection;

public slots:
    void slotUpdateCounts(const QVector<int> &count);
    void slotUpdateSelection(const int &numSel, const int &numAll);

signals:
    void signalFilerRequest(VstStatus status);
};

#endif // SIDEBAR_H
