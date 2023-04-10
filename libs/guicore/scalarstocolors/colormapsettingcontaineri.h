#ifndef COLORMAPSETTINGCONTAINERI_H
#define COLORMAPSETTINGCONTAINERI_H

#include "../guicore_global.h"

class ColorMapLegendSettingContainerI;
class ValueRangeContainer;

class vtkDataSet;
class vtkMapper;

class QDomNode;
class QPainter;
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
	virtual void paintNodeData(double x1, double x2, double v1, double v2, double ymin, double ymax, QPainter* painter) = 0;
	virtual void paintCellData(double x1, double x2, double v, double ymin, double ymax, QPainter* painter) = 0;

	virtual ColorMapLegendSettingContainerI* legendSetting() = 0;
	virtual ColorMapLegendSettingContainerI* copyLegendSetting() = 0;

	QString valueCaption;

	double autoMinValue;
	double autoMaxValue;
};

#endif // COLORMAPSETTINGCONTAINERI_H
