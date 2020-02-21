// This file is part of LinVstManager.

#ifndef SCANWORKER_H
#define SCANWORKER_H

#include <QObject>
#include "enums.h"
#include <QMap>
#include <QVector>
#include "scanresult.h"
#include <QStringList>

class ScanWorker : public QObject
{
    Q_OBJECT
public:
    explicit ScanWorker(const quint8 pWorkerID,
                        const QStringList pFindingPaths,
                        bool pUseCheckBasic,
                        const QByteArrayList pExistingPathHashes,
                        QObject *parent = nullptr);
    static ArchType checkArch(const QString &findingAbsPath);

private:
    void verifyDll(bool &verified, VstType &vstType, VstProbabilityType &vstProbability, const QString &finding);
    bool checkDllBasic(const QString &findingAbsPath, bool caseSensitive, const QString &pattern);
    const quint8 mWorkerID;
    const QStringList mFindingPaths;
    const bool mUseCheckBasic;
    const QByteArrayList mExistingPathHashes;

signals:
    void signalScanWorkerFinished(const quint8 pWorkerID,
                                  bool wasCanceled,
                                  QVector<ScanResult> scanResults);
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();

public slots:
    void slotPerformScan();

};

#endif // SCANWORKER_H
