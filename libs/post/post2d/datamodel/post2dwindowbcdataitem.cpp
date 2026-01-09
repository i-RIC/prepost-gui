#include "post2dwindowbcdataitem.h"
#include "post2dwindowdataitem.h"
#include "private/post2dwindowbcdataitem_impl.h"

#include <guicore/base/iricmainwindowi.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/errormessage.h>

Post2dWindowBCDataItem::Impl::Impl(Post2dWindowBCDataItem* item) :
	m_dialog {nullptr}
{

}

Post2dWindowBCDataItem::Impl::~Impl()
{
	delete m_dialog;
}

Post2dWindowBCDataItem::Post2dWindowBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, QString caption, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	impl->m_condition = cond;

}

Post2dWindowBCDataItem::~Post2dWindowBCDataItem()
{
	delete impl;
}

void Post2dWindowBCDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{

}

void Post2dWindowBCDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{

}