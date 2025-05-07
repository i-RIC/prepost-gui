#ifndef GEODATAGDALT_H
#define GEODATAGDALT_H

#include "geodatagdal.h"

#include <vtkSmartPointer.h>

#include <vector>

template <class V, class DA>
class GeoDataGdalT : public GeoDataGdal
{

public:
	GeoDataGdalT(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	virtual ~GeoDataGdalT();

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
	void loadRasterData(int index) override;
};

#include "private/geodatagdalt_detail.h"

#endif // GEODATAGDALT_H
