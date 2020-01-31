// This file is part of LinVstManager.

#ifndef LINEEDITBRIDGE_H
#define LINEEDITBRIDGE_H

#include <QWidget>

class QHBoxLayout;
class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

class LineEditBridge : public QWidget
{
    Q_OBJECT
public:
    explicit LineEditBridge(QString t_name, QWidget *parent = nullptr);
    bool getBridgeEnabled();
    bool pathIsSet();
    QString getPath();
    void setBridgeEnabled(bool setEnabled);
    void setPath(QString path);
private:
    QHBoxLayout *mLayoutHBridge;
    QLabel *mLabelBridge;
    QLineEdit *mLineEditBridge;
    QCheckBox *mCheckBoxBridge;
    QPushButton *mPushButtonBridge;
    QString name;
    QPalette *palette;
    QPalette *paletteDefault;

private slots:
    void slotCheckBoxBridge();
    void slotPushButtonSelectBridge();

signals:
    void signalCheckBoxBridge(bool);
};

#endif // LINEEDITBRIDGE_H
