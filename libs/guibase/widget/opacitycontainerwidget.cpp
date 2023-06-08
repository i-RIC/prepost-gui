#include "opacitycontainerwidget.h"
#include <misc/opacitycontainer.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

OpacityContainerWidget::OpacityContainerWidget(QWidget* parent) :
	TransparencyWidget {parent},
	m_container {nullptr}
{}

void OpacityContainerWidget::setContainer(OpacityContainer* container)
{
	m_container = container;
	setOpacityPercent(container->value());

	connect(container, &OpacityContainer::updated, this, &OpacityContainerWidget::apply);
}

QUndoCommand* OpacityContainerWidget::createModifyCommand(bool apply)
{
	OpacityContainer newValue;
	newValue.setValue(opacityPercent());

	return new ValueModifyCommmand<OpacityContainer>(iRIC::generateCommandId("ModifyOpacity"), apply, newValue, m_container);
}

void OpacityContainerWidget::apply()
{
	if (m_container == nullptr) {return;}

	blockSignals(true);

	setOpacityPercent(m_container->value());

	blockSignals(false);
}
