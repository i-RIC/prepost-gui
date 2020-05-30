#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupcreator.h"
#include "geodatapolylinegroupshpimporter.h"
#include "geodatapolylinegroupshpexporter.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

#include <QStandardItem>

GeoDataPolyLineGroupCreator::GeoDataPolyLineGroupCreator(const QString& typeName) :
	GeoDataPolyDataGroupCreator {typeName, tr("Polyline Group")}
{
	importers().push_back(new GeoDataPolyLineGroupShpImporter(this));
	exporters().push_back(new GeoDataPolyLineGroupShpExporter(this));
}

QString GeoDataPolyLineGroupCreator::name(unsigned int index) const
{
	return QString("polyLineGroup%1").arg(index);
}

QString GeoDataPolyLineGroupCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Polyline Group%1")).arg(index);
}

bool GeoDataPolyLineGroupCreator::isCreatable() const
{
	return true;
}

GeoData* GeoDataPolyLineGroupCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto g = new GeoDataPolyLineGroup(parent, this, condition);
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent);
	item->standardItem()->setData(QVariant(tr("Deleting this item will also remove any graph windows associated with this polyline group.  Are you sure you want to delete this item?")), Qt::UserRole + 20);
	g->setPosition(condition->position());
	g->setDefaultMapper();
	return g;
}

int GeoDataPolyLineGroupCreator::shapeType() const
{
	return SHPT_ARC;
}

QString GeoDataPolyLineGroupCreator::shapeName() const
{
	return tr("polyline");
}

QString GeoDataPolyLineGroupCreator::shapeNameCamelCase() const
{
	return tr("Polyline");
}
