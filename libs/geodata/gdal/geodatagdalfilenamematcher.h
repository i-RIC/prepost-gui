#ifndef GEODATAGDALFILENAMEMATCHER_H
#define GEODATAGDALFILENAMEMATCHER_H

#include <QRegExp>
#include <QString>

class QDateTime;

class GeoDataGdalFileNameMatcher
{
public:
	GeoDataGdalFileNameMatcher(const QString& pattern);
	bool setup(const QString& filename);

	QString pattern() const;
	QDateTime getDateTime(const QString& filename, bool *ok) const;

private:
	QString m_pattern;
	QRegExp m_patternRe;

	int m_YYYYCapId;
	int m_MMCapId;
	int m_DDCapId;
	int m_hhCapId;
	int m_mmCapId;
	int m_ssCapId;
};

#endif // GEODATAGDALFILENAMEMATCHER_H
