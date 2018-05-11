#ifndef GEODATANETCDFT_H
#define GEODATANETCDFT_H

#include "geodatanetcdf.h"

#include <vtkSmartPointer.h>

#include <vector>

template <class V, class DA>
class GeoDataNetcdfT : public GeoDataNetcdf
{

public:
	GeoDataNetcdfT(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	virtual ~GeoDataNetcdfT();

	DA* vtkValues() const;

	V value(vtkIdType index) const;
	void setValue(vtkIdType index, V val);

	virtual V missingValue() const = 0;

	bool getValueRange(double* min, double* max) override;

	int outputValues(int ncid, int varid, V* vals);
	int outputValues(int ncid, int varid, const std::vector<int>& indices, V* vals);

private:
	void doHandleDimensionCurrentIndexChange(int oldIndex, int newIndex) override;
	void doHandleDimensionValuesChange(GridAttributeDimensionContainer* cont, const std::vector<QVariant>& before, const std::vector<QVariant>& after) override;
};

#include "private/geodatanetcdft_detail.h"

#endif // GEODATANETCDFT_H
