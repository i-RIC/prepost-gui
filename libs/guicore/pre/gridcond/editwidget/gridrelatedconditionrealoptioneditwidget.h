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
