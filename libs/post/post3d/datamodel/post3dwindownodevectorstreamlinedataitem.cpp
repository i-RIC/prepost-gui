#include "post3dwindownodevectorstreamlinedataitem.h"
#include "post3dwindownodevectorstreamlinegroupdataitem.h"

Post3dWindowNodeVectorStreamlineDataItem::Post3dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post3dWindowNodeVectorStreamlineDataItem::~Post3dWindowNodeVectorStreamlineDataItem()
{}

void Post3dWindowNodeVectorStreamlineDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

QDialog* Post3dWindowNodeVectorStreamlineDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

Post3dWindowNodeVectorStreamlineGroupDataItem* Post3dWindowNodeVectorStreamlineDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowNodeVectorStreamlineGroupDataItem*> (parent());
}
