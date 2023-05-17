#include "contoursettingcontainer.h"
#include "../scalarstocolors/colormapsettingcontainer.h"

#include <vtkContourFilter.h>

ContourSettingContainer::ContourSettingContainer() :
	CompositeContainer({&drawContourLines, &contourLineWidth, &mode, &numberOfValues}),
	drawContourLines {"drawContourLines", false},
	contourLineWidth {"contourLineWidth", 1},
	mode {"mode", Mode::UseColorMapValues},
	numberOfValues {"numberOfValues", 10},
	m_colorMapSetting {nullptr},
	m_filter {vtkContourFilter::New()}
{}

ContourSettingContainer::ContourSettingContainer(const ContourSettingContainer& c) :
	ContourSettingContainer {}
{
	copyValue(c);
}

ContourSettingContainer::~ContourSettingContainer()
{
	m_filter->Delete();
}

ContourSettingContainer& ContourSettingContainer::operator=(const ContourSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ContourSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ContourSettingContainer&> (c));
}

void ContourSettingContainer::setColorMapSetting(ColorMapSettingContainer* setting)
{
	m_colorMapSetting = setting;
}

vtkPointSet* ContourSettingContainer::buildFilteredData(vtkPointSet* data) const
{
	if (! drawContourLines) {
		data->Register(nullptr);
		return data;
	}

	auto cols = m_colorMapSetting->getColors();
	if (mode == Mode::UseColorMapValues) {
		m_filter->SetNumberOfContours(cols.size());
		for (int i = 0; i < cols.size(); ++i) {
			const auto& c = cols.at(i);
			m_filter->SetValue(i, c.value.value());
		}
	} else if (mode == Mode::SpecifyNumberOfValues) {
		auto min = m_colorMapSetting->getMinValue();
		auto max = m_colorMapSetting->getMaxValue();
		m_filter->SetNumberOfContours(numberOfValues);
		m_filter->SetValue(0, min);
		m_filter->SetValue(numberOfValues - 1, max);
		for (int i = 1; i < numberOfValues - 1; ++i) {
			m_filter->SetValue(i, min + (i / static_cast<double>(numberOfValues - 1)) * (max - min));
		}
	}
	m_filter->SetInputData(data);
	m_filter->Update();

	auto output = m_filter->GetOutput();
	output->Register(nullptr);
	return output;
}
