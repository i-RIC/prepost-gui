#include "solverdefinitiontranslator.h"
#include "private/solverdefinitiontranslator_impl.h"

#include <misc/errormessage.h>
#include <misc/xmlsupport.h>

#include <QLocale>
#include <QDir>
#include <QDomDocument>
#include <QDomNode>
#include <QString>

SolverDefinitionTranslator::Impl::Impl()
{}

SolverDefinitionTranslator::Impl::Impl(const QString &solverfolder, const QLocale &locale) :
	m_locale {locale}
{
	load(solverfolder, locale);
}

void SolverDefinitionTranslator::Impl::load(const QString &solverfolder, const QLocale &locale)
{
	// get translation file name
	QDir solFolder(solverfolder);
	QString trFilename = solFolder.absoluteFilePath(filenameFromLocale(locale));
	if (! QFile::exists(trFilename)) {
		// translation file for specified locale doesn't exists.
		return;
	}
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
	int msgIndex = 1;
	while (! contextNode.isNull()) {
		if (contextNode.nodeName() != "context") {
			QString msg = errorHeader;
			msg.append("%1: Only context node is allowed under TS node, but %2 node is found.");
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
			QDomNode transNode = iRIC::getChildNode(messageNode, "translation");
			if (sourceNode.isNull()) {
				QString msg = errorHeader;
				msg.append("No source node is found under %2th message node.");
				msg = msg.arg(trFilename).arg(msgIndex);
				throw ErrorMessage(msg);
			}
			if (transNode.isNull()) {
				QString msg = errorHeader;
				msg.append("No translation node is found under %2th message node.");
				msg = msg.arg(trFilename).arg(msgIndex);
				throw ErrorMessage(msg);
			}
			if (transNode.toElement().attribute("type", "") != "unfinished" && iRIC::getText(transNode) != "") {
				m_dictionary.insert(iRIC::getText(sourceNode), iRIC::getText(transNode));
			}
			// register the message to the dictionary.
			messageNode = messageNode.nextSibling();
			++msgIndex;
		}
		contextNode = contextNode.nextSibling();
	}
}

SolverDefinitionTranslator::SolverDefinitionTranslator(const QString& solverfolder, const QLocale& locale) :
	impl {new Impl {solverfolder, locale}}
{}

SolverDefinitionTranslator::SolverDefinitionTranslator(const SolverDefinitionTranslator& t) :
	impl {new Impl {}}
{
	*impl = *(t.impl);
}

SolverDefinitionTranslator::SolverDefinitionTranslator(SolverDefinitionTranslator&& t) :
	impl {t.impl}
{
	t.impl = nullptr;
}

SolverDefinitionTranslator::~SolverDefinitionTranslator()
{
	delete impl;
}

QString SolverDefinitionTranslator::translate(const QString& src) const
{
	if (impl->m_dictionary.contains(src)) {
		return impl->m_dictionary.value(src);
	}

	// for vector values
	QString srcX = src;
	srcX.append("X");
	if (impl->m_dictionary.contains(srcX)) {
		QString tgtX = impl->m_dictionary.value(srcX);
		return tgtX.left(tgtX.length() - 1);
	}

	if (src.contains(" (magnitude)")) {
		QString srcX2 = src;
		srcX2.replace(" (magnitude)", "");
		srcX2.append("X");
		if (impl->m_dictionary.contains(srcX2)) {
			QString tgtX = impl->m_dictionary.value(srcX2);
			QString tgt = tgtX.left(tgtX.length() - 1);
			tgt.append(tr(" (magnitude)"));
			return tgt;
		}
	}
	return src;
}


QString SolverDefinitionTranslator::imageFilename(const QString& filename) const
{
	QFileInfo finfo(filename);
	QString tmpName("%1_%2.%3");
	return tmpName.arg(finfo.baseName()).arg(impl->m_locale.name()).arg(finfo.suffix());
}

QString SolverDefinitionTranslator::filenameFromLocale(const QLocale& locale)
{
	QString tmp("translation_");
	tmp.append(locale.name());
	tmp.append(".ts");
	return tmp;
}
