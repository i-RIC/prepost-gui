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

	ScalarsToColorsEditWidget* widget() const;
	void setWidget(ScalarsToColorsEditWidget* w);

	void save();

	QSize sizeHint() const override;

private:
	ScalarsToColorsEditWidget* m_widget;
};

#endif // SCALARSTOCOLORSEDITWIDGETCONTAINER_H
