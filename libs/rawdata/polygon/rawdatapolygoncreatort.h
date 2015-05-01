#ifndef RAWDATAPOLYGONCREATORT_H
#define RAWDATAPOLYGONCREATORT_H

#include "rawdatapolygoncreator.h"
#include "rawdatapolygonnodemappert.h"
#include "rawdatapolygoncellmappert.h"

template <class V, class DA>
class RawDataPolygonCreatorT : public RawDataPolygonCreator
{
public:
	/// Constructor
	RawDataPolygonCreatorT()
		: RawDataPolygonCreator() {
		m_nodeMappers.append(new RawDataPolygonNodeMapperT<V, DA>(this));
		m_cellMappers.append(new RawDataPolygonCellMapperT<V, DA>(this));
	}
	bool isCompatibleWith(SolverDefinitionGridRelatedCondition* condition) {
		SolverDefinitionGridRelatedConditionT<V>* cond = dynamic_cast<SolverDefinitionGridRelatedConditionT<V>* >(condition);
		if (cond == nullptr) {return false;}
		if (condition->dimensions().size() > 0) {return false;}
		return true;
	}
};

#endif // RAWDATAPOLYGONCREATORT_H
