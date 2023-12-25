#ifndef GRIDATTRIBUTECONTAINERT_H
#define GRIDATTRIBUTECONTAINERT_H

#include "gridattributecontainer.h"
#include "../../../solverdef/solverdefinitiongridattributet.h"

class vtkDataArray;

template <class V, class DA>
class GridAttributeContainerT : public GridAttributeContainer
{
public:
	GridAttributeContainerT(v4InputGrid* grid, SolverDefinitionGridAttributeT<V>* cond);
	~GridAttributeContainerT();

	V value(unsigned int index) const;
	void setValue(unsigned int index, V value);

	int loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts) override;
	int loadFromCgnsFileForIndex(const iRICLib::H5CgnsGridAttributes& atts, int index);
	int saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts) override;
	int saveToCgnsFileForIndex(iRICLib::H5CgnsGridAttributes* atts, int index);

	DA* dataArray() const;
	DA* dataArrayCopy() const;
	void setDefaultValue() override;
	void allocate() override;

	void setModified() override;

	bool loadFromExternalFile(const QString& filename) override;
	bool saveToExternalFile(const QString& filename) override;
	bool getValueRange(double* min, double* max) override;

private:
	void addArrayIfNeeded();
};

#include "private/gridattributecontainert_detail.h"

#endif // GRIDATTRIBUTECONTAINERT_H
