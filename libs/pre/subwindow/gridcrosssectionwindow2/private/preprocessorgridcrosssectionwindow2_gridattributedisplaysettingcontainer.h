#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRIDATTRIBUTEDISPLAYSETTINGCONTAINER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRIDATTRIBUTEDISPLAYSETTINGCONTAINER_H

#include "../preprocessorgridcrosssectionwindow2.h"

#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/qfontcontainer.h>

class ColorMapSettingContainerI;
class SolverDefinitionGridAttribute;

class PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer : public CompositeContainer
{
public:
	static const int MIN_HEIGHT = 10;
	enum class Mode {
		AsElevation,
		Chart,
		ColorMap,
	};

	GridAttributeDisplaySettingContainer(SolverDefinitionGridAttribute* att);
	GridAttributeDisplaySettingContainer(const GridAttributeDisplaySettingContainer& setting);

	GridAttributeDisplaySettingContainer& operator=(const GridAttributeDisplaySettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	SolverDefinitionGridAttribute* attribute() const;

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

private:
	SolverDefinitionGridAttribute* m_attribute;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRIDATTRIBUTEDISPLAYSETTINGCONTAINER_H
