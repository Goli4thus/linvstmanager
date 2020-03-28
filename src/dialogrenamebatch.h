// This file is part of LinVstManager.

#ifndef DIALOGRENAMEBATCH_H
#define DIALOGRENAMEBATCH_H

#include <QObject>
#include <QDialog>
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QSpacerItem;
class QDialogButtonBox;
class QPushButton;
class QTextEdit;
class QRadioButton;
class QButtonGroup;
#include "vstbucket.h"

class DialogRenameBatch : public QDialog
{
    Q_OBJECT;

public:
    DialogRenameBatch(const QVector<VstBucket> &pVstBuckets);
    void init(QVector<int> indices);
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVBoxLayout *mLayoutVMain;
    QHBoxLayout *mLayoutHMode;
    QHBoxLayout *mLayoutHLocation;
    QHBoxLayout *mLayoutHNameNew;
    QHBoxLayout *mLayoutHBottom;
    QTextEdit *mTextEdit;
    QLineEdit *mLineEditPhrase;
    QRadioButton *mRBModeRemove;
    QRadioButton *mRBModeAdd;
    QButtonGroup *mButtonGroupMode;
    QRadioButton *mRBAtStart;
    QRadioButton *mRBAtEnd;
    QButtonGroup *mButtonGroupLocation;
    QPushButton *mButtonAccept;
    QPushButton *mButtonCancel;
    const QVector<VstBucket> &mVstBuckets;
    QVector<int> mIndices;
    bool mModeAdd;
    bool mAtEnd;

private slots:
    void slotButtonAccept();
    void slotButtonCancel();
    void slotTextChanged();
    void slotModeChanged(int id);
    void slotLocationChanged(int id);
    void slotModeToggle();
    void slotLocationToggle();

signals:
    void signalRenameBatchAccept(QVector<int> indices, bool modeAdd, bool atEnd, QString phrase);
    void signalConfigDataChanged(bool needsRefresh, QVector<VstBridge> changedBridges);
};

#endif // DIALOGRENAMEBATCH_H
