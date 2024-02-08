#include "abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

namespace {

const int DEFAULT_CHART_HEIGHT = 80;
const int DEFAULT_COLORBAR_HEIGHT = 30;

} // namespace

AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::GridAttributeDisplaySettingContainer(const std::string& name, v4Grid::AttributeDataProvider* provider, Position pos, v4Structured2dGrid* g, const QString& prefix) :
	CompositeContainer({&visible, &mode,
										 &elevationChartLineWidth, &elevationChartLineColor,
										 &elevationChartShowPoint, &elevationChartPointSize,
										 &elevationChartShowIndex, &elevationChartIndexFont, &elevationChartIndexColor, &elevationChartIndexSkipRate,
										 &independentChartHeight, &independentChartLineWidth, &independentChartLineColor,
										 &independentChartTitleVisible, &independentChartTitleFont, &independentChartAxisFont, &independentChartAxisColor,
										 &colorBarHeight, &usePreColor, &colorBarTitleVisible, &colorBarTitleFont, &colorBarTitleColor}),
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
	colorBarHeight {"colorBarHeight", DEFAULT_COLORBAR_HEIGHT},
	colorBarTitleVisible {"colorBarTitleVisible", true},
	colorBarTitleFont {"colorBarTitleFont", QFont("MS UI Gothic", 12)},
	colorBarTitleColor {"colorBarTitleColor", Qt::black},
	usePreColor {"usePreColor", true},
	colorMapSetting {nullptr},
	grid {g},
	captionPrefix {prefix},
	m_attributeName {name},
	m_attributeDataProvider {provider},
	m_position {pos}
{
	colorMapSetting = provider->createColorMapSetting(name);
	auto cm = dynamic_cast<CompositeContainer*> (colorMapSetting);
	cm->addPrefix("colorMap");
}

AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::GridAttributeDisplaySettingContainer(const GridAttributeDisplaySettingContainer& setting) :
	GridAttributeDisplaySettingContainer {setting.m_attributeName, setting.m_attributeDataProvider, setting.m_position, setting.grid, setting.captionPrefix}
{
	copyValue(setting);
}

AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer& AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::operator=(const GridAttributeDisplaySettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const GridAttributeDisplaySettingContainer&> (c));
}

const std::string& AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::attributeName() const
{
	return m_attributeName;
}

v4Grid::AttributeDataProvider* AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::attributeDataProvider() const
{
	return m_attributeDataProvider;
}

AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::Position AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::position() const
{
	return m_position;
}

void AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	colorMapSetting->load(node);
}

void AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	colorMapSetting->save(writer);
}
