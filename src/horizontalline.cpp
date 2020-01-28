#include "horizontalline.h"

HorizontalLine::HorizontalLine()
{
    this->setGeometry(QRect(320, 150, 118, 3));
    this->setFrameShape(QFrame::HLine);
    this->setFrameShadow(QFrame::Sunken);
}
