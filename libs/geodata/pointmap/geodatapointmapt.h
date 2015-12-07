#ifndef GEODATAPOINTMAPT_H
#define GEODATAPOINTMAPT_H

#include "geodatapointmap.h"
#include "geodatapointmapmappingmode.h"
#include "geodatapointmaptemplatemappingsetting.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkVertex.h>

class GeoDataMapper;

template <class V, class DA>
class GeoDataPointMapT : public GeoDataPointmap
{

public:
	GeoDataPointMapT(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	DA* vtkValues() {return DA::SafeDownCast(m_vtkGrid->GetPointData()->GetArray("values"));}
	V value(vtkIdType index) {return vtkValues()->GetValue(index);}
	void setValue(vtkIdType index, V val) {return vtkValues()->SetValue(index, val);}
	bool getValueRange(double* min, double* max) override {
		if (vtkValues()->GetNumberOfTuples() == 0) {return false;}
		double range[2];
		vtkValues()->GetRange(range);
		*min = range[0];
		*max = range[1];
		return true;
	}
	GeoDataMapper* mapper();

private:
	GeoDataMapper* m_templateMapper;
};

#include "geodatapointmaptemplatenodemappert.h"

template <class V, class DA>
GeoDataPointMapT<V, DA>::GeoDataPointMapT(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition)
	: GeoDataPointmap(d, creator, condition)
{
	vtkSmartPointer<DA> values = vtkSmartPointer<DA>::New();
	values->SetName("values");
	m_vtkGrid->GetPointData()->AddArray(values);

	m_templateMapper = new GeoDataPointmapTemplateNodeMapperT<V, DA>(creator);
}

template <class V, class DA>
GeoDataMapper* GeoDataPointMapT<V, DA>::mapper()
{
	if (m_gridAttribute->position() == SolverDefinitionGridAttribute::CellCenter) {
		return GeoData::mapper();
	}
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent()->parent()->parent()->parent());
	if (gtItem->gridType()->defaultGridType() != SolverDefinitionGridType::gtStructured2DGrid) {
		return GeoData::mapper();
	}

	if (GeoDataPointmapMappingMode::mode == GeoDataPointmapMappingMode::mTIN) {
		return GeoData::mapper();
	}

	GeoDataPointmapTemplateMappingSetting s = GeoDataPointmapTemplateMappingSetting::setting;
	GeoDataPointmapTemplateNodeMapperT<V, DA>* tMapper =
		dynamic_cast<GeoDataPointmapTemplateNodeMapperT<V, DA>* >(m_templateMapper);

	tMapper->setAutoMode(s.tempAutoMode);
	tMapper->setStreamWiseLength(s.tempStreamWiseLength);
	tMapper->setCrossStreamLength(s.tempCrossStreamLength);
	tMapper->setNumExpansions(s.tempNumExpansion);
	tMapper->setWeightPowVal(s.tempWeightExponent);
	return tMapper;
}

#endif // GEODATAPOINTMAPT_H
