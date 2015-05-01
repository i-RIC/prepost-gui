#ifndef GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H
#define GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H

#include "../base/gridrelatedconditioneditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;
class SolverDefinitionGridRelatedComplexCondition;

class GridRelatedComplexConditionEditWidget : public GridRelatedConditionEditWidgetT<int>
{
public:
	GridRelatedComplexConditionEditWidget(QWidget* parent, SolverDefinitionGridRelatedComplexCondition* cond);
	~GridRelatedComplexConditionEditWidget();
	void setEnumerations(const QMap<int, QString>& enums) {
		m_enumerations = enums;
	}
	void setDefaultIndex(int idx) {
		m_defaultIndex = idx;
	}
	const QMap<int, QString>& enumerations() const {return m_enumerations;}
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	void setupWidget();
	void getValueFromInnerWidget();
private:
	QMap<int, QString> m_enumerations;
	int m_defaultIndex;
	QList<int> m_values;
	QComboBox* m_widget;
};

#endif // GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H
