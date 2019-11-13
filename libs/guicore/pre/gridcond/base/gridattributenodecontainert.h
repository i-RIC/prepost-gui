#ifndef GRIDATTRIBUTENODECONTAINERT_H
#define GRIDATTRIBUTENODECONTAINERT_H

#include "gridattributecontainert.h"

template <class V, class DA>
class GridAttributeNodeContainerT : public GridAttributeContainerT<V>
{
public:
	/// Constructor
	GridAttributeNodeContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond);
	virtual ~GridAttributeNodeContainerT();

	unsigned int dataCount() const override;

	V value(unsigned int index) const override;
	void setValue(unsigned int index, V val) override;

	DA* dataArray() const override;
	DA* dataArrayCopy() const override;

	void allocate() override;
};

#include "private/gridattributenodecontainert_detail.h"

#endif // GRIDATTRIBUTENODECONTAINERT_H
