#include "geodatabackgroundcomplexcreator.h"
#include "geodatabackgroundnodemappert.h"
#include "geodatabackgroundcellmappert.h"
#include "geodatabackgroundcomplex.h"

GeoDataBackgroundComplexCreator* GeoDataBackgroundComplexCreator::m_instance = nullptr;

GeoDataBackgroundComplexCreator::GeoDataBackgroundComplexCreator() :
	GeoDataCreator{"complexBackground", tr("Background")}
{
	nodeMappers().push_back(new GeoDataBackgroundNodeMapperT<int, vtkIntArray>(this));
	cellMappers().push_back(new GeoDataBackgroundCellMapperT<int, vtkIntArray>(this));
}

GeoData* GeoDataBackgroundComplexCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	SolverDefinitionGridComplexAttribute* tmpcond = dynamic_cast<SolverDefinitionGridComplexAttribute* >(condition);
	GeoData* geodata = new GeoDataBackgroundComplex(parent, this, tmpcond);
	geodata->setPosition(tmpcond->position());
	geodata->setDefaultMapper();
	return geodata;
}

bool GeoDataBackgroundComplexCreator::isCompatibleWith(SolverDefinitionGridAttribute*) const
{
	return true;
}

QString GeoDataBackgroundComplexCreator::defaultCaption(unsigned int) const
{
	return "";
}

GeoDataBackgroundComplexCreator* GeoDataBackgroundComplexCreator::instance()
{
	if (! m_instance) {
		m_instance = new GeoDataBackgroundComplexCreator();
	}
	return m_instance;
}
