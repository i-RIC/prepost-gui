#ifndef ABSTRACTCROSSSECTIONWINDOW_GRIDATTRIBUTEDISPLAYSETTINGCONTAINER_H
#define ABSTRACTCROSSSECTIONWINDOW_GRIDATTRIBUTEDISPLAYSETTINGCONTAINER_H

#include "../abstractcrosssectionwindow.h"

#include <guicore/grid/public/v4grid_attributedataprovider.h>
#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/qfontcontainer.h>

class ColorMapSettingContainerI;
class SolverDefinitionGridAttribute;
class v4Structured2dGrid;

class AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer : public CompositeContainer
{
public:
	static const int MIN_HEIGHT = 10;
	enum class Mode {
		AsElevation,
		Chart,
		ColorMap,
	};
	enum class Position {
		Node,
		Cell,
		IEdge,
		JEdge
	};

	GridAttributeDisplaySettingContainer(const std::string& name, v4Grid::AttributeDataProvider* provider, Position pos, v4Structured2dGrid* grid, const QString& prefix);
	GridAttributeDisplaySettingContainer(const GridAttributeDisplaySettingContainer& setting);

	GridAttributeDisplaySettingContainer& operator=(const GridAttributeDisplaySettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	const std::string& attributeName() const;
	v4Grid::AttributeDataProvider* attributeDataProvider() const;
	Position position() const;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	BoolContainer visible;
	EnumContainerT<Mode> mode;

	IntContainer elevationChartLineWidth;
	ColorContainer elevationChartLineColor;

	BoolContainer elevationChartShowPoint;
	IntContainer elevationChartPointSize;
	BoolContainer elevationChartShowIndex;
	QFontContainer elevationChartIndexFont;
	ColorContainer elevationChartIndexColor;
	IntContainer elevationChartIndexSkipRate;

	IntContainer independentChartHeight;
	IntContainer independentChartLineWidth;
	ColorContainer independentChartLineColor;
	BoolContainer independentChartTitleVisible;
	QFontContainer independentChartTitleFont;
	QFontContainer independentChartAxisFont;
	ColorContainer independentChartAxisColor;

	IntContainer colorMapHeight;
	BoolContainer colorMapTitleVisible;
	QFontContainer colorMapTitleFont;
	ColorContainer colorMapTitleColor;

	BoolContainer usePreColor;
	ColorMapSettingContainerI* colorMapSetting;

	v4Structured2dGrid* grid;
	QString captionPrefix;

private:
	std::string m_attributeName;
	v4Grid::AttributeDataProvider* m_attributeDataProvider;
	Position m_position;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRIDATTRIBUTEDISPLAYSETTINGCONTAINER_H
