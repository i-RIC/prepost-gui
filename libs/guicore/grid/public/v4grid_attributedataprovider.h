#ifndef V4GRID_ATTRIBUTEDATAPROVIDER_H
#define V4GRID_ATTRIBUTEDATAPROVIDER_H

#include "../../guicore_global.h"

#include "../v4grid.h"

class ColorMapLegendSettingContainerI;
class ColorMapSettingContainerI;
class ColorMapSettingEditDialog;
class ColorMapSettingEditWidgetI;
class ColorMapSettingToolBarWidget;
class GridAttributeStringConverter;

class GUICOREDLL_EXPORT v4Grid::AttributeDataProvider
{
public:
	virtual ~AttributeDataProvider();

	virtual QString caption(const std::string& name) const = 0;
	virtual QVariant attributeBrowserValue(const std::string& name, double value) const = 0;
	virtual ColorMapSettingContainerI* createColorMapSetting(const std::string& name) const = 0;
	virtual ColorMapLegendSettingContainerI* createColorMapLegendSettingContainer(const std::string& name) const = 0;
	virtual ColorMapSettingEditWidgetI* createColorMapSettingEditWidget(const std::string& name, QWidget* parent) const = 0;
	virtual ColorMapSettingEditDialog* createColorMapSettingEditDialog(const std::string& name, QWidget* parent) const = 0;
	virtual ColorMapSettingToolBarWidget* createColorMapSettingToolbarWidget(const std::string& name, QWidget* parent) const = 0;
	virtual GridAttributeStringConverter* createStringConverter(const std::string& name) const = 0;
};

#endif // V4GRID_ATTRIBUTEDATAPROVIDER_H
