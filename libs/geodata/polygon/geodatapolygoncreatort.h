#ifndef GEODATAPOLYGONCREATORT_H
#define GEODATAPOLYGONCREATORT_H

#include "geodatapolygoncreator.h"
#include "geodatapolygonnodemappert.h"
#include "geodatapolygoncellmappert.h"

template <class V, class DA>
class GeoDataPolygonCreatorT : public GeoDataPolygonCreator
{

public:
	/// Constructor
	GeoDataPolygonCreatorT()
		: GeoDataPolygonCreator() {
		m_nodeMappers.append(new GeoDataPolygonNodeMapperT<V, DA>(this));
		m_cellMappers.append(new GeoDataPolygonCellMapperT<V, DA>(this));
	}
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) override {
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
		if (cond == nullptr) {return false;}
		if (condition->dimensions().size() > 0) {return false;}
		return true;
	}
};

#endif // GEODATAPOLYGONCREATORT_H
