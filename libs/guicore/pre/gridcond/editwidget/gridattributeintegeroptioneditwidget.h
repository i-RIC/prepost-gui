#ifndef GRIDRELATEDCONDITIONINTEGEROPTIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONINTEGEROPTIONEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;

class GridAttributeIntegerOptionEditWidget : public GridAttributeEditWidgetT<int>
{

public:
	GridAttributeIntegerOptionEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond);
	~GridAttributeIntegerOptionEditWidget();
	void setEnumerations(const QMap<int, QString>& enums) {
		m_enumerations = enums;
	}
	const QMap<int, QString>& enumerations() const {return m_enumerations;}
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;

private:
	QMap<int, QString> m_enumerations;
	QList<int> m_values;
	QComboBox* m_widget;
};

#endif // GRIDRELATEDCONDITIONINTEGEROPTIONEDITWIDGET_H
