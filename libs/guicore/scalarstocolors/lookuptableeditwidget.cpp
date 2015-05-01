#include "lookuptableeditwidget.h"
#include "ui_lookuptableeditwidget.h"
#include "lookuptablecontainer.h"

LookupTableEditWidget::LookupTableEditWidget(QWidget* /*parent*/)
	: ui(new Ui::LookupTableEditWidget)
{
	ui->setupUi(this);
	connect(ui->autoCheckBox, SIGNAL(clicked(bool)), this, SLOT(autoRangeUpdate(bool)));
}

LookupTableEditWidget::~LookupTableEditWidget()
{
	delete ui;
}

void LookupTableEditWidget::setupWidget()
{
	LookupTableContainer* cont = dynamic_cast<LookupTableContainer*>(m_container);
	ui->colorMapSetting->setColorMap(cont->colorMap());
	ui->colorMapSetting->setCustomSetting(cont->customSetting());
	ui->autoCheckBox->setChecked(cont->autoRange());
	if (cont->autoRange()) {
		ui->maxEdit->setDisabled(true);
		ui->maxEdit->setValue(cont->autoMax());
		ui->minEdit->setDisabled(true);
		ui->minEdit->setValue(cont->autoMin());
		ui->fillLowerCheckBox->setDisabled(true);
		ui->fillLowerCheckBox->setChecked(true);
		ui->fillUpperCheckBox->setDisabled(true);
		ui->fillUpperCheckBox->setChecked(true);
	} else {
		ui->maxEdit->setEnabled(true);
		ui->maxEdit->setValue(cont->manualMax());
		ui->minEdit->setEnabled(true);
		ui->minEdit->setValue(cont->manualMin());
		ui->fillLowerCheckBox->setEnabled(true);
		ui->fillUpperCheckBox->setEnabled(true);
	}
}

void LookupTableEditWidget::autoRangeUpdate(bool a)
{
	ui->maxEdit->setDisabled(a);
	ui->minEdit->setDisabled(a);
	ui->fillLowerCheckBox->setDisabled(a);
	ui->fillUpperCheckBox->setDisabled(a);
	if (a) {
		ui->fillLowerCheckBox->setChecked(true);
		ui->fillUpperCheckBox->setChecked(true);
	}
}

void LookupTableEditWidget::save()
{
	LookupTableContainer* cont = dynamic_cast<LookupTableContainer*>(m_container);
	cont->setColorMap(ui->colorMapSetting->colorMap());
	cont->setCustomSetting(ui->colorMapSetting->customSetting());
	cont->setAutoRange(ui->autoCheckBox->isChecked());
	if (! cont->autoRange()) {
		cont->setManualMax(ui->maxEdit->value());
		cont->setManualMin(ui->minEdit->value());
	}
	cont->update();
}

void LookupTableEditWidget::setFillUpper(bool fill)
{
	LookupTableContainer* c = dynamic_cast<LookupTableContainer*>(m_container);
	ui->fillUpperCheckBox->setChecked(c->autoRange() || fill);
}

void LookupTableEditWidget::setFillLower(bool fill)
{
	LookupTableContainer* c = dynamic_cast<LookupTableContainer*>(m_container);
	ui->fillLowerCheckBox->setChecked(c->autoRange() || fill);
}

bool LookupTableEditWidget::fillUpper()
{
	LookupTableContainer* c = dynamic_cast<LookupTableContainer*>(m_container);
	return c->autoRange() || ui->fillUpperCheckBox->isChecked();
}

bool LookupTableEditWidget::fillLower()
{
	LookupTableContainer* c = dynamic_cast<LookupTableContainer*>(m_container);
	return c->autoRange() || ui->fillLowerCheckBox->isChecked();
}

bool LookupTableEditWidget::autoRange()
{
	return ui->autoCheckBox->isChecked();
}

double LookupTableEditWidget::upperLimit()
{
	return ui->maxEdit->value();
}

double LookupTableEditWidget::lowerLimit()
{
	return ui->minEdit->value();
}

void LookupTableEditWidget::hideFillSettings()
{
	ui->fillLowerCheckBox->hide();
	ui->fillUpperCheckBox->hide();
	adjustSize();
}

void LookupTableEditWidget::setDivisionNumber(int div)
{
	ui->divisionNumberSpinBox->setValue(div);
}

int LookupTableEditWidget::divisionNumber()
{
	return ui->divisionNumberSpinBox->value();
}

void LookupTableEditWidget::hideDivisionNumber()
{
	ui->divisionNumberLabel->hide();
	ui->divisionNumberSpinBox->hide();
	adjustSize();
}

void LookupTableEditWidget::showDivisionNumber()
{
	ui->divisionNumberLabel->show();
	ui->divisionNumberSpinBox->show();
	adjustSize();
}

void LookupTableEditWidget::setContourSetting(ContourSettingWidget::Contour contour)
{
	switch (contour) {
	case ContourSettingWidget::Isolines:
	case ContourSettingWidget::ContourFigure:
		ui->divisionNumberSpinBox->setEnabled(true);
		break;
	case ContourSettingWidget::Points:
	case ContourSettingWidget::ColorFringe:
		ui->divisionNumberSpinBox->setDisabled(true);
		break;
	}
}
