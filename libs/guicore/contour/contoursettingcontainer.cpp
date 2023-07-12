#include "contoursettingcontainer.h"
#include "../scalarstocolors/colormapsettingcontainer.h"

#include <vtkContourFilter.h>

ContourSettingContainer::ContourSettingContainer() :
	CompositeContainer({&drawContourLines, &contourLineWidth, &mode, &numberOfValues}),
	drawContourLines {"drawContourLines", false},
	contourLineWidth {"contourLineWidth", 1},
	mode {"contourMode", Mode::UseColorMapValues},
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

void ContourSettingContainer::setColorMapSetting(ColorMapSettingContainerI* setting)
{
	m_colorMapSetting = setting;
}

vtkPointSet* ContourSettingContainer::buildFilteredData(vtkPointSet* data) const
{
	if (! drawContourLines) {
		data->Register(nullptr);
		return data;
	}
	auto cs = dynamic_cast<ColorMapSettingContainer*> (m_colorMapSetting);
	if (cs == nullptr) {
		data->Register(nullptr);
		return data;
	}

	auto cols = cs->getColors();
	if (mode == Mode::UseColorMapValues) {
		m_filter->SetNumberOfContours(cols.size());
		for (int i = 0; i < cols.size(); ++i) {
			const auto& c = cols.at(i);
			m_filter->SetValue(i, c.value.value());
		}
	} else if (mode == Mode::SpecifyNumberOfValues) {
		auto min = cs->getMinValue();
		auto max = cs->getMaxValue();
		m_filter->SetNumberOfContours(numberOfValues + 1);
		m_filter->SetValue(0, min);
		m_filter->SetValue(numberOfValues, max);
		for (int i = 1; i < numberOfValues; ++i) {
			m_filter->SetValue(i, min + (i / static_cast<double>(numberOfValues)) * (max - min));
		}
	}
	m_filter->SetInputData(data);
	m_filter->Update();

	auto output = m_filter->GetOutput();
	output->Register(nullptr);
	return output;
}
