#ifndef TIMEFORMATUTIL_H
#define TIMEFORMATUTIL_H

#include "../guibase_global.h"
#include "timeformat.h"

class QDateTime;
class QString;

class GUIBASEDLL_EXPORT TimeFormatUtil
{
public:
	static QString formattedString(const QDateTime& zeroDateTime, double elapsed, TimeFormat format, const QString& customFormat);
	static QString formatString(TimeFormat format, const QString& customFormat);
	static QString toString(TimeFormat format);
	static TimeFormat fromString(const QString& format);

private:
	TimeFormatUtil();
};

#endif // TIMEFORMATUTIL_H
