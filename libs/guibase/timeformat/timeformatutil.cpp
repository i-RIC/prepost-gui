#include "timeformatutil.h"

#include <misc/mathsupport.h>

#include <QDateTime>
#include <QLocale>

#include <cmath>

namespace  {

const int INTDISPLAY_LIMIT = 10000000;

QString SS_String(double seconds, int secondLength = 0)
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

QString SS_sec_String(double seconds) {
	auto str = SS_String(seconds);
	str.append(" sec");
	return str;
}

QString MM_SS_String(double seconds, int minuteLength = 0)
{
	QString fmt("%1:%2");
	double minutes = std::floor(seconds / 60);
	if (minutes > INTDISPLAY_LIMIT) {
		return fmt.arg(minutes, minuteLength).arg(SS_String(seconds - minutes * 60, 2));
	} else {
		int minutes = static_cast<int>(seconds / 60);
		return fmt.arg(minutes, minuteLength, static_cast<int>(10), QChar('0')).arg(SS_String(seconds - minutes * 60, 2));
	}
}

QString HH_MM_SS_String(double seconds, int hourLength = 0)
{
	double hours = std::floor(seconds / 3600);
	if (hours == 0) {return MM_SS_String(seconds);}
	QString fmt("%1:%2");
	if (hours > INTDISPLAY_LIMIT) {
		return fmt.arg(hours, hourLength).arg(MM_SS_String(seconds - hours * 3600, 2));
	} else {
		int hours = static_cast<int>(seconds / 3600);
		return fmt.arg(hours, hourLength, static_cast<int>(10), QChar('0')).arg(MM_SS_String(seconds - hours * 3600., 2));
	}
}

QString DD_HH_MM_SS_String(double seconds)
{
	double days = std::floor(seconds / 86400);
	if (days == 0) {return HH_MM_SS_String(seconds);}
	if (days > INTDISPLAY_LIMIT) {
		QString fmt("%1:%2");
		return fmt.arg(days).arg(HH_MM_SS_String(seconds - days * 86400, 2));
	} else {
		int days = static_cast<int>(seconds / 86400);
		QString fmt("%1:%2");
		return fmt.arg(days).arg(HH_MM_SS_String(seconds - days * 86400., 2));
	}
}

} // namespace

QString TimeFormatUtil::formattedString(const QDateTime& zeroDateTime, double elapsed, TimeFormat format, const QString& customFormat)
{
	if (format == TimeFormat::elapsed_SS_sec) {
		return SS_sec_String(elapsed);
	} else if (format == TimeFormat::elapsed_MM_SS) {
		return MM_SS_String(elapsed);
	} else if (format == TimeFormat::elapsed_HH_MM_SS) {
		return HH_MM_SS_String(elapsed);
	} else if (format == TimeFormat::elapsed_DD_HH_MM_SS) {
		return DD_HH_MM_SS_String(elapsed);
	} else {
		QDateTime time = zeroDateTime;
		if (time.isNull()) {
			return SS_sec_String(elapsed);
		}

		time = time.addSecs(static_cast<qint64>(elapsed));
		QString formatStr;
		if (format == TimeFormat::actual_yyyy_mm_dd_HH_MM_SS) {
			formatStr = "yyyy/MM/dd HH:mm:ss";
		} else if (format == TimeFormat::actual_yyyy_mm_dd_HH_MM) {
			formatStr = "yyyy/MM/dd HH:mm";
		} else if (format == TimeFormat::actual_mm_dd_HH_MM_SS) {
			formatStr = "MM/dd HH:mm:ss";
		} else if (format == TimeFormat::actual_mm_dd_HH_MM) {
			formatStr = "MM/dd HH:mm";
		} else if (format == TimeFormat::actual_HH_MM_SS) {
			formatStr = "HH:mm:ss";
		} else if (format == TimeFormat::actual_HH_MM) {
			formatStr = "HH:mm";
		} else if (format == TimeFormat::actual_Custom) {
			formatStr = customFormat;
		}
		auto locale = QLocale(QLocale::English, QLocale::Country::UnitedStates);
		return locale.toString(time, formatStr);
	}
}

QString TimeFormatUtil::formatString(TimeFormat format, const QString& customFormat)
{
	if (format == TimeFormat::elapsed_SS_sec) {
		return "SS sec";
	} else if (format == TimeFormat::elapsed_MM_SS) {
		return "MM:SS";
	} else if (format == TimeFormat::elapsed_HH_MM_SS) {
		return "HH:MM:SS";
	} else if (format == TimeFormat::elapsed_DD_HH_MM_SS) {
		return "DD:HH:MM:SS";
	} else if (format == TimeFormat::actual_yyyy_mm_dd_HH_MM_SS) {
		return "yyyy/mm/dd HH:MM:SS";
	} else if (format == TimeFormat::actual_yyyy_mm_dd_HH_MM) {
		return "yyyy/mm/dd HH:MM";
	} else if (format == TimeFormat::actual_mm_dd_HH_MM_SS) {
		return "mm/dd HH:MM:SS";
	} else if (format == TimeFormat::actual_mm_dd_HH_MM) {
		return "mm/dd HH:MM";
	} else if (format == TimeFormat::actual_HH_MM_SS) {
		return "HH:MM:SS";
	} else if (format == TimeFormat::actual_HH_MM) {
		return "HH:MM";
	} else if (format == TimeFormat::actual_Custom) {
		return customFormat;
	} else {
		return "SS sec";
	}
}

QString TimeFormatUtil::toString(TimeFormat format)
{
	if (format == TimeFormat::elapsed_SS_sec) {
		return "elapsed|SS sec";
	} else if (format == TimeFormat::elapsed_MM_SS) {
		return "elapsed|MM:SS";
	} else if (format == TimeFormat::elapsed_HH_MM_SS) {
		return "elapsed|HH:MM:SS";
	} else if (format == TimeFormat::elapsed_DD_HH_MM_SS) {
		return "elapsed|DD:HH:MM:SS";
	} else if (format == TimeFormat::actual_yyyy_mm_dd_HH_MM_SS) {
		return "actual|yyyy/mm/dd HH:MM:SS";
	} else if (format == TimeFormat::actual_yyyy_mm_dd_HH_MM) {
		return "actual|yyyy/mm/dd HH:MM";
	} else if (format == TimeFormat::actual_mm_dd_HH_MM_SS) {
		return "actual|mm/dd HH:MM:SS";
	} else if (format == TimeFormat::actual_mm_dd_HH_MM) {
		return "actual|mm/dd HH:MM";
	} else if (format == TimeFormat::actual_HH_MM_SS) {
		return "actual|HH:MM:SS";
	} else if (format == TimeFormat::actual_HH_MM) {
		return "actual|HH:MM";
	} else if (format == TimeFormat::actual_Custom) {
		return "actual|custom";
	} else {
		return "elapsed|SS sec";
	}
}

TimeFormat TimeFormatUtil::fromString(const QString& format)
{
	if (format == "elapsed|SS sec") {
		return TimeFormat::elapsed_SS_sec;
	} else if (format == "elapsed|MM:SS") {
		return TimeFormat::elapsed_MM_SS;
	} else if (format == "elapsed|HH:MM:SS") {
		return TimeFormat::elapsed_HH_MM_SS;
	} else if (format == "elapsed|DD:HH:MM:SS") {
		return TimeFormat::elapsed_DD_HH_MM_SS;
	} else if (format == "actual|yyyy/mm/dd HH:MM:SS") {
		return TimeFormat::actual_yyyy_mm_dd_HH_MM_SS;
	} else if (format == "actual|yyyy/mm/dd HH:MM") {
		return TimeFormat::actual_yyyy_mm_dd_HH_MM;
	} else if (format == "actual|mm/dd HH:MM:SS") {
		return TimeFormat::actual_mm_dd_HH_MM_SS;
	} else if (format == "actual|mm/dd HH:MM") {
		return TimeFormat::actual_mm_dd_HH_MM;
	} else if (format == "actual|HH:MM:SS") {
		return TimeFormat::actual_HH_MM_SS;
	} else if (format == "actual|HH:MM") {
		return TimeFormat::actual_HH_MM;
	} else if (format == "actual|custom") {
		return TimeFormat::actual_Custom;
	} else {
		return TimeFormat::elapsed_SS_sec;
	}
}
