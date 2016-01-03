#ifndef GRIDATTRIBUTEREALOPTIONEDITWIDGET_H
#define GRIDATTRIBUTEREALOPTIONEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;

class GridAttributeRealOptionEditWidget : public GridAttributeEditWidgetT<double>
{
public:
	GridAttributeRealOptionEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond);
	~GridAttributeRealOptionEditWidget();

	const QMap<double, QString>& enumerations() const;
	void setEnumerations(const QMap<double, QString>& enums);

private:
	void setupWidget() override;
	void getValueFromInnerWidget() const override;

	QWidget* editWidget() const override;

	QMap<double, QString> m_enumerations;
	QList<double> m_values;
	QComboBox* m_widget;
};

#endif // GRIDATTRIBUTEREALOPTIONEDITWIDGET_H
