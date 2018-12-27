#include "hydraulicdatacreator.h"
#include "hydraulicdataimporter.h"
#include "private/hydraulicdataimporter_impl.h"

HydraulicDataImporter::HydraulicDataImporter(HydraulicDataCreator* creator) :
	QObject {creator},
	impl {new Impl {}}
{}

HydraulicDataImporter::~HydraulicDataImporter()
{
	delete impl;
}

HydraulicDataCreator* HydraulicDataImporter::creator() const
{
	return dynamic_cast<HydraulicDataCreator*>(parent());
}

bool HydraulicDataImporter::init(const QString& filename, const QString& selectedFilter, int* count, QWidget* w)
{
	impl->m_filename = filename;
	impl->m_selectedFilter = selectedFilter;
	*count = 1;
	return doInit(filename, selectedFilter, count, w);
}

const QString& HydraulicDataImporter::filename() const
{
	return impl->m_filename;
}

const QString& HydraulicDataImporter::selectedFilter() const
{
	return impl->m_selectedFilter;
}

bool HydraulicDataImporter::doInit(const QString&, const QString&, int*, QWidget*)
{
	return true;
}
