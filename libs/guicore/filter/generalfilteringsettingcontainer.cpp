#include "generalfilteringsettingcontainer.h"

#include <misc/stringtool.h>

#include <vtkMaskPoints.h>
#include <vtkSmartPointer.h>

const int GeneralFilteringSettingContainer::DEFAULT_SAMPLING_RATE = 1;
const int GeneralFilteringSettingContainer::DEFAULT_SAMPLING_NUMBER = 100;

GeneralFilteringSettingContainer::GeneralFilteringSettingContainer() :
	CompositeContainer({&samplingMode, &samplingRate, &samplingNumber}),
	samplingMode {"samplingMode", SamplingMode::All},
	samplingRate {"samplingRate", DEFAULT_SAMPLING_RATE},
	samplingNumber {"samplingNumber", DEFAULT_SAMPLING_NUMBER}
{}

GeneralFilteringSettingContainer::GeneralFilteringSettingContainer(const GeneralFilteringSettingContainer& c) :
	GeneralFilteringSettingContainer {}
{
	copyValue(c);
}

GeneralFilteringSettingContainer::~GeneralFilteringSettingContainer()
{}

GeneralFilteringSettingContainer& GeneralFilteringSettingContainer::operator=(const GeneralFilteringSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& GeneralFilteringSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const GeneralFilteringSettingContainer&> (c));
}

vtkPointSet* GeneralFilteringSettingContainer::buildSampledData(vtkPointSet* data) const
{
	if (samplingMode == SamplingMode::All) {
		data->Register(nullptr);
		return data;
	} else if (samplingMode == SamplingMode::Number) {
		auto mask = vtkSmartPointer<vtkMaskPoints>::New();
		mask->SetInputData(data);
		mask->RandomModeOn();
		mask->SetRandomModeType(1);
		mask->SetMaximumNumberOfPoints(samplingNumber);
		mask->Update();

		auto ret = mask->GetOutput();
		ret->Register(nullptr);
		return ret;
	} else if (samplingMode == SamplingMode::Rate) {
		auto mask = vtkSmartPointer<vtkMaskPoints>::New();
		mask->SetInputData(data);
		mask->RandomModeOff();
		mask->SetOnRatio(samplingRate);
		mask->Update();

		auto ret = mask->GetOutput();
		ret->Register(nullptr);
		return ret;
	}
	return nullptr;
}
