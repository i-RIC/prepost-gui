#include "../../solverdef/solverdefinitiongridoutput.h"
#include "v4solutiongrid_attributedataprovider.h"

#include <misc/stringtool.h>

#include <QRegExp>

v4SolutionGrid::AttributeDataProvider::AttributeDataProvider(SolverDefinitionGridType* gridType) :
	m_gridType {gridType}
{}

QString v4SolutionGrid::AttributeDataProvider::caption(const std::string& name) const
{
	return m_gridType->outputCaption(name);
}

QVariant v4SolutionGrid::AttributeDataProvider::attributeBrowserValue(const std::string& name, double value) const
{
	return m_gridType->output(name)->attributeBrowserValue(value);
}

ColorMapSettingContainerI* v4SolutionGrid::AttributeDataProvider::createColorMapSetting(const std::string& name) const
{
	return output(name)->createColorMapSettingContainer();
}

ColorMapLegendSettingContainerI* v4SolutionGrid::AttributeDataProvider::createColorMapLegendSettingContainer(const std::string& name) const
{
	return output(name)->createColorMapLegendSettingContainer();
}

ColorMapSettingEditWidgetI* v4SolutionGrid::AttributeDataProvider::createColorMapSettingEditWidget(const std::string& name, QWidget* parent) const
{
	return output(name)->createColorMapSettingEditWidget(parent);
}

ColorMapSettingEditDialog* v4SolutionGrid::AttributeDataProvider::createColorMapSettingEditDialog(const std::string& name, QWidget* parent) const
{
	return output(name)->createColorMapSettingEditDialog(parent);
}

ColorMapSettingToolBarWidget* v4SolutionGrid::AttributeDataProvider::createColorMapSettingToolbarWidget(const std::string& name, QWidget* parent) const
{
	return output(name)->createColorMapSettingToolbarWidget(parent);
}

GridAttributeStringConverter* v4SolutionGrid::AttributeDataProvider::createStringConverter(const std::string& name) const
{
	return output(name)->stringConverter();
}

SolverDefinitionGridOutput* v4SolutionGrid::AttributeDataProvider::output(const std::string& name) const
{
	QRegExp re("^(.+) \\(magnitude\\)$");
	auto pos = re.indexIn(name.c_str());
	if (pos >= 0) {
		return m_gridType->output(iRIC::toStr(re.cap(1)) + "X");
	} else {
		return m_gridType->output(name);
	}
}
