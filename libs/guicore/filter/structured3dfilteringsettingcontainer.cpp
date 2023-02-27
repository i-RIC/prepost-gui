#include "structured3dfilteringsettingcontainer.h"

#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>

Structured3dFilteringSettingContainer::Structured3dFilteringSettingContainer() :
	CompositeContainer({&samplingMode, &iSampleRate, &jSampleRate, &kSampleRate}),
	samplingMode {"samplingMode", SamplingMode::All},
	iSampleRate {"iSampleRate", 1},
	jSampleRate {"jSampleRate", 1},
	kSampleRate {"kSampleRate", 1}
{}

Structured3dFilteringSettingContainer::Structured3dFilteringSettingContainer(const Structured3dFilteringSettingContainer& c) :
	Structured3dFilteringSettingContainer {}
{
	copyValue(c);
}

Structured3dFilteringSettingContainer& Structured3dFilteringSettingContainer::operator=(const Structured3dFilteringSettingContainer& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Structured3dFilteringSettingContainer::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Structured3dFilteringSettingContainer&> (setting));
}

vtkStructuredGrid* Structured3dFilteringSettingContainer::buildSampledData(vtkStructuredGrid* data) const
{
	if (samplingMode == SamplingMode::All) {
		data->Register(nullptr);
		return data;
	} else {
		auto filter = vtkSmartPointer<vtkExtractGrid>::New();
		filter->SetInputData(data);
		filter->SetSampleRate(iSampleRate, jSampleRate, kSampleRate);
		filter->Update();

		auto ret = filter->GetOutput();
		ret->Register(nullptr);
		return ret;
	}
}

