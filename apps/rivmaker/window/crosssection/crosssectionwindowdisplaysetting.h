#ifndef CROSSSECTIONWINDOWDISPLAYSETTING_H
#define CROSSSECTIONWINDOWDISPLAYSETTING_H

#include <QColor>
#include <QFont>

class CrossSectionWindowDisplaySetting
{
public:
	CrossSectionWindowDisplaySetting();

	QColor XSColor;
	QColor WSEColor;
	QColor HWMColor;
	QFont HWMFont;
};

#endif // CROSSSECTIONWINDOWDISPLAYSETTING_H
