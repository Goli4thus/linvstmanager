// This file is part of LinVstManager.

#include "verticalline.h"

VerticalLine::VerticalLine()
{
    this->setGeometry(QRect(150, 320, 3, 118));
    this->setFrameShape(QFrame::VLine);
    this->setFrameShadow(QFrame::Sunken);
}
