#ifndef RAWDATAPOINTMAPT_H
#define RAWDATAPOINTMAPT_H

#include "rawdatapointmap.h"
#include "rawdatapointmapmappingmode.h"
#include "rawdatapointmaptemplatemappingsetting.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkVertex.h>

class RawDataMapper;

template <class V, class DA>
class RawDataPointMapT : public RawDataPointmap
{
public:
	RawDataPointMapT(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition);

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
	RawDataMapper* mapper();

private:
	RawDataMapper* m_templateMapper;
};

#include "rawdatapointmaptemplatenodemappert.h"

template <class V, class DA>
RawDataPointMapT<V, DA>::RawDataPointMapT(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
	: RawDataPointmap(d, creator, condition)
{
	vtkSmartPointer<DA> values = vtkSmartPointer<DA>::New();
	values->SetName("values");
	m_vtkGrid->GetPointData()->AddArray(values);

	m_templateMapper = new RawDataPointmapTemplateNodeMapperT<V, DA>(creator);
}

template <class V, class DA>
RawDataMapper* RawDataPointMapT<V, DA>::mapper()
{
	if (m_gridRelatedCondition->position() == SolverDefinitionGridRelatedCondition::CellCenter) {
		return RawData::mapper();
	}
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent()->parent()->parent()->parent());
	if (gtItem->gridType()->defaultGridType() != SolverDefinitionGridType::gtStructured2DGrid) {
		return RawData::mapper();
	}

	if (RawDataPointmapMappingMode::mode == RawDataPointmapMappingMode::mTIN) {
		return RawData::mapper();
	}

	RawDataPointmapTemplateMappingSetting s = RawDataPointmapTemplateMappingSetting::setting;
	RawDataPointmapTemplateNodeMapperT<V, DA>* tMapper =
		dynamic_cast<RawDataPointmapTemplateNodeMapperT<V, DA>* >(m_templateMapper);

	tMapper->setAutoMode(s.tempAutoMode);
	tMapper->setStreamWiseLength(s.tempStreamWiseLength);
	tMapper->setCrossStreamLength(s.tempCrossStreamLength);
	tMapper->setNumExpansions(s.tempNumExpansion);
	tMapper->setWeightPowVal(s.tempWeightExponent);
	return tMapper;
}

#endif // RAWDATAPOINTMAPT_H
