#include "inputconditionwidgettooltip.h"

#include <QPixmap>

InputConditionWidgetTooltip::InputConditionWidgetTooltip(const QString& tooltip, QWidget* parent) :
	QLabel(parent)
{
	setPixmap(QPixmap(":/libs/guibase/images/iconQuestion.png"));
	setToolTip(tooltip);
}
