#ifndef INPUTCONDITIONWIDGETTOOLTIP_H
#define INPUTCONDITIONWIDGETTOOLTIP_H

#include <QLabel>

class InputConditionWidgetTooltip : public QLabel
{
public:
	InputConditionWidgetTooltip(const QString& tooltip, QWidget* parent = 0);
};

#endif // INPUTCONDITIONWIDGETWITHTOOLTIP_H
