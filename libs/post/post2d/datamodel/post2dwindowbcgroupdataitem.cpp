#include "post2dwindowbcdataitem.h"
#include "post2dwindowbcgroupdataitem.h"
#include "post2dwindowdataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgridtypedataitem.h"

#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>

Post2dWindowBCGroupDataItem::Post2dWindowBCGroupDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem{ tr("Boundary Condition"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	setSubPath("bc");

	QSettings s;

	auto gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent->parent()->parent());
	auto gridType = gtItem->gridType();
	for (int i = 0; i < gridType->boundaryConditions().size(); ++i) {
		auto bc = gridType->boundaryConditions().at(i);
		auto item = new Post2dWindowBCDataItem(projectData()->solverDefinition(), bc, bc->caption(), this);
		m_childItems.push_back(item);
	}
}

Post2dWindowBCGroupDataItem::~Post2dWindowBCGroupDataItem()
{
	
}

void Post2dWindowBCGroupDataItem::setupChildren()
{

}
