#ifndef SCALARSTOCOLORSEDITWIDGETCONTAINER_H
#define SCALARSTOCOLORSEDITWIDGETCONTAINER_H

#include "../guicore_global.h"
#include "scalarstocolorseditwidget.h"
#include <QWidget>
#include <QVBoxLayout>

class GUICOREDLL_EXPORT ScalarsToColorsEditWidgetContainer : public QWidget
{
public:
	ScalarsToColorsEditWidgetContainer(QWidget* parent);
	void setWidget(ScalarsToColorsEditWidget* w);
	ScalarsToColorsEditWidget* widget() const {return m_widget;}
	QSize sizeHint() const;
	void save();

private:
	ScalarsToColorsEditWidget* m_widget;
};

#endif // SCALARSTOCOLORSEDITWIDGETCONTAINER_H
