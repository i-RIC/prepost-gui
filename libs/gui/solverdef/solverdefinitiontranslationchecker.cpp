#include "solverdefinitiontranslationchecker.h"

#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiontranslator.h>
#include <misc/errormessage.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QTextStream>
#include <QXmlStreamWriter>

void SolverDefinitionTranslationChecker::TranslationMessage::save(QXmlStreamWriter& writer) const
{
	writer.writeStartElement("message");
	writer.writeTextElement("source", m_source);

	writer.writeStartElement("translation");
	/*
		// type ("obsolete" or "unfinished") are not output any more.
		switch (m_messageType){
		case TypeObsolete:
			writer.writeAttribute("type", "obsolete");
			break;
		case TypeUnfinished:
			writer.writeAttribute("type", "unfinished");
			break;
		case TypeNormal:
		default:
			break;
		}
	*/
	writer.writeCharacters(m_translation);
	writer.writeEndElement();

	writer.writeEndElement();
}

void SolverDefinitionTranslationChecker::load(const QString& solverfolder, const QLocale& locale)
{
	// get translation file name
	QDir solFolder(solverfolder);
	QString trFilename = solFolder.absoluteFilePath(SolverDefinitionTranslator::filenameFromLocale(locale));
	if (QFile::exists(trFilename)) {
		// load dictionary.
		QDomDocument doc;
		QString errorStr;
		int errorLine;
		int errorColumn;
		QFile trFile(trFilename);
		QString errorHeader = "Error occured while loading %1\n";
		bool ok = doc.setContent(&trFile, &errorStr, &errorLine, &errorColumn);
		if (! ok) {
			QString msg = errorHeader;
			msg.append("Parse error %2 at line %3, column %4");
			msg = msg.arg(trFilename).arg(errorStr).arg(errorLine).arg(errorColumn);
			throw ErrorMessage(msg);
		}
		QDomElement docElem = doc.documentElement();
		iRIC::removeAllComments(&docElem);
		QDomNode TSNode = doc.documentElement();
		QDomNode contextNode = TSNode.firstChild();
		while (! contextNode.isNull()) {
			if (contextNode.nodeName() != "context") {
				QString msg = errorHeader;
				msg.append("Only context node is allowed under TS node, but %1 node is found.");
				msg = msg.arg(trFilename).arg(contextNode.nodeName());
				throw ErrorMessage(msg);
			}
			QDomNode messageNode = contextNode.firstChild();
			while (! messageNode.isNull()) {
				if (messageNode.nodeName() == "name") {
					// skip this node
					messageNode = messageNode.nextSibling();
					continue;
				}
				if (messageNode.nodeName() != "message") {
					QString msg = errorHeader;
					msg.append("Only name or message node is allowed under context node, but %1 node is found.");
					msg = msg.arg(trFilename).arg(messageNode.nodeName());
					throw ErrorMessage(msg);
				}
				QDomNode sourceNode = iRIC::getChildNode(messageNode, "source");
				QString sourceString = iRIC::getText(sourceNode);
				QDomNode transNode = iRIC::getChildNode(messageNode, "translation");
				QString transString = iRIC::getText(transNode);
				m_uselessTranslations.insert(sourceString, transString);
				messageNode = messageNode.nextSibling();
			}
			contextNode = contextNode.nextSibling();
		}
	}
	// dictionary is loaded.

	// now open solver definition file.
	QString deffilename = solFolder.absoluteFilePath(SolverDefinition::FILENAME);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QFile file(deffilename);
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&file, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(deffilename).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
	QDomNode SDNode = doc.documentElement();
	// scan the solver definition.

	scanDefinition(SDNode);
}
void SolverDefinitionTranslationChecker::scanDefinition(const QDomNode& node)
{
	if (! node.isElement()) {return;}
	QDomElement elem = node.toElement();
	if (elem.hasAttribute("caption")) {
		QString caption = elem.attribute("caption");
		// Handle it only when it does not exists in m_dic.
		if (! m_dic.contains(caption)) {
			m_dic.insert(caption);
			if (m_uselessTranslations.contains(caption)) {
				// It already has translation.
				m_messages.append(TranslationMessage(caption, m_uselessTranslations.value(caption)));
				m_uselessTranslations.remove(caption);
			} else {
				// It does not has translation.
				m_messages.append(TranslationMessage(caption));
			}
		}
	}
	QDomNodeList cnodes = node.childNodes();
	for (int i = 0; i < cnodes.length(); ++i) {
		scanDefinition(cnodes.item(i));
	}
}

void SolverDefinitionTranslationChecker::save(const QString& solverfolder, const QLocale& locale)
{
	// Now, overwrites the translation file.
	QDir solFolder(solverfolder);
	QString trFilename = solFolder.absoluteFilePath(SolverDefinitionTranslator::filenameFromLocale(locale));

	QFile commentFile(QString(":/data/translation_comments/%1.txt").arg(locale.name()));
	QString comment, buffer;
	commentFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream in(&commentFile);
	in.setCodec("UTF-8");
	while (! in.atEnd()) {
		buffer = in.readLine();
		comment.append(buffer);
		comment.append("\n");
	}

	QFile f(trFilename);
	f.open(QFile::WriteOnly);
	QXmlStreamWriter w(&f);
	w.setAutoFormatting(true);
	// start xml
	w.writeStartDocument("1.0");
	w.writeStartElement("TS");
	w.writeAttribute("version", "2.0");
	w.writeAttribute("language", locale.name());
	w.writeComment(comment);
	w.writeStartElement("context");

	// write valid elements and lacking elements, in the order of appearance.
	for (const TranslationMessage& m : m_messages) {
		m.save(w);
	}

	w.writeEndElement();
	w.writeEndElement();
	w.writeEndDocument();
	f.close();
}
