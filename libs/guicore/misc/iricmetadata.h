#ifndef IRICMETADATA_H
#define IRICMETADATA_H

#include "../guicore_global.h"

#include <misc/versionnumber.h>

#include <QObject>
#include <QDate>
#include <QString>

#include <string>

class QLocale;
class iRICMainWindow;

/// Meta data about iRIC GUI
class GUICOREDLL_EXPORT iRICMetaData : public QObject
{
	Q_OBJECT

public:
	iRICMetaData(const std::string& appPath, const QLocale& locale);
	~iRICMetaData();

	const QString& caption() const;
	const VersionNumber& versionNumber() const;
	QString displayVersionNumber() const;
	QDate releaseDate() const;
	QString copyright() const;
	QString homeUrl() const;
	QString termsOfUseUrl() const;

private:
	void loadMetaData(const std::string& appPath, const QLocale& locale);
	void checkVersionNumber();

	QString m_caption;
	VersionNumber m_versionNumber;
	QString m_displayVersionNumber;
	QDate m_releaseDate;
	QString m_copyright;
	QString m_homeUrl;
	QString m_termsOfUseUrl;
};

#endif // IRICMETADATA_H
