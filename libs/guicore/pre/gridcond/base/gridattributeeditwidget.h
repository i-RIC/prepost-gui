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
	bool isValueSelected() const;

	virtual QVariant variantValue() const = 0;
	virtual void setVariantValue(const QVariant& v) = 0;

	virtual void scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices) = 0;
	virtual void applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) = 0;

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	virtual void getValueFromInnerWidget() const = 0;
	virtual void setupWidget() = 0;

	bool isValueCleared() const;
	void setValueCleared(bool cleared);

	void setValueSelected(bool selected) const;

private:
	virtual QWidget* editWidget() const = 0;

	mutable bool m_isValueSelected {false};
	bool m_isValueCleared {false};

	SolverDefinitionGridAttribute* m_gridAttribute;
};

#endif // GRIDATTRIBUTEEDITDIALOG_H
