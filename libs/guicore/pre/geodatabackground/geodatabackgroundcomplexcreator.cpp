#include "geodatabackgroundcellmappert.h"
#include "geodatabackgroundcomplex.h"
#include "geodatabackgroundcomplexcreator.h"
#include "geodatabackgroundifacemappert.h"
#include "geodatabackgroundjfacemappert.h"
#include "geodatabackgroundnodemappert.h"

GeoDataBackgroundComplexCreator* GeoDataBackgroundComplexCreator::m_instance = nullptr;

GeoDataBackgroundComplexCreator::GeoDataBackgroundComplexCreator() :
	GeoDataCreator{"complexBackground", tr("Background")}
{}

GeoData* GeoDataBackgroundComplexCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto geodata = new GeoDataBackgroundComplex(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		geodata->setMapper(new GeoDataBackgroundNodeMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		geodata->setMapper(new GeoDataBackgroundCellMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {
		geodata->setMapper(new GeoDataBackgroundIFaceMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {
		geodata->setMapper(new GeoDataBackgroundJFaceMapperT<int, vtkIntArray>(this));
	}
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
