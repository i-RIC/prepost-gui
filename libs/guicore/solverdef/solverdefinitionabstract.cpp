#include "solverdefinition.h"
#include "solverdefinitionabstract.h"
#include "solverdefinitiontranslator.h"
#include "private/solverdefinitionabstract_impl.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDate>
#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QString>
#include <QTextStream>

SolverDefinitionAbstract::Impl::Impl(SolverDefinitionAbstract* parent) :
	m_parent {parent}
{}

QString SolverDefinitionAbstract::Impl::loadFile(const QString& filename)
{
	QString fname = filename;
	fname.append("_%1");
	// get the local language version first.
	fname = m_folder.absoluteFilePath(fname.arg(m_locale.name()));
	if (! QFile::exists(fname)) {
		// local language version does not exists;
		fname = m_folder.absoluteFilePath(filename);
		if (! QFile::exists(fname)) {
			// global file does not exists.
			return "";
		}
	}

	QFile file(fname);
	QString ret, buffer;
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream in(&file);
	in.setCodec("UTF-8");
	while (! in.atEnd()) {
		buffer = in.readLine();
		ret.append(buffer);
		ret.append("\n");
	}
	return ret;
}

void SolverDefinitionAbstract::Impl::load(const QString& solverfolder, const QLocale& locale)
{
	// holds the copy of the locale.
	m_locale = locale;

	m_folder = QDir(solverfolder);
	m_folderName = m_folder.dirName();

	// Set up translator first.
	SolverDefinitionTranslator translator(solverfolder, locale);
	// get translation file name
	QDomDocument doc;
	m_parent->setupDomDocument(&doc);
	QDomNode SDNode = doc.documentElement();
	QDomElement SDElem = SDNode.toElement();

	m_name = iRIC::toStr(SDElem.attribute("name"));
	m_caption = translator.translate(SDElem.attribute("caption"));
	m_version = VersionNumber {SDElem.attribute("version")};
	m_copyright = SDElem.attribute("copyright");
	m_url = SDElem.attribute("homepage");
	m_release = QDate::fromString(SDElem.attribute("release"), "yyyy.MM.dd");
	if (! m_release.isValid()) {
		m_release = QDate::fromString(SDElem.attribute("release"), "yyyy.M.d");
	}
}

SolverDefinitionAbstract::SolverDefinitionAbstract(const QString& solverfolder, const QLocale& locale, QObject* parent) :
	QObject {parent},
	m_impl {new Impl {this}}
{
	m_impl->load(solverfolder, locale);
}

const QString& SolverDefinitionAbstract::folderName() const
{
	return m_impl->m_folderName;
}

const std::string& SolverDefinitionAbstract::name() const
{
	return m_impl->m_name;
}

const QString& SolverDefinitionAbstract::caption() const
{
	return m_impl->m_caption;
}

const VersionNumber& SolverDefinitionAbstract::version() const
{
	return m_impl->m_version;
}

QString SolverDefinitionAbstract::description() const
{
	return m_impl->loadFile(SolverDefinition::README);
}

const QString& SolverDefinitionAbstract::url() const
{
	return m_impl->m_url;
}

const QString& SolverDefinitionAbstract::copyright() const
{
	return m_impl->m_copyright;
}

const QDate& SolverDefinitionAbstract::release() const
{
	return m_impl->m_release;
}

QString SolverDefinitionAbstract::license() const
{
	return m_impl->loadFile(SolverDefinition::LICENSE);
}

const QDir& SolverDefinitionAbstract::folder() const
{
	return m_impl->m_folder;
}

void SolverDefinitionAbstract::setupDomDocument(QDomDocument* doc) const
{
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString filename = m_impl->m_folder.absoluteFilePath(SolverDefinition::FILENAME);
	QFile file(filename);
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc->setContent(&file, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(filename).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
	QDomElement docElem = doc->documentElement();
	iRIC::removeAllComments(&docElem);
}
