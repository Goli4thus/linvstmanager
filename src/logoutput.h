// This file is part of LinVstManager.

#ifndef LOGOUTPUT_H
#define LOGOUTPUT_H

#include <QObject>
#include <QTextEdit>
#include <QString>
#include <QStringList>
#include <QMenu>
#include <QContextMenuEvent>


class LogOutput : public QTextEdit
{
public:
    LogOutput();
    ~LogOutput();
    void appendLog(const QString &text, bool verbose = false);
    void appendLog(const QStringList &list, bool verbose = false);
    void contextMenuEvent(QContextMenuEvent *event);
    void enableVerboseLog(bool enable);
    bool mVerboseLogEnabled;
private:
    QMenu *menu;
};

#endif // LOGOUTPUT_H
