#include "geodatacreator.h"
#include "geodatawebimporter.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

GeoDataWebImporter::GeoDataWebImporter(const std::string& name, const QString& caption, GeoDataCreator* creator) :
	QObject {creator},
	m_name (name),
	m_caption {caption}
{}

std::string GeoDataWebImporter::name() const
{
	return m_name;
}

QString GeoDataWebImporter::caption() const
{
	return m_caption;
}

bool GeoDataWebImporter::isCompatibleWith(SolverDefinitionGridAttribute* /*condition*/) const
{
	return false;
}

GeoDataCreator* GeoDataWebImporter::creator() const
{
	return dynamic_cast<GeoDataCreator*>(parent());
}

PreProcessorGeoDataDataItemInterface* GeoDataWebImporter::import(SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	int count;
	bool ok = importInit(&count, condition, item, w);
	if (! ok) {return nullptr;}
	if (count > 1) {return nullptr;}

	auto ret = item->buildGeoDataDataItem();
	GeoData* data = creator()->create(ret, condition);
	creator()->setNameAndDefaultCaption(item->childItems(), data);
	ret->setGeoData(data);

	ok = importData(data, 0, w);
	if (! ok) {
		delete ret;
		return nullptr;
	}

	return ret;
}

bool GeoDataWebImporter::importInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	*count = 1;
	return doInit(count, condition, item, w);
}

bool GeoDataWebImporter::doInit(int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* /*w*/)
{
	return true;
}
