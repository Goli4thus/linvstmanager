// This file is part of LinVstManager.

#ifndef COLORS_H
#define COLORS_H

#include <QColor>

class Colors
{
public:
    static QColor getColorGreen() { return QColor(87, 196, 25); }
    static QColor getColorYellow() { return QColor(255, 212, 41); }
    static QColor getColorRed() { return QColor(226, 85, 37); }
    static QColor getColorBlue() { return QColor(91, 160, 255); }
    static QColor getColorViolette() { return QColor(182, 0, 231); }
    static QColor getColorLightGrey() { return QColor(220, 220, 220); }
    static QColor getColorBlack() { return QColor(51, 51, 51); }

private:
    Colors() {}
};

#endif // COLORS_H
