#ifndef GEODATANETCDFFILENAMEMATCHER_H
#define GEODATANETCDFFILENAMEMATCHER_H

#include <QRegExp>
#include <QString>

class QDateTime;

class GeoDataNetcdfFileNameMatcher
{
public:
	GeoDataNetcdfFileNameMatcher(const QString& pattern);
	bool setup(const QString& filename);

	QDateTime getDateTime(const QString& filename, bool *ok);

private:
	QString m_pattern;
	QRegExp m_patternRe;

	int m_YYYYCapId;
	int m_MMCapId;
	int m_DDCapId;
	int m_hhCapId;
	int m_mmCapId;
	int m_ssCapId;

	QRegExp m_filenameRe;
};

#endif // GEODATANETCDFFILENAMEMATCHER_H
