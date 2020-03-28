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
    QHBoxLayout *mLayoutHNameNew;
    QHBoxLayout *mLayoutHBottom;
    QTextEdit *mTextEdit;
    QLineEdit *mLineEditPhrase;
    QRadioButton *mRBModePrepend;
    QRadioButton *mRBModeAppend;
    QButtonGroup *mButtonGroupMode;
    QPushButton *mButtonAccept;
    QPushButton *mButtonCancel;
    const QVector<VstBucket> &mVstBuckets;
    QVector<int> mIndices;
    bool mModeAppend;

private slots:
    void slotButtonAccept();
    void slotButtonCancel();
    void slotTextChanged();
    void slotModeChanged(int id);
    void slotModeToggle();

signals:
    void signalRenameAccept(int index, QString nameNew);
    void signalConfigDataChanged(bool needsRefresh, QVector<VstBridge> changedBridges);
};

#endif // DIALOGRENAMEBATCH_H
