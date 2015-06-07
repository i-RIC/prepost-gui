#ifndef GRIDRELATEDCONDITIONREALOPTIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALOPTIONEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;

class GridAttributeRealOptionEditWidget : public GridAttributeEditWidgetT<double>
{

public:
	GridAttributeRealOptionEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond);
	~GridAttributeRealOptionEditWidget();
	void setEnumerations(const QMap<double, QString>& enums) {
		m_enumerations = enums;
	}
	const QMap<double, QString>& enumerations() const {return m_enumerations;}
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;

private:
	QMap<double, QString> m_enumerations;
	QList<double> m_values;
	QComboBox* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALOPTIONEDITWIDGET_H
