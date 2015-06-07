#ifndef GRIDRELATEDCONDITIONDIMENSIONCOMBOBOXSELECTWIDGET_H
#define GRIDRELATEDCONDITIONDIMENSIONCOMBOBOXSELECTWIDGET_H

#include "../base/gridattributedimensionselectwidget.h"

namespace Ui
{
	class GridAttributeDimensionComboboxSelectWidget;
}

class GridAttributeDimensionComboboxSelectWidget : public GridAttributeDimensionSelectWidget
{
	Q_OBJECT

public:
	explicit GridAttributeDimensionComboboxSelectWidget(GridAttributeDimensionContainer* container, QWidget* parent);
	~GridAttributeDimensionComboboxSelectWidget();

protected:
	void doApplyValues() override;
	void doSetCurrentIndex(int newIndex) override;

private:
	Ui::GridAttributeDimensionComboboxSelectWidget* ui;
};

#endif // GRIDRELATEDCONDITIONDIMENSIONCOMBOBOXSELECTWIDGET_H
