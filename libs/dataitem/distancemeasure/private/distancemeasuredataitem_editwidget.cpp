#include "distancemeasuredataitem_editwidget.h"
#include "distancemeasuredataitem_impl.h"
#include "ui_distancemeasuredataitem_editwidget.h"

#include <guicore/datamodel/graphicswindowdataitemstandarditemedittextcommand.h>
#include <misc/mathsupport.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

DistanceMeasureDataItem::EditWidget::EditWidget(DistanceMeasureDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::DistanceMeasureDataItem_EditWidget)
{
	ui->setupUi(this);
	ui->labelFontSettingWidget->hideColor();

	connect(ui->startPointXEdit, &RealNumberEditWidget::valueChanged, this, &EditWidget::updateAutoLabel);
	connect(ui->startPointYEdit, &RealNumberEditWidget::valueChanged, this, &EditWidget::updateAutoLabel);
	connect(ui->endPointXEdit, &RealNumberEditWidget::valueChanged, this, &EditWidget::updateAutoLabel);
	connect(ui->endPointYEdit, &RealNumberEditWidget::valueChanged, this, &EditWidget::updateAutoLabel);

	setName(item->m_standardItem->text());
	setSetting(item->impl->m_setting);
}

DistanceMeasureDataItem::EditWidget::~EditWidget()
{
	delete ui;
}

QString DistanceMeasureDataItem::EditWidget::name() const
{
	return ui->nameLineEdit->text();
}

void DistanceMeasureDataItem::EditWidget::setName(const QString& name)
{
	ui->nameLineEdit->setText(name.trimmed());
}

DistanceMeasureSetting DistanceMeasureDataItem::EditWidget::setting() const
{
	DistanceMeasureSetting ret = m_setting;

	ret.point1 = QPointF(ui->startPointXEdit->value(), ui->startPointYEdit->value());
	ret.point2 = QPointF(ui->endPointXEdit->value(), ui->endPointYEdit->value());

	ret.showLabel = ui->showLabelCheckBox->isChecked();

	if (ui->labelAutoRadioButton->isChecked()) {
		ret.labelMode = DistanceMeasureSetting::Auto;
	} else if (ui->labelCustomRadioButton->isChecked()) {
		ret.labelMode = DistanceMeasureSetting::Custom;
	}

	if (ui->lpCenterTop->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpTopCenter;
	} else if (ui->lpCenterBottom->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpBottomCenter;
	} else if (ui->lpMiddleLeft->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpMiddleLeft;
	} else if (ui->lpMiddleRight->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpMiddleRight;
	}

	ret.labelFontSetting = ui->labelFontSettingWidget->setting();
	ret.customLabel = ui->customLabelLineEdit->text();

	ret.showMarkers = ui->showMarkersCheckBox->isChecked();
	ret.markerSize = ui->markerSizeSpinBox->value();

	ret.color = ui->colorWidget->color();
	ret.labelFontSetting.fontColor = ret.color;

	return ret;
}

void DistanceMeasureDataItem::EditWidget::setSetting(const DistanceMeasureSetting& setting)
{
	m_setting = setting;

	ui->startPointXEdit->setValue(setting.point1.value().x());
	ui->startPointYEdit->setValue(setting.point1.value().y());
	ui->endPointXEdit->setValue(setting.point2.value().x());
	ui->endPointYEdit->setValue(setting.point2.value().y());

	ui->showLabelCheckBox->setChecked(setting.showLabel);

	if (setting.labelMode == DistanceMeasureSetting::Auto) {
		ui->labelAutoRadioButton->setChecked(true);
	} else {
		ui->labelCustomRadioButton->setChecked(true);
	}

	switch (setting.labelPosition.value()) {
	case vtkLabel2DActor::lpTopCenter:
		ui->lpCenterTop->setChecked(true);
		break;
	case vtkLabel2DActor::lpBottomCenter:
		ui->lpCenterBottom->setChecked(true);
		break;
	case vtkLabel2DActor::lpMiddleLeft:
		ui->lpMiddleLeft->setChecked(true);
		break;
	case vtkLabel2DActor::lpMiddleRight:
		ui->lpMiddleRight->setChecked(true);
		break;
	default:
		ui->lpCenterTop->setChecked(true);
	}

	ui->labelFontSettingWidget->setSetting(setting.labelFontSetting);
	ui->customLabelLineEdit->setText(setting.customLabel);

	ui->showMarkersCheckBox->setChecked(setting.showMarkers);
	ui->markerSizeSpinBox->setValue(setting.markerSize);

	ui->colorWidget->setColor(setting.color);
	updateAutoLabel();
}

QUndoCommand* DistanceMeasureDataItem::EditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("DistanceMeasureDataItem::EditWidget"), apply);
	command->addCommand(new GraphicsWindowDataItemStandardItemEditTextCommand(name(), m_item));
	command->addCommand(new ValueModifyCommmand<DistanceMeasureSetting>(iRIC::generateCommandId("Setting"), apply,  setting(), &m_item->impl->m_setting));
	return command;
}

void DistanceMeasureDataItem::EditWidget::setStartPoint(const QPointF& point)
{
	ui->startPointXEdit->setValue(point.x());
	ui->startPointYEdit->setValue(point.y());

	updateAutoLabel();
}

void DistanceMeasureDataItem::EditWidget::setEndPoint(const QPointF& point)
{
	ui->endPointXEdit->setValue(point.x());
	ui->endPointYEdit->setValue(point.y());

	updateAutoLabel();
}

void DistanceMeasureDataItem::EditWidget::updateAutoLabel()
{
	ui->autoLabelLabel->setText(autoLabel());
}

QString DistanceMeasureDataItem::EditWidget::autoLabel() const
{
	QPointF v1(ui->startPointXEdit->value(), ui->startPointYEdit->value());
	QPointF v2(ui->endPointXEdit->value(), ui->endPointYEdit->value());

	auto len = iRIC::length(v2 - v1);
	return QString::number(len);
}
