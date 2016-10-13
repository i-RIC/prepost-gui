#include "post3dwindowcontourgrouptopdataitem.h"

#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>


#include <QDomNode>
#include <QDomElement>
#include <QXmlStreamWriter>

#include <vtkPointData.h>

Post3dWindowContourGroupTopDataItem::Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p) : 
	Post3dWindowDataItem {tr("Contours"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData())) {
		m_colorBarTitleMap.insert(name, name.c_str());
	}
}

Post3dWindowContourGroupTopDataItem::~Post3dWindowContourGroupTopDataItem()
{
}

void Post3dWindowContourGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (node.toElement().nodeName() == "Contours") {
		// multi-contours
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarBarTitles") {
				QDomNodeList titles = children.at(i).childNodes();
				for (int i = 0; i < titles.count(); ++i) {
					std::string val = iRIC::toStr(titles.at(i).toElement().attribute("value"));
					QString title = titles.at(i).toElement().attribute("title");
					m_colorBarTitleMap[val] = title;
				}
			} else if (childElem.nodeName() == "ContourGroup") {
				auto item = new Post3dWindowContourGroupDataItem(this);
				item->updateZScale(m_zScale);
				item->loadFromProjectMainFile(children.at(i));
				m_childItems.push_back(item);
			}
		}
	} else {
		// single-contour
		size_t faces = 0;
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarBarTitle") {
				std::string val = iRIC::toStr(childElem.attribute("value"));
				QString title = childElem.attribute("title");
				m_colorBarTitleMap[val] = title;
			} else if (childElem.nodeName() == "FaceSetting") {
				++faces;
			}
		}
		if (faces > 0) {
			auto item  = new Post3dWindowContourGroupDataItem(this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(node);
			m_childItems.push_back(item);
		}
	}
}

void Post3dWindowContourGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// scalar bar titles
	writer.writeStartElement("ScalarBarTitles");
	QMapIterator<std::string, QString> i(m_colorBarTitleMap);
	while (i.hasNext()) {
		i.next();
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", i.key().c_str());
		writer.writeAttribute("title", i.value());
		writer.writeEndElement();
	}
	writer.writeEndElement();

	// contours
	for (auto item : m_childItems) {
		writer.writeStartElement("ContourGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowContourGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowContourGroupDataItem*>(item);
		child->update();
	}
}

void Post3dWindowContourGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}
