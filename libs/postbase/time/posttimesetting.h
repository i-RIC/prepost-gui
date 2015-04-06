#ifndef POSTTIMESETTING_H
#define POSTTIMESETTING_H

#include "../postbase_global.h"
#include <misc/timeformat.h>
#include <QColor>

class POSTBASEDLL_EXPORT PostTimeSetting {
public:
	/// Constructor
	PostTimeSetting(){
		timeFormat = TimeFormat::fmSecond;
		color = Qt::black;
	}
	/// Time format
	TimeFormat::Format timeFormat;
	/// Color
	QColor color;
};

#endif // POSTTIMESETTING_H
