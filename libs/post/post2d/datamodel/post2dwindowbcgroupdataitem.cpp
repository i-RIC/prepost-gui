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

Post2dWindowBCGroupDataItem::Post2dWindowBCGroupDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem{ tr("Boundary Condition"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	setSubPath("bc");

	QSettings s;

	auto inputGridBCs = dynamic_cast<Post2dWindowInputGridDataItem*>(parent)->zoneDataItem()->v4DataContainer()->inputGridBCs();

	for (const auto inputGridBc : inputGridBCs) {
		auto item = new Post2dWindowBCDataItem(projectData()->solverDefinition(), inputGridBc->condition(), inputGridBc->caption().c_str(), this);
		m_childItems.push_back(item);
	}
}

Post2dWindowBCGroupDataItem::~Post2dWindowBCGroupDataItem()
{
	
}

void Post2dWindowBCGroupDataItem::setupChildren()
{

}

void Post2dWindowBCGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{

}

void Post2dWindowBCGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{

}

