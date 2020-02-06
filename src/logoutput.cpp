// This file is part of LinVstManager.

#include "logoutput.h"
#include "QTime"

LogOutput::LogOutput()
{
    this->setReadOnly(true);
    menu = createStandardContextMenu();
    mVerboseLogEnabled = false;

    auto *clearLog = new QAction("Clear", this);

    menu->addSeparator();
    menu->addAction(clearLog);

    connect(clearLog, SIGNAL(triggered()), this, SLOT(clear()));
}

LogOutput::~LogOutput()
{
    delete menu;
}

void LogOutput::appendLog(const QString &text, bool verbose)
{
    bool printLog;
    if (verbose && mVerboseLogEnabled) {
        printLog = true;
    } else if (!verbose) {
        printLog = true;
    } else {
        printLog = false;
    }

    if (printLog) {
        QString time = QTime::currentTime().toString();
        this->append(QString("%1").arg(time.leftJustified(15, ' ')) + text);
        this->ensureCursorVisible();
    }
}

void LogOutput::appendLog(const QStringList &list, bool verbose)
{
    bool printLog;
    if (verbose && mVerboseLogEnabled) {
        printLog = true;
    } else if (!verbose) {
        printLog = true;
    } else {
        printLog = false;
    }

    if (printLog) {
        QString time = QTime::currentTime().toString();
        this->append(QString("%1").arg(time.leftJustified(15, ' ')) + list.at(0));
        for (int i = 1; i < list.size(); ++i) {
            this->append(QString("%1").arg(time.leftJustified(15, ' ')) + list.at(i));
        }
        this->ensureCursorVisible();
    }
}

void LogOutput::contextMenuEvent(QContextMenuEvent *event)
{
    menu->exec(event->globalPos());
}

void LogOutput::enableVerboseLog(bool enable)
{
    mVerboseLogEnabled = enable;
}
