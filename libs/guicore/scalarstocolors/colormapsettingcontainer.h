#ifndef COLORMAPSETTINGCONTAINER_H
#define COLORMAPSETTINGCONTAINER_H

#include "../guicore_global.h"
#include "colormapsettingcontaineri.h"
#include "colormapsettingvaluecolorpaircontainer.h"
#include "colormaplegendsettingcontainer.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

#include <vector>

class vtkScalarsToColors;

class GUICOREDLL_EXPORT ColorMapSettingContainer : public ColorMapSettingContainerI, public CompositeContainer
{
public:
	enum class TransitionMode {
		Continuous,
		Discrete,
	};

	enum class ValueMode {
		Absolute,
		Relative,
	};

	ColorMapSettingContainer();
	ColorMapSettingContainer(const ColorMapSettingContainer& c);

	vtkMapper* buildCellDataMapper(vtkDataSet* data, bool ignoreTransparent) override;
	vtkMapper* buildPointDataMapper(vtkDataSet* data) override;
	void paintNodeData(double x1, double x2, double v1, double v2, double ymin, double ymax, QPainter* painter) override;
	void paintCellData(double x1, double x2, double v, double ymin, double ymax, QPainter* painter) override;

	ColorMapLegendSettingContainerI* legendSetting() override;
	ColorMapLegendSettingContainerI* copyLegendSetting() override;

	double getMinValue() const;
	double getMaxValue() const;

	ColorMapSettingContainer& operator=(const ColorMapSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;
	void copy(const ColorMapSettingContainerI& c) override;
	ColorMapSettingContainerI* copy() override;

	void copyValue(const XmlAttributeContainer& c) override;

	bool importData(const QString& fileName);
	bool exportData(const QString& fileName);

	EnumContainerT<TransitionMode> transitionMode;
	EnumContainerT<ValueMode> valueMode;

	BoolContainer autoValueRange; // only for transitionMode = Continuous and valueMode = Relative

	DoubleContainer minValue;
	DoubleContainer maxValue;

	BoolContainer fillUpper;
	BoolContainer fillLower;

	DoubleContainer colorTableMinValue;
	std::vector<ColorMapSettingValueColorPairContainer> colors;

	ColorMapLegendSettingContainer legend;

	double getColorTableMinValue() const;
	std::vector<ColorMapSettingValueColorPairContainer> getColors() const;

private:
	void importDefault();

	vtkMapper* buildCellDataMapperContinuous(vtkDataSet* data);
	vtkMapper* buildCellDataMapperDiscrete(vtkDataSet* data, bool ignoreTransparent);
	vtkMapper* buildPointDataMapperContinuous(vtkDataSet* data);
	vtkMapper* buildPointDataMapperDiscrete(vtkDataSet* data);

	vtkScalarsToColors* buildScalarsToColorsContinuous();
};

#endif // COLORMAPSETTINGCONTAINER_H
