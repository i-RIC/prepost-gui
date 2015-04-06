#ifndef RAWDATAPOINTMAPT_H
#define RAWDATAPOINTMAPT_H

#include "rawdatapointmap.h"
#include "rawdatapointmaptemplatenodemappert.h"
#include "rawdatapointmapmappingmode.h"
#include "rawdatapointmaptemplatemappingsetting.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkVertex.h>

template <class V, class DA>
class RawDataPointMapT : public RawDataPointmap
{
public:
	RawDataPointMapT(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
		: RawDataPointmap(d, creator, condition)
	{
		vtkSmartPointer<DA> values = vtkSmartPointer<DA>::New();
		values->SetName("values");
		m_vtkGrid->GetPointData()->AddArray(values);

		m_templateMapper = new RawDataPointmapTemplateNodeMapperT<V, DA>(creator);
	}
	DA* vtkValues(){return DA::SafeDownCast(m_vtkGrid->GetPointData()->GetArray("values"));}
	V value(vtkIdType index){return vtkValues()->GetValue(index);}
	void setValue(vtkIdType index, V val){return vtkValues()->SetValue(index, val);}
	bool getValueRange(double* min, double* max)
	{
		if (vtkValues()->GetNumberOfTuples() == 0){return false;}
		double range[2];
		vtkValues()->GetRange(range);
		*min = range[0];
		*max = range[1];
		return true;
	}
	RawDataMapper* mapper()
	{
		if (m_gridRelatedCondition->position() == SolverDefinitionGridRelatedCondition::CellCenter){
			return RawData::mapper();
		}
		PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent()->parent()->parent()->parent());
		if (gtItem->gridType()->defaultGridType() != SolverDefinitionGridType::gtStructured2DGrid){
			return RawData::mapper();
		}

		if (RawDataPointmapMappingMode::mode == RawDataPointmapMappingMode::mTIN){
			return RawData::mapper();
		}

		RawDataPointmapTemplateMappingSetting s = RawDataPointmapTemplateMappingSetting::setting;
		m_templateMapper->setAutoMode(s.tempAutoMode);
		m_templateMapper->setStreamWiseLength(s.tempStreamWiseLength);
		m_templateMapper->setCrossStreamLength(s.tempCrossStreamLength);
		m_templateMapper->setNumExpansions(s.tempNumExpansion);
		m_templateMapper->setWeightPowVal(s.tempWeightExponent);
		return m_templateMapper;
	}

private:
	RawDataPointmapTemplateNodeMapperT<V, DA>* m_templateMapper;
};

#endif // RAWDATAPOINTMAPT_H
