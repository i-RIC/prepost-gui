#ifndef GRIDATTRIBUTECONTAINERT_H
#define GRIDATTRIBUTECONTAINERT_H

#include "gridattributecontainer.h"
#include "../../../solverdef/solverdefinitiongridattributet.h"

class vtkDataArray;

template <class V>
class GridAttributeContainerT : public GridAttributeContainer
{
public:
	GridAttributeContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond);
	virtual ~GridAttributeContainerT();

	virtual V value(unsigned int index) const = 0;
	virtual void setValue(unsigned int index, V value) = 0;

	bool loadFromCgnsFile(int fn, int B, int Z) override;
	bool loadFromCgnsFileForIndex(int fn, int B, int Z, int index);
	bool saveToCgnsFile(int fn, int B, int Z) override;
	bool saveToCgnsFileForIndex(int index);

	virtual vtkDataArray* dataArray() const = 0;
	virtual vtkDataArray* dataArrayCopy() const = 0;
	virtual void setModified() override;

	bool loadFromExternalFile(const QString& filename) override;
	bool saveToExternalFile(const QString& filename) override;
	bool getValueRange(double* min, double* max) override;

protected:
	QString arrayNameForIndex(int index) const;
	virtual DataType_t dataType() const = 0;
};

#include "private/gridattributecontainert_detail.h"

#endif // GRIDATTRIBUTECONTAINERT_H
