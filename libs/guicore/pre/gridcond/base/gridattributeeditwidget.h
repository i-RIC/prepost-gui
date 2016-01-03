#ifndef GRIDATTRIBUTEEDITWIDGET_H
#define GRIDATTRIBUTEEDITWIDGET_H

#include "../../../guicore_global.h"
#include <QWidget>
#include <vtkIdList.h>
class SolverDefinitionGridAttribute;
class GridAttributeContainer;
class vtkDataSetAttributes;
class PreProcessorGridDataItemInterface;

class GUICOREDLL_EXPORT GridAttributeEditWidget : public QWidget
{
	Q_OBJECT

public:
	GridAttributeEditWidget(QWidget* parent, SolverDefinitionGridAttribute* att);

	SolverDefinitionGridAttribute* gridAttribute() const;

	void clearValue();
	bool valueSelected() const;

	virtual QVariant variantValue() const = 0;
	virtual void setVariantValue(const QVariant& v) = 0;

	virtual void scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices) = 0;
	virtual void applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) = 0;

protected:
	virtual void getValueFromInnerWidget() const = 0;
	virtual void setupWidget() = 0;

protected:
	bool m_valueCleared {false};
	mutable bool m_valueSelected {false};

private:
	SolverDefinitionGridAttribute* m_gridAttribute;
};

#endif // GRIDATTRIBUTEEDITDIALOG_H
