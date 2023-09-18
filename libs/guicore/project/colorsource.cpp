#include "colorsource.h"
#include "private/colorsource_impl.h"

#include <misc/stringtool.h>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QXmlStreamWriter>

ColorSource::ColorSource(ProjectDataItem* d) :
	ProjectDataItem {d},
	impl {new Impl()}
{
	loadDefault();
}

ColorSource::~ColorSource()
{}

void ColorSource::loadDefault()
{
	load(":/libs/guicore/data/colorsource.xml");
}

void ColorSource::load(const QString& filename)
{
	QFile f(filename);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	QDomElement element = doc.documentElement().toElement();
	loadFromProjectMainFile(element);
}

QColor ColorSource::getColor(int index) const
{
	int i = index % impl->m_colors.size();
	return impl->m_colors.at(i);
}

void ColorSource::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_colors.clear();
	QDomNodeList childNodes = node.childNodes();
	for (int i = 0; i < childNodes.count(); ++i) {
		QDomNode child = childNodes.at(i);
		QColor color = iRIC::QColorFromString(child.toElement().attribute("value", "#000000"));
		impl->m_colors.push_back(color);
	}
}

void ColorSource::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (QColor c : impl->m_colors) {
		writer.writeStartElement("Color");
		writer.writeAttribute("value", iRIC::QColorToString(c));
		writer.writeEndElement();
	}
}
