#ifndef GRIDATTRIBUTEINTEGEROPTIONEDITWIDGET_H
#define GRIDATTRIBUTEINTEGEROPTIONEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;

class GridAttributeIntegerOptionEditWidget : public GridAttributeEditWidgetT<int, vtkIntArray>
{

public:
	GridAttributeIntegerOptionEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond);
	~GridAttributeIntegerOptionEditWidget();

	const QMap<int, QString>& enumerations() const;
	void setEnumerations(const QMap<int, QString>& enums);

private:
	void setupWidget() override;
	void getValueFromInnerWidget() const override;

	QWidget* editWidget() const override;

	QMap<int, QString> m_enumerations;
	QList<int> m_values;
	QComboBox* m_widget;
};

#endif // GRIDATTRIBUTEINTEGEROPTIONEDITWIDGET_H
