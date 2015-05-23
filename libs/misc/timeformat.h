#ifndef TIMEFORMAT_H
#define TIMEFORMAT_H

#include "misc_global.h"

class QString;

/// Support class for handling time format
class MISCDLL_EXPORT TimeFormat
{
public:
	enum Format {
		fmSecond, ///< ss sec
		fmMinuteSecond, ///< mm:ss
		fmHourMinuteSecond, ///< hh:mm:ss
		fmDayHourMinuteSecond ///< dd:hh:mm:ss
	};
	/// Returns formatted time string from seconds value
	static const QString formattedString(double seconds, Format format);

private:
	static const QString secondString(double seconds, int secondLength = 0);
	static const QString minuteSecondString(double seconds, int minuteLength = 0);
	static const QString hourMinuteSecondString(double seconds, int hourLength = 0);
	static const QString dayHourMinuteSecondString(double seconds);
};

#endif // TIMEFORMAT_H
