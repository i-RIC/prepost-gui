#ifndef GRIDRELATEDCONDITIONDIMENSIONCOMBOBOXSELECTWIDGET_H
#define GRIDRELATEDCONDITIONDIMENSIONCOMBOBOXSELECTWIDGET_H

#include "../base/gridrelatedconditiondimensionselectwidget.h"

namespace Ui
{
	class GridRelatedConditionDimensionComboboxSelectWidget;
}

class GridRelatedConditionDimensionComboboxSelectWidget : public GridRelatedConditionDimensionSelectWidget
{
	Q_OBJECT

public:
	explicit GridRelatedConditionDimensionComboboxSelectWidget(GridRelatedConditionDimensionContainer* container, QWidget* parent);
	~GridRelatedConditionDimensionComboboxSelectWidget();

protected:
	void doApplyValues();
	void doSetCurrentIndex(int newIndex);

private:
	Ui::GridRelatedConditionDimensionComboboxSelectWidget* ui;
};

#endif // GRIDRELATEDCONDITIONDIMENSIONCOMBOBOXSELECTWIDGET_H
