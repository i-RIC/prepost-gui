#ifndef RAWDATABACKGROUNDCOMPLEXCREATOR_H
#define RAWDATABACKGROUNDCOMPLEXCREATOR_H

#include "rawdatabackgroundcreatort.h"
#include "rawdatabackgroundcomplex.h"
#include "rawdatabackgroundnodemappert.h"
#include "rawdatabackgroundcellmappert.h"
#include "../../solverdef/solverdefinitiongridcomplexattribute.h"
#include <vtkIntArray.h>

class RawDataBackgroundComplexCreator : public RawDataCreator
{
protected:
	/// Constructor
	RawDataBackgroundComplexCreator() : RawDataCreator() {
		m_caption = "Background";
		m_nodeMappers.append(new RawDataBackgroundNodeMapperT<int, vtkIntArray>(this));
		m_cellMappers.append(new RawDataBackgroundCellMapperT<int, vtkIntArray>(this));
		m_typeName = "complexBackground";
	}
public:
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override {
		SolverDefinitionGridComplexAttribute* tmpcond = dynamic_cast<SolverDefinitionGridComplexAttribute* >(condition);
		RawData* rawdata = new RawDataBackgroundComplex(parent, this, tmpcond);
		rawdata->setPosition(tmpcond->position());
		rawdata->setDefaultMapper();
		return rawdata;
	}
	bool isCompatibleWith(SolverDefinitionGridAttribute* /* condition*/) override {
		return true;
	}
	QString defaultCaption(unsigned int /*index*/) override {return "";}

	static RawDataBackgroundComplexCreator* instance() {
		if (! m_instance) {
			m_instance = new RawDataBackgroundComplexCreator();
		}
		return m_instance;
	}
protected:
	static RawDataBackgroundComplexCreator* m_instance;

};

#endif // RAWDATABACKGROUNDCOMPLEXCREATOR_H
