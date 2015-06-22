#ifndef GEODATABACKGROUNDCREATORT_H
#define GEODATABACKGROUNDCREATORT_H

#include "../geodata/geodatacreator.h"
#include "geodatabackground.h"
#include "geodatabackgroundnodemappert.h"
#include "geodatabackgroundcellmappert.h"

template <class V, class DA>
class GeoDataBackgroundCreatorT : public GeoDataCreator
{

protected:
	/// Constructor
	GeoDataBackgroundCreatorT() : GeoDataCreator() {
		m_caption = "Background";
		m_nodeMappers.append(new GeoDataBackgroundNodeMapperT<V, DA>(this));
		m_cellMappers.append(new GeoDataBackgroundCellMapperT<V, DA>(this));
	}

public:
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override {
		SolverDefinitionGridAttributeT<V>* tmpcond = dynamic_cast<SolverDefinitionGridAttributeT<V> * >(condition);
		GeoData* geodata = new GeoDataBackground(parent, this, tmpcond);
		geodata->setPosition(tmpcond->position());
		geodata->setDefaultMapper();
		return geodata;
	}
	bool isCompatibleWith(SolverDefinitionGridAttribute* /* condition*/) override {
		return true;
	}
	QString defaultCaption(unsigned int /*index*/) override {return "";}
};

#endif // GEODATABACKGROUNDCREATORT_H
