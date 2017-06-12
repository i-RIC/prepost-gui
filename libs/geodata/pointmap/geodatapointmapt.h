#ifndef GEODATAPOINTMAPT_H
#define GEODATAPOINTMAPT_H

#include "geodatapointmap.h"

class GeoDataMapper;

template <class V, class DA>
class GeoDataPointMapT : public GeoDataPointmap
{
public:
	GeoDataPointMapT(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	DA* vtkValues() const;

	V value(vtkIdType index) const;
	void setValue(vtkIdType index, V val);

	bool getValueRange(double* min, double* max) override;
	GeoDataMapper* mapper() const override;

private:
	GeoDataMapper* m_templateMapper;
};

#include "private/geodatapointmapt_detail.h"

#endif // GEODATAPOINTMAPT_H
