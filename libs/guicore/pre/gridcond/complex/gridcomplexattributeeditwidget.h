#ifndef GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H
#define GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;
class SolverDefinitionGridComplexAttribute;

class GridComplexAttributeEditWidget : public GridAttributeEditWidgetT<int>
{

public:
	GridComplexAttributeEditWidget(QWidget* parent, SolverDefinitionGridComplexAttribute* cond);
	~GridComplexAttributeEditWidget();

	const QMap<int, QString>& enumerations() const;
	void setEnumerations(const QMap<int, QString>& enums);

	void setDefaultIndex(int idx);

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;

private:
	QMap<int, QString> m_enumerations;
	int m_defaultIndex;
	QList<int> m_values;
	QComboBox* m_widget;
};

#endif // GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H
