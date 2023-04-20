#include "boolcontainerwidget.h"
#include "ui_boolcontainerwidget.h"

#include <misc/boolcontainer.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

BoolContainerWidget::BoolContainerWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BoolContainerWidget)
{
	ui->setupUi(this);

	connect(ui->checkBox, &QCheckBox::toggled, this, &BoolContainerWidget::toggled);
}

BoolContainerWidget::~BoolContainerWidget()
{
	delete ui;
}

void BoolContainerWidget::setText(const QString& text)
{
	ui->checkBox->setText(text);
}

void BoolContainerWidget::setContainer(BoolContainer* container)
{
	m_container = container;
	ui->checkBox->setChecked(container->value());
}

QUndoCommand* BoolContainerWidget::createModifyCommand(bool apply)
{
	BoolContainer newValue("value");
	newValue.setValue(ui->checkBox->isChecked());

	return new ValueModifyCommmand<BoolContainer>(iRIC::generateCommandId("ModifyBool"), apply, newValue, m_container);
}

void BoolContainerWidget::apply()
{
	if (m_container == nullptr) {return;}

	ui->checkBox->setChecked(m_container->value());
}
