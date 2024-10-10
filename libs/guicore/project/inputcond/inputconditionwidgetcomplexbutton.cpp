#include "inputconditionwidgetcomplexbutton.h"
#include "private/inputconditionwidgettooltip.h"

#include <QPushButton>
#include <QHBoxLayout>

InputConditionWidgetComplexButton::InputConditionWidgetComplexButton() :
	InputConditionWidget {}
{
	auto button = new QPushButton(tr("Edit..."), this);
	connect(button, &QPushButton::clicked, this, &InputConditionWidgetComplexButton::clicked);

	auto layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(button);
	setLayout(layout);
}

void InputConditionWidgetComplexButton::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(0, tt);
}
