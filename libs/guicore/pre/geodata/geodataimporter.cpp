#include "geodatacreator.h"
#include "geodataimporter.h"
#include "geodataimportersetting.h"
#include "private/geodataimporter_impl.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>

GeoDataImporter::Impl::Impl() :
	m_setting {nullptr}
{}

GeoDataImporter::Impl::~Impl()
{
	delete m_setting;
}

GeoDataImporter::GeoDataImporter(const std::string& name, const QString& caption, GeoDataCreator* creator) :
	QObject {creator},
	impl {new Impl {}}
{
	impl->m_name = name;
	impl->m_caption = caption;
}

GeoDataImporter::~GeoDataImporter()
{}

std::string GeoDataImporter::name() const
{
	return impl->m_name;
}

QString GeoDataImporter::caption() const
{
	return impl->m_caption;
}

PreProcessorGeoDataDataItemI* GeoDataImporter::import(const QString& filename, bool copyToProject, const QString& selectedFilter, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	delete impl->m_setting;
	impl->m_setting = createSetting();

	impl->m_setting->setFileName(filename);
	impl->m_setting->setCopiedToProject(copyToProject);
	impl->m_setting->setSelectedFilter(selectedFilter);

	int count;
	bool ok = importInit(&count, condition, item, w, false);
	if (! ok) {return nullptr;}
	if (count > 1) {return nullptr;}

	auto ret = item->buildGeoDataDataItem();
	GeoData* data = creator()->create(ret, condition);
	creator()->setNameAndDefaultCaption(item->childItems(), data, item->projectData());
	ret->setGeoData(data);

	ok = importData(data, 0, w);
	if (! ok) {
		delete ret;
		return nullptr;
	}

	return ret;
}

PreProcessorGeoDataDataItemI* GeoDataImporter::import(GeoDataImporterSetting* setting, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	int count;
	delete impl->m_setting;
	impl->m_setting = setting;

	bool ok = importInit(&count, condition, item, w, true);
	if (! ok) {return nullptr;}
	if (count > 1) {return nullptr;}

	auto ret = item->buildGeoDataDataItem();
	GeoData* data = creator()->create(ret, condition);
	creator()->setNameAndDefaultCaption(item->childItems(), data, item->projectData());
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

bool GeoDataImporter::importInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w, bool withSetting)
{
	*count = 1;
	if (withSetting) {
		return doInitWithSetting(count, condition, item, w);
	} else {
		return doInit(count, condition, item, w);
	}
}

GeoDataImporterSetting* GeoDataImporter::setting() const
{
	return impl->m_setting;
}

bool GeoDataImporter::doInit(int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemI* /*item*/, QWidget* /*w*/)
{
	return true;
}

bool GeoDataImporter::doInitWithSetting(int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemI* /*item*/, QWidget* /*w*/)
{
	return true;
}

GeoDataImporterSetting* GeoDataImporter::createSetting() const
{
	return new GeoDataImporterSetting();
}
