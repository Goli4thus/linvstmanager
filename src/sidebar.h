// This file is part of LinVstManager.

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
class StatisticLine;
class QVBoxLayout;
class QLabel;
#include "enums.h"
#include "vstbucket.h"

class SideBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideBar(const QVector<VstBucket> &pVstBuckets, QWidget *parent = nullptr);

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
    const QVector<VstBucket> &mVstBuckets;

public slots:
    void slotUpdateCounts();
    void slotUpdateSelection(const int &numSel, const int &numAll);

signals:
    void signalFilerRequest(VstStatus status);
};

#endif // SIDEBAR_H
