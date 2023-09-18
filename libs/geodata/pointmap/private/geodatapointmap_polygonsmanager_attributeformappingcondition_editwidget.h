#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_EDITWIDGET_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_EDITWIDGET_H

#include "geodatapointmap_polygonsmanager_attributeformappingcondition.h"
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>

#include <QWidget>

namespace Ui {
class GeoDataPointmap_PolygonsManager_AttributeForMappingCondition_EditWidget;
}

class GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget : public GridAttributeEditWidget
{
	Q_OBJECT

public:
	EditWidget(QWidget *parent, SolverDefinitionGridAttribute* att);
	~EditWidget();

	QVariant variantValue() const override;
	void setVariantValue(const QVariant& v) override;

	void scanAndSetDefault(GridAttributeContainer* container, const std::vector<vtkIdType>& indices) override;
	void applyValue(GridAttributeContainer* container, const std::vector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem) override;

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

private:
	void getValueFromInnerWidget() const override;
	void setupWidget() override;

	QWidget* editWidget() const override;

	Ui::GeoDataPointmap_PolygonsManager_AttributeForMappingCondition_EditWidget *ui;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_EDITWIDGET_H
