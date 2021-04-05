#include "post3dwindownodevectorarrowtopdataitem.h"

Post3dWindowNodeVectorArrowTopDataItem::Post3dWindowNodeVectorArrowTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

void Post3dWindowNodeVectorArrowTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{

}
void Post3dWindowNodeVectorArrowTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{

}
