// This file is part of LinVstManager.

#ifndef DIALOGRENAME_H
#define DIALOGRENAME_H

#include <QObject>
#include <QDialog>
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QSpacerItem;
class QDialogButtonBox;
class QPushButton;
#include "vstbucket.h"

class DialogRename : public QDialog
{
    Q_OBJECT;

public:
    DialogRename(const QVector<VstBucket> &pVstBuckets);
    void init(int indexNameConflict);
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVBoxLayout *mLayoutVMain;
    QHBoxLayout *mLayoutHNameConflict;
    QHBoxLayout *mLayoutHNameNew;
    QHBoxLayout *mLayoutHBottom;
    QLineEdit *mLineEditNameConflict;
    QLineEdit *mLineEditNameNew;
    QPushButton *mButtonAccept;
    QPushButton *mButtonCancel;
    const QVector<VstBucket> &mVstBuckets;
    int mIndexNameOld;

private slots:
    void slotbuttonAccept();
    void slotButtonCancel();

signals:
    void signalRenameAccept(int index, QString nameNew);
    void signalConfigDataChanged(bool needsRefresh, QVector<VstBridge> changedBridges);
};

#endif // DIALOGRENAME_H
