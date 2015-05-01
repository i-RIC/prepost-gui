#ifndef GRIDRELATEDCONDITIONINTEGEROPTIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONINTEGEROPTIONEDITWIDGET_H

#include "../base/gridrelatedconditioneditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;

class GridRelatedConditionIntegerOptionEditWidget : public GridRelatedConditionEditWidgetT<int>
{
public:
	GridRelatedConditionIntegerOptionEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<int>* cond);
	~GridRelatedConditionIntegerOptionEditWidget();
	void setEnumerations(const QMap<int, QString>& enums) {
		m_enumerations = enums;
	}
	const QMap<int, QString>& enumerations() const {return m_enumerations;}
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	void setupWidget();
	void getValueFromInnerWidget();
private:
	QMap<int, QString> m_enumerations;
	QList<int> m_values;
	QComboBox* m_widget;
};

#endif // GRIDRELATEDCONDITIONINTEGEROPTIONEDITWIDGET_H
