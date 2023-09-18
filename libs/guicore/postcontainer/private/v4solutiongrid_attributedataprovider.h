#ifndef V4SOLUTIONGRID_ATTRIBUTEDATAPROVIDER_H
#define V4SOLUTIONGRID_ATTRIBUTEDATAPROVIDER_H

#include "../v4solutiongrid.h"
#include "../../grid/public/v4grid_attributedataprovider.h"

class v4SolutionGrid::AttributeDataProvider : public v4Grid::AttributeDataProvider
{
public:
	AttributeDataProvider(SolverDefinitionGridType* gridType);

	QString caption(const std::string& name) const override;
	QVariant attributeBrowserValue(const std::string& name, double value) const override;
	ColorMapSettingContainerI* createColorMapSetting(const std::string& name) const override;
	ColorMapLegendSettingContainerI* createColorMapLegendSettingContainer(const std::string& name) const override;
	ColorMapSettingEditWidgetI* createColorMapSettingEditWidget(const std::string& name, QWidget* parent) const override;
	ColorMapSettingEditDialog* createColorMapSettingEditDialog(const std::string& name, QWidget* parent) const override;
	ColorMapSettingToolBarWidget* createColorMapSettingToolbarWidget(const std::string& name, QWidget* parent) const override;

private:
	SolverDefinitionGridOutput* output(const std::string& name) const;

	SolverDefinitionGridType* m_gridType;
};

#endif // V4SOLUTIONGRID_ATTRIBUTEDATAPROVIDER_H
