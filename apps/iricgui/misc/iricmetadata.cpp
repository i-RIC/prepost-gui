#include "iricmetadata.h"

#include "main/iricmainwindow.h"
#include <guicore/solverdef/solverdefinitionabstract.h>

#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QApplication>

iRICMetaData::iRICMetaData(iRICMainWindow *mainW)
{
	loadMetaData(mainW);
}

iRICMetaData::~iRICMetaData()
{}

void iRICMetaData::loadMetaData(iRICMainWindow *mainW)
{
	// load internal xml first.
	loadIricInfoXml();
	// load definition.xml if it exists, and overwrite.
	loadDefinitionXml(mainW);
}

void iRICMetaData::loadIricInfoXml()
{
	QFile f(":/data/iricinfo.xml");
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		return;
	}
	QDomElement docElem = doc.documentElement();

	m_versionNumber.fromString(docElem.attribute("version"));
	m_displayVersionNumber = m_versionNumber.toAboutString();

	if (docElem.hasAttribute("displayVersion")) {
		m_displayVersionNumber = docElem.attribute("displayVersion");
	}
	m_releaseDate = QDate::fromString(docElem.attribute("release"), "yyyy/MM/dd");
	m_homeUrl = docElem.attribute("url");
	m_termsOfUseUrl = docElem.attribute("tou");
	m_copyright = docElem.attribute("copyright");
}

void iRICMetaData::loadDefinitionXml(iRICMainWindow *mainW)
{
	QLocale locale = mainW->locale();
	try {
		SolverDefinitionAbstract abst(qApp->applicationDirPath(), locale, nullptr);

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
