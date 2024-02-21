#include "geodatapointmap.h"
#include "geodatapointmapriter3dimporter.h"
#include "geodatapointmapriter3dloader.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/project/projectdata.h>

GeoDataPointmapRiter3dImporter::GeoDataPointmapRiter3dImporter(GeoDataCreator* creator) :
	GeoDataImporter("riter3d", tr("RiTER3D project"), creator),
	m_loader {nullptr}
{}

GeoDataPointmapRiter3dImporter::~GeoDataPointmapRiter3dImporter()
{
	delete m_loader;
}

bool GeoDataPointmapRiter3dImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	auto pointmap = dynamic_cast<GeoDataPointmap*>(data);
	bool ok = m_loader->load(pointmap, w);
	if (! ok) {return false;}

	pointmap->setRiter3dProject(m_fileName);
	return true;
}

const QStringList GeoDataPointmapRiter3dImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("RiTER3D project file (*.ertr)");
	return ret;
}

const QStringList GeoDataPointmapRiter3dImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "ertr";
	return ret;
}

bool GeoDataPointmapRiter3dImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	m_fileName = filename;
	m_loader = new GeoDataPointmapRiter3dLoader(filename);

	bool ok = m_loader->open(item->projectData()->tmpFileName(), item->iricMainWindow());
	if (! ok) {return false;}

	ok = m_loader->check(w);
	if (! ok) {return false;}

	return true;
}
