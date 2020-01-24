#ifndef LINKHANDLER_H
#define LINKHANDLER_H

#include <QObject>
#include "enums.h"

class LinkHandler : public QObject
{
    Q_OBJECT
public:
    explicit LinkHandler(QObject *parent = nullptr);
    RvLinkHandler enableVst();
    RvLinkHandler disableVst();
    RvLinkHandler updateVst();
    RvLinkHandler refreshStatus();
    RvLinkHandler checkForOrphans();
    RvLinkHandler removeOrphans();

private:
    bool checkSoFileMatch();

signals:

public slots:

};

#endif // LINKHANDLER_H
