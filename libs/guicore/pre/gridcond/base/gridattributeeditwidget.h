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
	GridAttributeEditWidget(QWidget* parent, SolverDefinitionGridAttribute* att) :
		QWidget(parent),
		m_gridAttribute {att}
	{}
	void clearValue() {
		m_valueCleared = true;
		m_valueSelected = false;
		setupWidget();
	}
	bool valueSelected() {
		getValueFromInnerWidget();
		return m_valueSelected;
	}
	SolverDefinitionGridAttribute* gridAttribute() {return m_gridAttribute;}
	virtual void setVariantValue(const QVariant& v) = 0;
	virtual void scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices) = 0;
	virtual void applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) = 0;
	virtual QVariant variantValue() = 0;

protected:
	virtual void setupWidget() = 0;
	virtual void getValueFromInnerWidget() = 0;
	bool m_valueCleared {false};
	bool m_valueSelected {false};

	SolverDefinitionGridAttribute* m_gridAttribute;
};

#endif // GRIDATTRIBUTEEDITDIALOG_H
