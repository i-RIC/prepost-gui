#ifndef GEODATAPOINTMAPT_DETAIL_H
#define GEODATAPOINTMAPT_DETAIL_H

#include "../geodatapointmapt.h"
#include "../geodatapointmapmappingmode.h"
#include "../geodatapointmaptemplatemappingsetting.h"
#include "../geodatapointmaptemplatenodemappert.h"

#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>

template <class V, class DA>
GeoDataPointMapT<V, DA>::GeoDataPointMapT(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataPointmap(d, creator, condition)
{
	vtkSmartPointer<DA> values = vtkSmartPointer<DA>::New();
	values->SetName("values");
	m_vtkGrid->GetPointData()->AddArray(values);

	m_templateMapper = new GeoDataPointmapTemplateNodeMapperT<V, DA>(creator);
}

template <class V, class DA>
DA* GeoDataPointMapT<V, DA>::vtkValues() const
{
	return DA::SafeDownCast(m_vtkGrid->GetPointData()->GetArray("values"));
}

template <class V, class DA>
V GeoDataPointMapT<V, DA>::value(vtkIdType index) const
{
	return vtkValues()->GetValue(index);
}

template <class V, class DA>
void GeoDataPointMapT<V, DA>::setValue(vtkIdType index, V val)
{
	vtkValues()->SetValue(index, val);
}

template <class V, class DA>
bool GeoDataPointMapT<V, DA>::getValueRange(double* min, double* max)
{
	if (vtkValues()->GetNumberOfTuples() == 0) {return false;}
	double range[2];
	vtkValues()->GetRange(range);
	*min = range[0];
	*max = range[1];
	return true;
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

#endif // GEODATAPOINTMAPT_DETAIL_H
