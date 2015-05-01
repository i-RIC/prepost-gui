#include "solverdefinition.h"
#include "solverdefinitionabstract.h"
#include "solverdefinitiontranslator.h"

#include <misc/errormessage.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomDocument>
#include <QDomNode>
#include <QTextStream>

void SolverDefinitionAbstract::load(const QString& solverfolder, const QLocale& locale)
{
	// holds the copy of the locale.
	m_locale = locale;

	m_folder = QDir(solverfolder);
	m_folderName = m_folder.dirName();

	// Set up translator first.
	SolverDefinitionTranslator translator(solverfolder, locale);
	// get translation file name
	QDomDocument doc;
	setupDomDocument(&doc);
	QDomNode SDNode = doc.documentElement();
	m_name = SDNode.attributes().namedItem("name").nodeValue();
	m_caption = translator.translate(SDNode.attributes().namedItem("caption").nodeValue());
	QString version = SDNode.attributes().namedItem("version").nodeValue();
	m_version.fromString(version);
	m_copyright = SDNode.attributes().namedItem("copyright").nodeValue();
	m_url = SDNode.attributes().namedItem("homepage").nodeValue();
	m_release = QDate::fromString(SDNode.attributes().namedItem("release").nodeValue(), "yyyy.MM.dd");
	if (! m_release.isValid()) {
		m_release = QDate::fromString(SDNode.attributes().namedItem("release").nodeValue(), "yyyy.M.d");
	}
}

const QString SolverDefinitionAbstract::description() const
{
	// get readme filename
	QString filename = SolverDefinition::README;
	filename.append("_").append(m_locale.name());
	// get the local language version README first.
	filename = m_folder.absoluteFilePath(filename);
	if (! QFile::exists(filename)) {
		// local language version does not exists;
		filename = m_folder.absoluteFilePath(SolverDefinition::README);
		if (! QFile::exists(filename)) {
			// README does not exists.
			return "";
		}
	}
	QFile file(filename);
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

const QString SolverDefinitionAbstract::license() const
{
	// get license filename
	QString filename = SolverDefinition::LICENSE;
	filename.append("_").append(m_locale.name());
	// get the local language version LICENSE first.
	filename = m_folder.absoluteFilePath(filename);
	if (! QFile::exists(filename)) {
		// local language version does not exists;
		filename = m_folder.absoluteFilePath(SolverDefinition::LICENSE);
		if (! QFile::exists(filename)) {
			// LICENSE does not exists.
			return "";
		}
	}
	QFile file(filename);
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

void SolverDefinitionAbstract::setupDomDocument(QDomDocument* doc) const
{
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString filename = m_folder.absoluteFilePath(SolverDefinition::FILENAME);
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
