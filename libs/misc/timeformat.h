#ifndef TIMEFORMAT_H
#define TIMEFORMAT_H

#include "misc_global.h"

class QString;

/// Support class for handling time format
class MISCDLL_EXPORT TimeFormat
{

public:
	enum class Format {
		Second, ///< ss sec
		MinuteSecond, ///< mm:ss
		HourMinuteSecond, ///< hh:mm:ss
		DayHourMinuteSecond ///< dd:hh:mm:ss
	};
	/// Returns formatted time string from seconds value
	static QString formattedString(double seconds, Format format);
};

#endif // TIMEFORMAT_H
