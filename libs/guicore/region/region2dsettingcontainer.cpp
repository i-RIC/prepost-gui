#include "region2dsettingcontainer.h"
#include "../postcontainer/postzonedatacontainer.h"

#include <guibase/iricactivecellfilter.h>
#include <misc/stringtool.h>

#include <vtkClipPolyData.h>
#include <vtkExtractGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkSmartPointer.h>

Region2dSettingContainer::Region2dSettingContainer() :
	CompositeContainer {&mode, &iMin, &iMax, &jMin, &jMax},
	mode {"mode"},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"}
{}

Region2dSettingContainer::Region2dSettingContainer(const Region2dSettingContainer& s) :
	Region2dSettingContainer {}
{
	copyValue(s);
}

Region2dSettingContainer& Region2dSettingContainer::operator=(const Region2dSettingContainer& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& Region2dSettingContainer::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const Region2dSettingContainer&> (s));
}

vtkPointSet* Region2dSettingContainer::buildNodeFilteredData(vtkPointSet* data)
{
	if (mode == Mode::Full) {
		data->Register(nullptr);
		return data;
	} else if (mode == Mode::Active) {
		auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
		filter->SetInputData(data);
		auto clipper = vtkSmartPointer<vtkClipPolyData>::New();
		clipper->SetInputConnection(filter->GetOutputPort());
		clipper->SetValue(PostZoneDataContainer::IBCLimit);
		clipper->InsideOutOff();
		data->GetPointData()->SetActiveScalars(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
		clipper->Update();
		auto clipped = clipper->GetOutput();
		clipped->Register(nullptr);
		return clipped;
	} else if (mode == Mode::Custom) {
		auto filter = vtkSmartPointer<vtkExtractGrid>::New();
		filter->SetInputData(data);
		filter->SetVOI(iMin, iMax, jMin, jMax, 0, 0);
		filter->Update();
		auto clipped = filter->GetOutput();
		clipped->Register(nullptr);
		return clipped;
	}
	return nullptr;
}

vtkPointSet* Region2dSettingContainer::buildCellFilteredData(vtkPointSet* data)
{
	if (mode == Mode::Full) {
		data->Register(nullptr);
		return data;
	} else if (mode == Mode::Active) {
		auto filter = vtkSmartPointer<iricActiveCellFilter>::New();
		filter->SetInputData(data);
		filter->SetValue(PostZoneDataContainer::IBCLimit);
		filter->CellClippingOn();
		data->GetCellData()->SetActiveScalars(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
		filter->Update();
		auto activeData = filter->GetOutput();
		activeData->Register(nullptr);
		return activeData;
	} else if (mode == Mode::Custom) {
		auto filter = vtkSmartPointer<vtkExtractGrid>::New();
		filter->SetInputData(data);
		filter->SetVOI(iMin, iMax + 1, jMin, jMax + 1, 0, 0);
		filter->Update();
		auto clipped = filter->GetOutput();
		clipped->Register(nullptr);
		return clipped;
	}
	return nullptr;
}
