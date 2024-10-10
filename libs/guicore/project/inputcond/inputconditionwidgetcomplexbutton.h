#ifndef INPUTCONDITIONWIDGETCOMPLEXBUTTON_H
#define INPUTCONDITIONWIDGETCOMPLEXBUTTON_H

#include "../../guicore_global.h"

#include "inputconditionwidget.h"

class InputConditionWidgetComplexButton : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetComplexButton();

	void addTooltip(const QString& tooltip) override;

signals:
	void clicked();
};

#endif // INPUTCONDITIONWIDGETCOMPLEXBUTTON_H
