#include "geodatabackgroundcomplexcreator.h"
#include "geodatabackgroundnodemappert.h"
#include "geodatabackgroundcellmappert.h"
#include "geodatabackgroundcomplex.h"

GeoDataBackgroundComplexCreator* GeoDataBackgroundComplexCreator::m_instance = nullptr;

GeoDataBackgroundComplexCreator::GeoDataBackgroundComplexCreator() :
	GeoDataCreator{}
{
	m_caption = "Background";
	m_nodeMappers.append(new GeoDataBackgroundNodeMapperT<int, vtkIntArray>(this));
	m_cellMappers.append(new GeoDataBackgroundCellMapperT<int, vtkIntArray>(this));
	m_typeName = "complexBackground";
}

GeoData* GeoDataBackgroundComplexCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	SolverDefinitionGridComplexAttribute* tmpcond = dynamic_cast<SolverDefinitionGridComplexAttribute* >(condition);
	GeoData* geodata = new GeoDataBackgroundComplex(parent, this, tmpcond);
	geodata->setPosition(tmpcond->position());
	geodata->setDefaultMapper();
	return geodata;
}

bool GeoDataBackgroundComplexCreator::isCompatibleWith(SolverDefinitionGridAttribute*)
{
	return true;
}

QString GeoDataBackgroundComplexCreator::defaultCaption(unsigned int)
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
