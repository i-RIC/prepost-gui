#ifndef RAWDATABACKGROUNDCREATORT_H
#define RAWDATABACKGROUNDCREATORT_H

#include "../rawdata/rawdatacreator.h"
#include "rawdatabackground.h"
#include "rawdatabackgroundnodemappert.h"
#include "rawdatabackgroundcellmappert.h"

template <class V, class DA>
class RawDataBackgroundCreatorT : public RawDataCreator
{
protected:
	/// Constructor
	RawDataBackgroundCreatorT() : RawDataCreator() {
		m_caption = "Background";
		m_nodeMappers.append(new RawDataBackgroundNodeMapperT<V, DA>(this));
		m_cellMappers.append(new RawDataBackgroundCellMapperT<V, DA>(this));
	}
public:
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override {
		SolverDefinitionGridAttributeT<V>* tmpcond = dynamic_cast<SolverDefinitionGridAttributeT<V> * >(condition);
		RawData* rawdata = new RawDataBackground(parent, this, tmpcond);
		rawdata->setPosition(tmpcond->position());
		rawdata->setDefaultMapper();
		return rawdata;
	}
	bool isCompatibleWith(SolverDefinitionGridAttribute* /* condition*/) override {
		return true;
	}
	QString defaultCaption(unsigned int /*index*/) override {return "";}
};

#endif // RAWDATABACKGROUNDCREATORT_H
