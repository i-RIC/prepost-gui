#ifndef GRIDATTRIBUTEVARIATIONEDITWIDGET_H
#define GRIDATTRIBUTEVARIATIONEDITWIDGET_H

#include "../../../guicore_global.h"

#include <QWidget>
#include <vtkIdList.h>

class SolverDefinitionGridAttribute;
class GridAttributeContainer;
class vtkDataSetAttributes;
class PreProcessorGridDataItemInterface;

class GUICOREDLL_EXPORT GridAttributeVariationEditWidget : public QWidget
{
	Q_OBJECT

public:
	GridAttributeVariationEditWidget(QWidget* parent, SolverDefinitionGridAttribute* condition);
	virtual ~GridAttributeVariationEditWidget();

	void clearValue();

	SolverDefinitionGridAttribute* gridAttribute() const;
	virtual QVariant variantValue() = 0;

	virtual void applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) = 0;

protected:
	virtual void getValueFromInnerWidget() = 0;

private:
	virtual void setupWidget() = 0;

	SolverDefinitionGridAttribute* m_gridAttribute;
};

#endif // GRIDATTRIBUTEVARIATIONEDITWIDGET_H
