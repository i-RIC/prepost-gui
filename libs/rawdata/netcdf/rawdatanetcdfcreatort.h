#ifndef RAWDATANETCDFCREATORT_H
#define RAWDATANETCDFCREATORT_H

#include "rawdatanetcdfcreator.h"
#include "rawdatanetcdft.h"
#include "rawdatanetcdfimportert.h"
#include "rawdatanetcdfnodemappert.h"
#include "rawdatanetcdfcellmappert.h"

#include <guicore/solverdef/solverdefinitiongridattributet.h>

template <class V, class DA>
class RawDataNetcdfCreatorT : public RawDataNetcdfCreator
{

public:
	/// Constructor
	RawDataNetcdfCreatorT()
		: RawDataNetcdfCreator() {
		m_nodeMappers.append(new RawDataNetcdfNodeMapperT<V, DA>(this));
		m_cellMappers.append(new RawDataNetcdfCellMapperT<V, DA>(this));
	}
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) override {
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
		if (cond == 0) {return false;}
		return true;
	}
};

#endif // RAWDATANETCDFCREATORT_H
