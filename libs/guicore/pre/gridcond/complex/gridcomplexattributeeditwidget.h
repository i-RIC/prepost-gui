#ifndef GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H
#define GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H

#include "../../../guicore_global.h"

#include "../base/gridattributeeditwidgett.h"
#include <QMap>
#include <QList>

class QComboBox;
class SolverDefinitionGridComplexAttribute;

class GUICOREDLL_EXPORT GridComplexAttributeEditWidget : public GridAttributeEditWidgetT<int>
{
public:
	GridComplexAttributeEditWidget(QWidget* parent, SolverDefinitionGridComplexAttribute* cond);
	~GridComplexAttributeEditWidget();

	const QMap<int, QString>& enumerations() const;
	void setEnumerations(const QMap<int, QString>& enums);

	void setDefaultIndex(int idx);

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

private:
	void setupWidget() override;
	void getValueFromInnerWidget() const override;

	QMap<int, QString> m_enumerations;
	int m_defaultIndex;
	QList<int> m_values;
	QComboBox* m_widget;
};

#endif // GRIDRELATEDCOMPLEXCONDITIONINTEGEROPTIONEDITWIDGET_H
