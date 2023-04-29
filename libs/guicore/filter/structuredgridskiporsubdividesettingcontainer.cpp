#include "structuredgridskiporsubdividesettingcontainer.h"

#include <guibase/vtksubdividegrid.h>
#include <vtkExtractGrid.h>
#include <vtkSmartPointer.h>

StructuredGridSkipOrSubdivideSettingContainer::StructuredGridSkipOrSubdivideSettingContainer() :
	CompositeContainer({&samplingMode, &skipRate, &subDivideRate}),
	samplingMode {"samplingMode", SamplingMode::Normal},
	skipRate {"skipRate", 1},
	subDivideRate {"subdivideRate", 1}
{}

StructuredGridSkipOrSubdivideSettingContainer::StructuredGridSkipOrSubdivideSettingContainer(const StructuredGridSkipOrSubdivideSettingContainer& s) :
	StructuredGridSkipOrSubdivideSettingContainer {}
{
	copyValue(s);
}

StructuredGridSkipOrSubdivideSettingContainer::~StructuredGridSkipOrSubdivideSettingContainer()
{}

StructuredGridSkipOrSubdivideSettingContainer& StructuredGridSkipOrSubdivideSettingContainer::operator=(const StructuredGridSkipOrSubdivideSettingContainer& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& StructuredGridSkipOrSubdivideSettingContainer::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const StructuredGridSkipOrSubdivideSettingContainer&> (setting));
}

vtkStructuredGrid* StructuredGridSkipOrSubdivideSettingContainer::buildSampledData(vtkStructuredGrid* data, Dimension dimension) const
{
	if (samplingMode == SamplingMode::Normal) {
		data->Register(nullptr);
		return data;
	} else if (samplingMode == SamplingMode::Skip) {
		auto extract = vtkSmartPointer<vtkExtractGrid>::New();
		extract->SetInputData(data);
		if (dimension == Dimension::Two) {
			extract->SetSampleRate(skipRate, skipRate, 1);
		} else if (dimension == Dimension::Three) {
			extract->SetSampleRate(skipRate, skipRate, skipRate);
		}
		extract->Update();
		auto result = extract->GetOutput();
		result->Register(nullptr);
		return result;
	} else if (samplingMode == SamplingMode::Subdivide) {
		auto subdivide = vtkSmartPointer<vtkSubdivideGrid>::New();
		subdivide->SetInputData(data);
		if (dimension == Dimension::Two) {
			subdivide->SetDivideRate(subDivideRate, subDivideRate, 1);
		} else if (dimension == Dimension::Three) {
			subdivide->SetDivideRate(subDivideRate, subDivideRate, subDivideRate);
		}
		subdivide->Update();
		auto result = subdivide->GetOutput();
		result->Register(nullptr);
		return result;
	} else {
		return nullptr;
	}
}
