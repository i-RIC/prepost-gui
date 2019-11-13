#ifndef GRIDATTRIBUTECELLCONTAINERT_H
#define GRIDATTRIBUTECELLCONTAINERT_H

#include "gridattributecontainert.h"

template <class V, class DA>
class GridAttributeCellContainerT : public GridAttributeContainerT<V>
{
public:
	GridAttributeCellContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond);
	virtual ~GridAttributeCellContainerT();

	unsigned int dataCount() const override;

	V value(unsigned int index) const override;
	void setValue(unsigned int index, V val) override;

	DA* dataArray() const override;
	DA* dataArrayCopy() const override;

	void allocate() override;
};

#include "private/gridattributecellcontainert_detail.h"

#endif // GRIDATTRIBUTECELLCONTAINERT_H
