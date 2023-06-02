#ifndef GRIDCOMPLEXCONDITIONGROUPREALEDITWIDGET_H
#define GRIDCOMPLEXCONDITIONGROUPREALEDITWIDGET_H

#include "../../guicore_global.h"
#include "../gridcond/base/gridattributevariationeditwidget.h"

#include <QWidget>

#include <vector>

namespace Ui {
class GridComplexConditionGroupRealEditWidget;
}

class GridComplexConditionGroup;
class QCheckBox;

class GUICOREDLL_EXPORT GridComplexConditionGroupRealEditWidget : public GridAttributeVariationEditWidget
{
	Q_OBJECT

public:
	struct SubItem {
		enum Type {Real, Functional};

		Type type;
		QString name;
		QString subName;
		QString caption;
	};

	GridComplexConditionGroupRealEditWidget(QWidget *parent, SolverDefinitionGridAttribute* att);
	~GridComplexConditionGroupRealEditWidget() override;

	void setMode(Mode mode) override;
	void applyVariation(GridAttributeContainer* container, const std::vector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) override;
	void applyVariation(const std::vector<vtkIdType>& indices, const std::vector<GridComplexConditionGroup*>& groups);

private:
	void getValueFromInnerWidget() override;
	void setupWidget();
	void setupSubItems();

	std::vector<SubItem> m_subItems;
	std::vector<QCheckBox*> m_checkBoxes;
	Ui::GridComplexConditionGroupRealEditWidget *ui;
};

#endif // GRIDCOMPLEXCONDITIONGROUPREALEDITWIDGET_H
