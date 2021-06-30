#include "iricmetadata.h"
#include "../solverdef/solverdefinitionabstract.h"

#include <misc/errormessage.h>

iRICMetaData::iRICMetaData(const std::string &appPath, const QLocale& locale)
{
	loadMetaData(appPath, locale);
	checkVersionNumber();
}

iRICMetaData::~iRICMetaData()
{}

const QString& iRICMetaData::caption() const
{
	return m_caption;
}

const VersionNumber& iRICMetaData::versionNumber() const
{
	return m_versionNumber;
}

QString iRICMetaData::displayVersionNumber() const
{
	return m_displayVersionNumber;
}

QDate iRICMetaData::releaseDate() const
{
	return m_releaseDate;
}

QString iRICMetaData::copyright() const
{
	return m_copyright;
}

QString iRICMetaData::homeUrl() const
{
	return m_homeUrl;
}

QString iRICMetaData::termsOfUseUrl() const
{
	return m_termsOfUseUrl;
}

void iRICMetaData::loadMetaData(const std::string &appPath, const QLocale& locale)
{
	try {
		SolverDefinitionAbstract abst(appPath.c_str(), locale, nullptr);

		m_caption = abst.caption();
		m_versionNumber = abst.version();
		m_displayVersionNumber = m_versionNumber.toAboutString();
		m_releaseDate = abst.release();
		m_homeUrl = abst.url();
		m_copyright = abst.copyright();
	} catch (...) {
		// if error occurs, skip and exit.
		return;
	}
}

void iRICMetaData::checkVersionNumber()
{
	if (m_versionNumber.build() == 0) {
		throw ErrorMessage(tr("Build number of the version number is not specified."));
	}
}
