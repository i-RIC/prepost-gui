#include "post3dwindownodevectorparticledataitem.h"
#include "post3dwindownodevectorparticlegroupdataitem.h"

Post3dWindowNodeVectorParticleDataItem::Post3dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post3dWindowNodeVectorParticleDataItem::showPropertyDialog()
{
	return groupDataItem()->showPropertyDialog();
}

QDialog* Post3dWindowNodeVectorParticleDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

Post3dWindowNodeVectorParticleGroupDataItem* Post3dWindowNodeVectorParticleDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowNodeVectorParticleGroupDataItem*> (parent());
}
