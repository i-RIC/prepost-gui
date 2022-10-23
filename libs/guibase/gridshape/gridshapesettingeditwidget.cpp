#include "gridshapesettingcontainer.h"
#include "gridshapesettingeditwidget.h"
#include "ui_gridshapesettingeditwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GridShapeSettingEditWidget::GridShapeSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GridShapeSettingEditWidget)
{
	ui->setupUi(this);

	connect<void(QRadioButton::*)(bool)>(ui->outlineRadioButton, &QRadioButton::clicked, this, &GridShapeSettingEditWidget::handleOutlineOnlyClick);
}

GridShapeSettingEditWidget::~GridShapeSettingEditWidget()
{
	delete ui;
}

void GridShapeSettingEditWidget::hideShape()
{
	ui->allRadioButton->setChecked(true);
	ui->allRadioButton->hide();
	ui->outlineRadioButton->hide();
	adjustSize();
}

void GridShapeSettingEditWidget::hideIndex()
{
	ui->indicesGroupBox->hide();
}

GridShapeSettingContainer GridShapeSettingEditWidget::setting() const
{
	GridShapeSettingContainer ret;

	ret.color = ui->gridColorEditWidget->color();
	ret.indexVisible = ui->indexVisibleCheckBox->isChecked();
	ret.indexTextSetting = ui->indexTextSettingWidget->setting();
	if (ui->outlineRadioButton->isChecked()) {
		ret.shape = GridShapeSettingContainer::Shape::Outline;
	} else {
		ret.shape = GridShapeSettingContainer::Shape::Wireframe;
	}

	return ret;
}

void GridShapeSettingEditWidget::setSetting(GridShapeSettingContainer* setting)
{
	setSetting(*setting);

	m_setting = setting;
}

void GridShapeSettingEditWidget::setSetting(const GridShapeSettingContainer& s)
{
	ui->gridColorEditWidget->setColor(s.color);
	ui->indexVisibleCheckBox->setChecked(s.indexVisible);
	ui->indexTextSettingWidget->setSetting(s.indexTextSetting);
	if (s.shape == GridShapeSettingContainer::Shape::Outline) {
		ui->outlineRadioButton->setChecked(true);
	} else {
		ui->allRadioButton->setChecked(true);
	}
}

QUndoCommand* GridShapeSettingEditWidget::createModifyCommand() const
{
	return new ValueModifyCommmand<GridShapeSettingContainer>(
				iRIC::generateCommandId("GridShapeSettingEditWidget::Modify"),
				true, setting(), m_setting);
}

void GridShapeSettingEditWidget::handleOutlineOnlyClick()
{
	ui->indexVisibleCheckBox->setChecked(false);
}
