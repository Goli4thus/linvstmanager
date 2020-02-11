#include "sidebar.h"
#include "statisticline.h"
#include <QVBoxLayout>
#include <QLabel>
#include "colors.h"

SideBar::SideBar(QWidget *parent) : QWidget(parent)
{
    this->setParent(parent);

    mLayoutVMain = new QVBoxLayout();
    mStatsLineEnabled = new StatisticLine(VstStatus::Enabled, "Enabled", Colors::getColorGreen());
    mStatsLineDisabled = new StatisticLine(VstStatus::Disabled, "Disabled", Colors::getColorYellow());
    mStatsLineMismatch = new StatisticLine(VstStatus::Mismatch, "Mismatch", Colors::getColorBlue());
    mStatsLineNo_So = new StatisticLine(VstStatus::No_So, "No_So", Colors::getColorBlue());
    mStatsLineNoBridge = new StatisticLine(VstStatus::NoBridge, "NoBridge", Colors::getColorRed());
    mStatsLineConflict = new StatisticLine(VstStatus::Conflict, "Conflict", Colors::getColorRed());
    mStatsLineNotFound = new StatisticLine(VstStatus::NotFound, "NotFound", Colors::getColorViolette());
    mStatsLineBlacklisted = new StatisticLine(VstStatus::Blacklisted, "Blacklisted", Colors::getColorBlack());
    mLabelSelection = new QLabel("Selection:     0  /  X");

    mLayoutVMain->addWidget(mStatsLineEnabled);
    mLayoutVMain->addWidget(mStatsLineDisabled);
    mLayoutVMain->addWidget(mStatsLineMismatch);
    mLayoutVMain->addWidget(mStatsLineNo_So);
    mLayoutVMain->addWidget(mStatsLineNoBridge);
    mLayoutVMain->addWidget(mStatsLineConflict);
    mLayoutVMain->addWidget(mStatsLineNotFound);
    mLayoutVMain->addWidget(mStatsLineOrphan);
    mLayoutVMain->addWidget(mStatsLineNA);
    mLayoutVMain->addWidget(mStatsLineBlacklisted);
    mLayoutVMain->addStretch();
    mLayoutVMain->addWidget(mLabelSelection);

    this->setLayout(mLayoutVMain);

    connect(mStatsLineEnabled, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
    connect(mStatsLineDisabled, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
    connect(mStatsLineMismatch, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
    connect(mStatsLineNo_So, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
    connect(mStatsLineNoBridge, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
    connect(mStatsLineConflict, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
    connect(mStatsLineNotFound, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
    connect(mStatsLineBlacklisted, &StatisticLine::signalFilerRequest, this, &SideBar::signalFilerRequest);
}

void SideBar::slotUpdateCounts(const QVector<int> &count)
{
//    mLineEditCount->setText(QString::number(count));
}

void SideBar::slotUpdateSelection(const int &numSel, const int &numAll)
{
    mLabelSelection->setText(QString("Selection:   %1  /  %2").arg(numSel).arg(numAll));
}
