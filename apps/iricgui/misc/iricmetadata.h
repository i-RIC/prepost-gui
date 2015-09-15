#ifndef IRICMETADATA_H
#define IRICMETADATA_H

#include <misc/versionnumber.h>
#include <QDate>
#include <QString>

class iRICMainWindow;

/// Meta data about iRIC GUI
class iRICMetaData
{
public:
	/// Constructor
	iRICMetaData(iRICMainWindow* mainW);
	/// Destructor
	~iRICMetaData();

	const VersionNumber versionNumber() const {return m_versionNumber;}
	const QString& displayVersionNumber() const {return m_displayVersionNumber;}
	const QDate& releaseDate() const {return m_releaseDate;}
	const QString& copyright() const {return m_copyright;}
	const QString& homeUrl() const {return m_homeUrl;}
	const QString& termsOfUseUrl() const {return m_termsOfUseUrl;}

private:
	void loadMetaData(iRICMainWindow *mainW);
	void loadIricInfoXml();
	void loadDefinitionXml(iRICMainWindow *mainW);
	void checkVersionNumber();

	VersionNumber m_versionNumber;
	QString m_displayVersionNumber;
	QDate m_releaseDate;
	QString m_copyright;
	QString m_homeUrl;
	QString m_termsOfUseUrl;
};

#endif // IRICMETADATA_H
