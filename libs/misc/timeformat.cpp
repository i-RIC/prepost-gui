#include "mathsupport.h"
#include "stringtool.h"
#include "timeformat.h"

#include <QString>
#include <QVector>

#include <cmath>

#define INTDISPLAY_LIMIT 10000000

namespace {
	QString secondString(double seconds, int secondLength = 0)
	{
		if (secondLength == 2) {
			// show in "00" format, to be used for "3:00", for example.
			int sec = static_cast<int>(seconds);
			int milisec = static_cast<int>((seconds - sec) * 1000);
			QString sec_str, milisec_str;
			sec_str.sprintf("%02d", sec);
			milisec_str.sprintf("%03d", milisec);
			while (milisec_str.length() > 0 && milisec_str.at(milisec_str.length() - 1) == '0') {
				milisec_str.chop(1);
			}
			if (milisec_str.length() > 0) {
				return sec_str + "." + milisec_str;
			} else {
				return sec_str;
			}
		} else {
			// show in normal format
			return iRIC::timeSecondValueStr(seconds);
		}
	}

	QString minuteSecondString(double seconds, int minuteLength = 0)
	{
		QString fmt("%1:%2");
		double minutes = std::floor(seconds / 60);
		if (minutes > INTDISPLAY_LIMIT) {
			return fmt.arg(minutes, minuteLength).arg(secondString(seconds - minutes * 60, 2));
		} else {
			int minutes = static_cast<int>(seconds / 60);
			return fmt.arg(minutes, minuteLength, static_cast<int>(10), QChar('0')).arg(secondString(seconds - minutes * 60, 2));
		}
	}

	QString hourMinuteSecondString(double seconds, int hourLength = 0)
	{
		double hours = std::floor(seconds / 3600);
		if (hours == 0) {return minuteSecondString(seconds);}
		QString fmt("%1:%2");
		if (hours > INTDISPLAY_LIMIT) {
			return fmt.arg(hours, hourLength).arg(minuteSecondString(seconds - hours * 3600, 2));
		} else {
			int hours = static_cast<int>(seconds / 3600);
			return fmt.arg(hours, hourLength, static_cast<int>(10), QChar('0')).arg(minuteSecondString(seconds - hours * 3600., 2));
		}
	}

	QString dayHourMinuteSecondString(double seconds)
	{
		double days = std::floor(seconds / 86400);
		if (days == 0) {return hourMinuteSecondString(seconds);}
		if (days > INTDISPLAY_LIMIT) {
			QString fmt("%1:%2");
			return fmt.arg(days).arg(hourMinuteSecondString(seconds - days * 86400, 2));
		} else {
			int days = static_cast<int>(seconds / 86400);
			QString fmt("%1:%2");
			return fmt.arg(days).arg(hourMinuteSecondString(seconds - days * 86400., 2));
		}
	}
}

QString TimeFormat::formattedString(double seconds, Format format)
{
	QString ret;
	seconds = iRIC::timeSecondValue(seconds);
	switch (format) {
	case Format::Second:
		ret = secondString(seconds);
		ret.append(" sec");
		break;
	case Format::MinuteSecond:
		ret = minuteSecondString(seconds);
		break;
	case Format::HourMinuteSecond:
		ret = hourMinuteSecondString(seconds);
		break;
	case Format::DayHourMinuteSecond:
		ret = dayHourMinuteSecondString(seconds);
		break;
	}
	return ret;
}
