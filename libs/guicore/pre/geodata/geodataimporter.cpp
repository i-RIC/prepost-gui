#include "geodatacreator.h"
#include "geodataimporter.h"
#include "private/geodataimporter_impl.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

GeoDataImporter::GeoDataImporter(const std::string& name, const QString& caption, GeoDataCreator* creator) :
	QObject {creator},
	impl {new Impl {}}
{
	impl->m_name = name;
	impl->m_caption = caption;
}

GeoDataImporter::~GeoDataImporter()
{
	delete impl;
}

std::string GeoDataImporter::name() const
{
	return impl->m_name;
}

QString GeoDataImporter::caption() const
{
	return impl->m_caption;
}

PreProcessorGeoDataDataItemInterface* GeoDataImporter::import(const QString& filename, const QString& selectedFilter, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	int count;
	bool ok = importInit(filename, selectedFilter, &count, condition, item, w);
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

GeoDataCreator* GeoDataImporter::creator() const
{
	return dynamic_cast<GeoDataCreator*>(parent());
}

QString GeoDataImporter::filename() const
{
	return impl->m_filename;
}

QString GeoDataImporter::selectedFilter() const
{
	return impl->m_selectedFilter;
}

bool GeoDataImporter::importInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	impl->m_filename = filename;
	impl->m_selectedFilter = selectedFilter;
	*count = 1;
	return doInit(filename, selectedFilter, count, condition, item, w);
}

bool GeoDataImporter::doInit(const QString& /*filename*/, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* /*w*/)
{
	return true;
}
