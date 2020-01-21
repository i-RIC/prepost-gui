#include "geodatanetcdffilenamematcher.h"

#include <QDate>
#include <QDateTime>
#include <QTime>

#include <algorithm>
#include <vector>

namespace {

const QString YYYY = "YYYY";
const QString MM = "MM";
const QString DD = "DD";
const QString hh = "hh";
const QString mm = "mm";
const QString ss = "ss";

int indexOf(const std::vector<int>& vec, int val)
{
	auto it = std::find(vec.begin(), vec.end(), val);
	return static_cast<int>(it - vec.begin());
}

int setupCapId(int idx, const std::vector<int>& indices)
{
	if (idx == -1) {return -1;}
	return indexOf(indices, idx) + 1;
}

} // namespace

GeoDataNetcdfFileNameMatcher::GeoDataNetcdfFileNameMatcher(const QString& pattern)
{
	int YYYY_idx = pattern.indexOf(YYYY);
	int MM_idx = pattern.indexOf(MM);
	int DD_idx = pattern.indexOf(DD);
	int hh_idx = pattern.indexOf(hh);
	int mm_idx = pattern.indexOf(mm);
	int ss_idx = pattern.indexOf(ss);

	std::vector<int> indices;
	if (YYYY_idx != -1) {indices.push_back(YYYY_idx);}
	if (MM_idx != -1) {indices.push_back(MM_idx);}
	if (DD_idx != -1) {indices.push_back(DD_idx);}
	if (hh_idx != -1) {indices.push_back(hh_idx);}
	if (mm_idx != -1) {indices.push_back(mm_idx);}
	if (ss_idx != -1) {indices.push_back(ss_idx);}

	std::sort(indices.begin(), indices.end());

	m_YYYYCapId = setupCapId(YYYY_idx, indices);
	m_MMCapId = setupCapId(MM_idx, indices);
	m_DDCapId = setupCapId(DD_idx, indices);
	m_hhCapId = setupCapId(hh_idx, indices);
	m_mmCapId = setupCapId(mm_idx, indices);
	m_ssCapId = setupCapId(ss_idx, indices);

	QString regexpPattern = pattern;
	regexpPattern = regexpPattern.replace("YYYY", "(\\d{4})");
	regexpPattern = regexpPattern.replace("MM", "(\\d{2})");
	regexpPattern = regexpPattern.replace("DD", "(\\d{2})");
	regexpPattern = regexpPattern.replace("hh", "(\\d{2})");
	regexpPattern = regexpPattern.replace("mm", "(\\d{2})");
	regexpPattern = regexpPattern.replace("ss", "(\\d{2})");

	m_patternRe = QRegExp(regexpPattern);
}

bool GeoDataNetcdfFileNameMatcher::setup(const QString& filename)
{
	int pos = m_patternRe.indexIn(filename);
	if (pos == -1) {return false;}

	QString prefix = filename.left(pos);
	QString suffix = filename.right(filename.length() - pos - m_patternRe.cap().length());

	QString fnamePattern = (prefix + ".{%1}" + suffix).arg(m_patternRe.cap().length());
	m_filenameRe = QRegExp(fnamePattern);

	return true;
}

QDateTime GeoDataNetcdfFileNameMatcher::getDateTime(const QString& filename, bool* ok)
{
	auto current = QDate::currentDate();
	int year = current.year();
	int month = current.month();
	int day = current.day();
	int hour = 0;
	int minute = 0;
	int second = 0;

	int pos = m_filenameRe.indexIn(filename);
	if (pos == -1) {
		*ok = false;
		return QDateTime::currentDateTime();
	}

	pos = m_patternRe.indexIn(filename);
	if (pos == -1) {
		*ok = false;
		return QDateTime::currentDateTime();
	}

	*ok = true;

	if (m_YYYYCapId != -1) {
		year = m_patternRe.cap(m_YYYYCapId).toInt();
	}
	if (m_MMCapId != -1) {
		month = m_patternRe.cap(m_MMCapId).toInt();
	}
	if (m_DDCapId != -1) {
		day = m_patternRe.cap(m_DDCapId).toInt();
	}
	if (m_hhCapId != -1) {
		hour = m_patternRe.cap(m_hhCapId).toInt();
	}
	if (m_mmCapId != -1) {
		minute = m_patternRe.cap(m_mmCapId).toInt();
	}
	if (m_ssCapId != -1) {
		second = m_patternRe.cap(m_ssCapId).toInt();
	}
	QDate date(year, month, day);
	QTime time(hour, minute, second);

	return QDateTime(date, time, Qt::UTC);
}
