#include "structured2dfilteringsettingcontainer.h"

#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>

Structured2dFilteringSettingContainer::Structured2dFilteringSettingContainer() :
	CompositeContainer({&samplingMode, &iSampleRate, &jSampleRate}),
	samplingMode {"samplingMode", SamplingMode::All},
	iSampleRate {"iSampleRate", 1},
	jSampleRate {"jSampleRate", 1}
{}

Structured2dFilteringSettingContainer::Structured2dFilteringSettingContainer(const Structured2dFilteringSettingContainer& c) :
	Structured2dFilteringSettingContainer {}
{
	copyValue(c);
}

Structured2dFilteringSettingContainer& Structured2dFilteringSettingContainer::operator=(const Structured2dFilteringSettingContainer& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Structured2dFilteringSettingContainer::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Structured2dFilteringSettingContainer&> (setting));
}

vtkStructuredGrid* Structured2dFilteringSettingContainer::buildSampledData(vtkStructuredGrid* data) const
{
	if (samplingMode == SamplingMode::All) {
		data->Register(nullptr);
		return data;
	} else {
		auto filter = vtkSmartPointer<vtkExtractGrid>::New();
		filter->SetInputData(data);
		filter->SetSampleRate(iSampleRate, jSampleRate, 1);
		filter->Update();

		auto ret = filter->GetOutput();
		ret->Register(nullptr);
		return ret;
	}
}

