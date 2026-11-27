#include "post2dwindowgraphdataitem.h"

Post2dWindowGraphDataItem::Post2dWindowGraphDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{
	setStandardItemToolTip(tr("Selects the result used for the graph display."));
}

Post2dWindowGraphDataItem::~Post2dWindowGraphDataItem()
{}
