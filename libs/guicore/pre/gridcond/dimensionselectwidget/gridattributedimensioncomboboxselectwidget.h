#ifndef GRIDATTRIBUTEDIMENSIONCOMBOBOXSELECTWIDGET_H
#define GRIDATTRIBUTEDIMENSIONCOMBOBOXSELECTWIDGET_H

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

#endif // GRIDATTRIBUTEDIMENSIONCOMBOBOXSELECTWIDGET_H
