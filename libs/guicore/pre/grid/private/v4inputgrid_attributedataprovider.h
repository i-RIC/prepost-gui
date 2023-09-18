#ifndef V4INPUTGRID_ATTRIBUTEDATAPROVIDER_H
#define V4INPUTGRID_ATTRIBUTEDATAPROVIDER_H

#include "../v4inputgrid.h"
#include "../../../grid/public/v4grid_attributedataprovider.h"

class PreProcessorGeoDataTopDataItemI;

class v4InputGrid::AttributeDataProvider : public v4Grid::AttributeDataProvider
{
public:
	AttributeDataProvider(SolverDefinitionGridType* gridType);

	void setGeoDataTopDataItem(PreProcessorGeoDataTopDataItemI* item);

	QString caption(const std::string& name) const override;
	QVariant attributeBrowserValue(const std::string& name, double value) const override;
	ColorMapSettingContainerI* createColorMapSetting(const std::string& name) const override;
	ColorMapLegendSettingContainerI* createColorMapLegendSettingContainer(const std::string& name) const override;
	ColorMapSettingEditWidgetI* createColorMapSettingEditWidget(const std::string& name, QWidget* parent) const override;
	ColorMapSettingEditDialog* createColorMapSettingEditDialog(const std::string& name, QWidget* parent) const override;
	ColorMapSettingToolBarWidget* createColorMapSettingToolbarWidget(const std::string& name, QWidget* parent) const override;

private:
	SolverDefinitionGridAttribute* attribute(const std::string& name) const;

	SolverDefinitionGridType* m_gridType;
	PreProcessorGeoDataTopDataItemI* m_geoDataTopDataItem;
};

#endif // V4INPUTGRID_ATTRIBUTEDATAPROVIDER_H
