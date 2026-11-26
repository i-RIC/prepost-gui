#include <QStandardItem>
#include "post3dwindownodevectorparticledataitem.h"
#include "post3dwindownodevectorparticlegroupdataitem.h"

Post3dWindowNodeVectorParticleDataItem::Post3dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{
	m_standardItem->setToolTip(tr("Selects the vector field used to move the visualization particles."));
	if (m_standardItemCopy != nullptr) {m_standardItemCopy->setToolTip(m_standardItem->toolTip());}
}

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
