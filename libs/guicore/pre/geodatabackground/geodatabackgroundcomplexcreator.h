#ifndef GEODATABACKGROUNDCOMPLEXCREATOR_H
#define GEODATABACKGROUNDCOMPLEXCREATOR_H

#include "geodatabackgroundcreatort.h"
#include "geodatabackgroundcomplex.h"
#include "geodatabackgroundnodemappert.h"
#include "geodatabackgroundcellmappert.h"
#include "../../solverdef/solverdefinitiongridcomplexattribute.h"
#include <vtkIntArray.h>

class GeoDataBackgroundComplexCreator : public GeoDataCreator
{

protected:
	/// Constructor
	GeoDataBackgroundComplexCreator() : GeoDataCreator() {
		m_caption = "Background";
		m_nodeMappers.append(new GeoDataBackgroundNodeMapperT<int, vtkIntArray>(this));
		m_cellMappers.append(new GeoDataBackgroundCellMapperT<int, vtkIntArray>(this));
		m_typeName = "complexBackground";
	}

public:
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override {
		SolverDefinitionGridComplexAttribute* tmpcond = dynamic_cast<SolverDefinitionGridComplexAttribute* >(condition);
		GeoData* geodata = new GeoDataBackgroundComplex(parent, this, tmpcond);
		geodata->setPosition(tmpcond->position());
		geodata->setDefaultMapper();
		return geodata;
	}
	bool isCompatibleWith(SolverDefinitionGridAttribute* /* condition*/) override {
		return true;
	}
	QString defaultCaption(unsigned int /*index*/) override {return "";}

	static GeoDataBackgroundComplexCreator* instance() {
		if (! m_instance) {
			m_instance = new GeoDataBackgroundComplexCreator();
		}
		return m_instance;
	}

protected:
	static GeoDataBackgroundComplexCreator* m_instance;

};

#endif // GEODATABACKGROUNDCOMPLEXCREATOR_H
