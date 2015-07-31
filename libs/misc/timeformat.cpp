#include "mathsupport.h"
#include "stringtool.h"
#include "timeformat.h"

#include <QString>
#include <QVector>

#include <cmath>

#define INTDISPLAY_LIMIT 10000000

QString TimeFormat::formattedString(double seconds, Format format)
{
	QString ret;
	seconds = iRIC::timeSecondValue(seconds);
	switch (format) {
	case fmSecond:
		ret = secondString(seconds);
		ret.append(" sec");
		break;
	case fmMinuteSecond:
		ret = minuteSecondString(seconds);
		break;
	case fmHourMinuteSecond:
		ret = hourMinuteSecondString(seconds);
		break;
	case fmDayHourMinuteSecond:
		ret = dayHourMinuteSecondString(seconds);
		break;
	}
	return ret;
}

QString TimeFormat::secondString(double seconds, int secondLength)
{
	if (seconds > INTDISPLAY_LIMIT) {
		return QString::number(seconds);
	}
	int milisec = static_cast<int>(seconds * 1000);
	int tmpint = milisec - (milisec / 1000) * 1000;
	QVector<int> seclist;
	for (int i = 0; i < 3; ++i) {
		int tmpint2 = tmpint;
		tmpint2 /= 10;
		tmpint2 *= 10;
		seclist.append(tmpint - tmpint2);
		tmpint = tmpint2 / 10;
	}
	int precision = 3;
	for (int i = 0; i < 3; ++i) {
		if (seclist[i] == 0) {
			-- precision;
		} else {
			break;
		}
	}

	double sec = milisec / 1000.;
	if (precision == 0) {
		int intsec = static_cast<int>(sec);
		QString str;
		if (secondLength == 2) {
			str.sprintf("%02d", intsec);
		} else {
			str.sprintf("%d", intsec);
		}
		return str;
	} else {
		QString str;
		QString header;
		QString format("%1.%2f");
		QString f2;
		if (secondLength == 2) {
			header = "%0";
		} else {
			header = "%";
		}
		f2 = format.arg(precision + 3).arg(precision);
		header.append(f2);
		return str.sprintf(iRIC::toStr(header).c_str(), sec);
	}
}

QString TimeFormat::minuteSecondString(double seconds, int minuteLength)
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

QString TimeFormat::hourMinuteSecondString(double seconds, int hourLength)
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

QString TimeFormat::dayHourMinuteSecondString(double seconds)
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
