#include "post2dwindowbcdataitem.h"
#include "post2dwindowbcgroupdataitem.h"
#include "post2dwindowdataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/v4postzonedatabc.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/xmlsupport.h>

Post2dWindowBCGroupDataItem::Post2dWindowBCGroupDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem{ tr("Boundary Condition"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	setSubPath("bc");
}

Post2dWindowBCGroupDataItem::~Post2dWindowBCGroupDataItem()
{
	
}

void Post2dWindowBCGroupDataItem::setupChildren()
{
	auto inputGridBCs = dynamic_cast<Post2dWindowInputGridDataItem*>(parent())->zoneDataItem()->v4DataContainer()->inputGridBCs();

	for (const auto inputGridBC : inputGridBCs) {
		auto item = new Post2dWindowBCDataItem(projectData()->solverDefinition(), inputGridBC->condition(), inputGridBC, this);
		m_childItems.push_back(item);
	}
}

void Post2dWindowBCGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (const auto& child : m_childItems) {
		auto bc = dynamic_cast<Post2dWindowBCDataItem*> (child);

		auto subNode = iRIC::getChildNodeWithAttribute(node, "BoundaryCondition", "name", bc->inputGridBC()->caption().c_str());
		if (subNode.isNull()) {continue;}

		bc->loadFromProjectMainFile(subNode);
	}
}

void Post2dWindowBCGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (const auto& bc : m_childItems) {
		writer.writeStartElement("BoundaryCondition");
		writer.writeAttribute("name", dynamic_cast<Post2dWindowBCDataItem*> (bc)->inputGridBC()->caption().c_str());
		bc->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

