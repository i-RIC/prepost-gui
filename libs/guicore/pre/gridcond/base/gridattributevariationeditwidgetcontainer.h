#ifndef GRIDATTRIBUTEVARIATIONEDITWIDGETCONTAINER_H
#define GRIDATTRIBUTEVARIATIONEDITWIDGETCONTAINER_H

#include <QWidget>

class GridAttributeVariationEditWidget;

class GridAttributeVariationEditWidgetContainer : public QWidget
{
public:
	GridAttributeVariationEditWidgetContainer(QWidget* parent);

	GridAttributeVariationEditWidget* widget() const;
	void setWidget(GridAttributeVariationEditWidget* w);

	QSize sizeHint() const override;

private:
	GridAttributeVariationEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEEDITWIDGETCONTAINER_H
