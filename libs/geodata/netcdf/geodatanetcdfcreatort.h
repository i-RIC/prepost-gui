#ifndef GEODATANETCDFCREATORT_H
#define GEODATANETCDFCREATORT_H

#include "geodatanetcdfcreator.h"
#include "geodatanetcdft.h"
#include "geodatanetcdfimportert.h"
#include "geodatanetcdfnodemappert.h"
#include "geodatanetcdfcellmappert.h"

#include <guicore/solverdef/solverdefinitiongridattributet.h>

template <class V, class DA>
class GeoDataNetcdfCreatorT : public GeoDataNetcdfCreator
{

public:
	/// Constructor
	GeoDataNetcdfCreatorT()
		: GeoDataNetcdfCreator() {
		m_nodeMappers.append(new GeoDataNetcdfNodeMapperT<V, DA>(this));
		m_cellMappers.append(new GeoDataNetcdfCellMapperT<V, DA>(this));
	}
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) override {
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
		if (cond == 0) {return false;}
		return true;
	}
};

#endif // GEODATANETCDFCREATORT_H
