#include "postprocessorwindowfactory.h"

#include <misc/xmlsupport.h>
#include <post/graph2dhybrid/graph2dhybridwindowprojectdataitem.h>
#include <post/graph2dscattered/graph2dscatteredwindowprojectdataitem.h>
#include <post/graph2dverification/graph2dverificationwindowprojectdataitem.h>
#include <post/post2d/post2dwindowprojectdataitem.h>
#include <post/post2dbirdeye/post2dbirdeyewindowprojectdataitem.h>
#include <post/post3d/post3dwindowprojectdataitem.h>

#include <QDomElement>
#include <QXmlStreamWriter>

PostProcessorWindowFactory::PostProcessorWindowFactory(QObject* parent)
	: QObject(parent)
{

}

PostProcessorWindowProjectDataItem* PostProcessorWindowFactory::factory(const QString& name, ProjectDataItem* parent, QWidget* parentwindow)
{
	if (! m_windowCount.contains(name)) {
		m_windowCount.insert(name, 0);
	}
	int c = (++ m_windowCount[name]);
	if (name == "post2dwindow") {
		return new Post2dWindowProjectDataItem(parent, c, parentwindow);
	} else if (name == "post2dbirdeyewindow") {
		return new Post2dBirdEyeWindowProjectDataItem(parent, c, parentwindow);
	} else if (name == "post3dwindow") {
		return new Post3dWindowProjectDataItem(parent, c, parentwindow);
	} else if (name == "graph2dhybridwindow") {
		return new Graph2dHybridWindowProjectDataItem(parent, c, parentwindow);
	} else if (name == "graph2dscatteredwindow") {
		return new Graph2dScatteredWindowProjectDataItem(parent, c, parentwindow);
	} else if (name == "graph2dverificationwindow") {
		return new Graph2dVerificationWindowProjectDataItem(parent, c, parentwindow);
	}
	return 0;
}

PostProcessorWindowProjectDataItem* PostProcessorWindowFactory::restore(const QDomNode& node, ProjectDataItem* parent, QWidget* parentwindow) const
{
	int c = node.toElement().attribute("index").toInt();
	if (node.toElement().attribute("type") == "post2dwindow") {
		return new Post2dWindowProjectDataItem(parent, c, parentwindow);
	} else if (node.toElement().attribute("type") == "post2dbirdeyewindow") {
		return new Post2dBirdEyeWindowProjectDataItem(parent, c, parentwindow);
	} else if (node.toElement().attribute("type") == "post3dwindow") {
		return new Post3dWindowProjectDataItem(parent, c, parentwindow);
	} else if (node.toElement().attribute("type") == "graph2dhybridwindow") {
		return new Graph2dHybridWindowProjectDataItem(parent, c, parentwindow);
	} else if (node.toElement().attribute("type") == "graph2dscatteredwindow") {
		return new Graph2dScatteredWindowProjectDataItem(parent, c, parentwindow);
	} else if (node.toElement().attribute("type") == "graph2dverificationwindow") {
		return new Graph2dVerificationWindowProjectDataItem(parent, c, parentwindow);
	}
	return 0;
}

void PostProcessorWindowFactory::resetWindowCounts()
{
	m_windowCount.clear();
}

void PostProcessorWindowFactory::loadWindowCounts(const QDomNode& node)
{
	resetWindowCounts();
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement elem = children.at(i).toElement();
		QString name = elem.attribute("name");
		int value = iRIC::getIntAttribute(elem, "value", 0);
		m_windowCount.insert(name, value);
	}
}

void PostProcessorWindowFactory::saveWindowCounts(QXmlStreamWriter& writer)
{
	foreach (QString name, m_windowCount.keys()) {
		int val = m_windowCount.value(name);
		writer.writeStartElement("WindowCount");
		writer.writeAttribute("name", name);
		iRIC::setIntAttribute(writer, "value", val);
		writer.writeEndElement();
	}
}
