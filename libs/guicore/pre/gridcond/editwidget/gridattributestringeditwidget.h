#ifndef GRIDATTRIBUTESTRINGEDITWIDGET_H
#define GRIDATTRIBUTESTRINGEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"

#include <vtkStringArray.h>

#include <string>

class QLineEdit;

class GridAttributeStringEditWidget : public GridAttributeEditWidgetT<std::string, vtkStringArray>
{
public:
	GridAttributeStringEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<std::string>* const);
	~GridAttributeStringEditWidget();

private:
	void setupWidget() override;
	void getValueFromInnerWidget() const override;

	QWidget* editWidget() const override;

	QLineEdit* m_widget;
};

#endif // GRIDATTRIBUTESTRINGEDITWIDGET_H
