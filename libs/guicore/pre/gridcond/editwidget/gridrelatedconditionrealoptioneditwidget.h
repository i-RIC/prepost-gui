#ifndef GRIDRELATEDCONDITIONREALOPTIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALOPTIONEDITWIDGET_H

#include "../base/gridrelatedconditioneditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;

class GridRelatedConditionRealOptionEditWidget : public GridRelatedConditionEditWidgetT<double>
{
public:
	GridRelatedConditionRealOptionEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<double>* cond);
	~GridRelatedConditionRealOptionEditWidget();
	void setEnumerations(const QMap<double, QString>& enums) {
		m_enumerations = enums;
	}
	const QMap<double, QString>& enumerations() const {return m_enumerations;}
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	void setupWidget();
	void getValueFromInnerWidget();
private:
	QMap<double, QString> m_enumerations;
	QList<double> m_values;
	QComboBox* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALOPTIONEDITWIDGET_H
