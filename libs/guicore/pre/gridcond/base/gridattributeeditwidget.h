#ifndef GRIDATTRIBUTEEDITWIDGET_H
#define GRIDATTRIBUTEEDITWIDGET_H

#include "../../../guicore_global.h"
#include <QWidget>
#include <vtkIdList.h>
class SolverDefinitionGridAttribute;
class GridAttributeContainer;
class vtkDataSetAttributes;
class PreProcessorGridDataItemI;

class GUICOREDLL_EXPORT GridAttributeEditWidget : public QWidget
{
	Q_OBJECT

public:
	GridAttributeEditWidget(QWidget* parent, SolverDefinitionGridAttribute* att);
	virtual ~GridAttributeEditWidget();

	SolverDefinitionGridAttribute* gridAttribute() const;

	void clearValue();
	bool isValueSelected() const;

	virtual QVariant variantValue() const = 0;
	virtual void setVariantValue(const QVariant& v) = 0;

	virtual void scanAndSetDefault(GridAttributeContainer* container, const std::vector<vtkIdType>& indices) = 0;
	virtual void applyValue(GridAttributeContainer* container, const std::vector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem) = 0;

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	virtual void getValueFromInnerWidget() const = 0;
	virtual void setupWidget() = 0;

	bool isValueCleared() const;
	void setValueCleared(bool cleared) const;

	void setValueSelected(bool selected) const;

private:
	virtual QWidget* editWidget() const = 0;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/gridattributeeditwidget_impl.h"
#endif // _DEBUG

#endif // GRIDATTRIBUTEEDITDIALOG_H
