#ifndef POSTTITLESETTING_H
#define POSTTITLESETTING_H

#include "../postbase_global.h"
#include <QString>
#include <QColor>

class POSTBASEDLL_EXPORT PostTitleSetting
{

public:
	/// Constructor
	PostTitleSetting() {
		fontSize = 20;
		color = Qt::black;
	}
	/// Title
	QString title;
	/// Font size (px)
	int fontSize;
	/// Color
	QColor color;
};

#endif // POSTTITLESETTING_H
