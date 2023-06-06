#include "preprocessorgridcrosssectionwindow2_gridattributedisplaysettingcontainer.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

namespace {

const int DEFAULT_CHART_HEIGHT = 80;
const int DEFAULT_COLORMAP_HEIGHT = 30;

} // namespace

PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer::GridAttributeDisplaySettingContainer(SolverDefinitionGridAttribute* att) :
	CompositeContainer({&visible, &mode,
										 &elevationChartLineWidth, &elevationChartLineColor,
										 &elevationChartShowPoint, &elevationChartPointSize,
										 &elevationChartShowIndex, &elevationChartIndexFont, &elevationChartIndexColor, &elevationChartIndexSkipRate,
										 &independentChartHeight, &independentChartLineWidth, &independentChartLineColor,
										 &independentChartTitleVisible, &independentChartTitleFont, &independentChartAxisFont, &independentChartAxisColor,
										 &colorMapHeight, &usePreColor, &colorMapTitleVisible, &colorMapTitleFont, &colorMapTitleColor}),
	visible {"visible", false},
	mode {"mode", Mode::ColorMap},
	elevationChartLineWidth {"elevationChartLineWidth", 2},
	elevationChartLineColor {"elevationChartLineColor", Qt::black},
	elevationChartShowPoint {"elevationChartShowPoint", true},
	elevationChartPointSize {"elevationChartPointSize", 5},
	elevationChartShowIndex {"elevationChartShowIndex", true},
	elevationChartIndexFont {"elevationChartIndexFont", QFont("MS UI Gothic", 12)},
	elevationChartIndexColor {"elevationChartIndexColor", Qt::black},
	elevationChartIndexSkipRate {"elevationChartIndexSkipRate", 5},
	independentChartHeight {"independentChartHeight", DEFAULT_CHART_HEIGHT},
	independentChartLineWidth {"independentChartLineWidth", 1},
	independentChartLineColor {"independentChartLineColor", Qt::black},
	independentChartTitleVisible {"independentChartTitleVisible", true},
	independentChartTitleFont {"independentChartTitleFont", QFont("MS UI Gothic", 12)},
	independentChartAxisFont {"independentChartAxisFont", QFont("MS UI Gothic", 10)},
	independentChartAxisColor{"independentChartAxisColor", Qt::darkGray},
	colorMapHeight {"colorMapHeight", DEFAULT_COLORMAP_HEIGHT},
	colorMapTitleVisible {"colorMapTitleVisible", true},
	colorMapTitleFont {"colorMapTitleFont", QFont("MS UI Gothic", 12)},
	colorMapTitleColor {"colorMapTitleColor", Qt::black},
	usePreColor {"usePreColor", true},
	colorMapSetting {nullptr},
	m_attribute {att}
{
	colorMapSetting = att->createColorMapSettingContainer();
	auto cm = dynamic_cast<CompositeContainer*> (colorMapSetting);
	cm->addPrefix("colorMap");
}

PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer::GridAttributeDisplaySettingContainer(const GridAttributeDisplaySettingContainer& setting) :
	GridAttributeDisplaySettingContainer {setting.m_attribute}
{
	copyValue(setting);
}

PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer& PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer::operator=(const GridAttributeDisplaySettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const GridAttributeDisplaySettingContainer&> (c));
}

SolverDefinitionGridAttribute* PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer::attribute() const
{
	return m_attribute;
}

void PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	colorMapSetting->load(node);
}

void PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	colorMapSetting->save(writer);
}
