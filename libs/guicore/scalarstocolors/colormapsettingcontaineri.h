#ifndef COLORMAPSETTINGCONTAINERI_H
#define COLORMAPSETTINGCONTAINERI_H

#include "../guicore_global.h"

class ColorMapLegendSettingContainerI;
class ValueRangeContainer;

class vtkDataSet;
class vtkMapper;

class QDomNode;
class QXmlStreamWriter;

class GUICOREDLL_EXPORT ColorMapSettingContainerI
{
public:
	ColorMapSettingContainerI();
	virtual ~ColorMapSettingContainerI();

	void setAutoValueRange(const ValueRangeContainer& range);
	void setAutoValueRange(double min, double max);

	virtual void load(const QDomNode& node) = 0;
	virtual void save(QXmlStreamWriter& writer) const = 0;
	virtual void copy(const ColorMapSettingContainerI& c) = 0;

	virtual vtkMapper* buildCellDataMapper(vtkDataSet* data, bool ignoreTransparent) = 0;
	virtual vtkMapper* buildPointDataMapper(vtkDataSet* data) = 0;

	virtual ColorMapLegendSettingContainerI* legendSetting() = 0;
	virtual ColorMapLegendSettingContainerI* copyLegendSetting() = 0;

	QString valueCaption;

	double autoMinValue;
	double autoMaxValue;
};

#endif // COLORMAPSETTINGCONTAINERI_H
