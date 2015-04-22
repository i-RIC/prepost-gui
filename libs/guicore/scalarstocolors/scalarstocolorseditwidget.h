#ifndef SCALARSTOCOLORSEDITWIDGET_H
#define SCALARSTOCOLORSEDITWIDGET_H

#include "../guicore_global.h"
#include <QWidget>

class ScalarsToColorsContainer;

class GUICOREDLL_EXPORT ScalarsToColorsEditWidget : public QWidget
{
public:
	ScalarsToColorsEditWidget(QWidget *parent = nullptr);
	ScalarsToColorsContainer* container() const {
		return m_container;
	}
	void setContainer(ScalarsToColorsContainer* c){
		m_container = c;
		setupWidget();
	}
	virtual void save() = 0;

protected:
	virtual void setupWidget() = 0;
	ScalarsToColorsContainer* m_container;
};

#endif // SCALARSTOCOLORSEDITWIDGET_H
