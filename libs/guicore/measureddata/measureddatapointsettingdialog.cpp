#include "ui_measureddatapointsettingdialog.h"

#include "../project/measureddata.h"
#include "measureddatapointsettingdialog.h"

#include <guibase/scalarbardialog.h>

#include <QMessageBox>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

MeasuredDataPointSettingDialog::MeasuredDataPointSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::MeasuredDataPointSettingDialog)
{
	ui->setupUi(this);
	connect(ui->measuredValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showColorBarDialog()));
	connect(ui->contourWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ui->colormapWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));
}

MeasuredDataPointSettingDialog::~MeasuredDataPointSettingDialog()
{
	delete ui;
}

void MeasuredDataPointSettingDialog::setData(MeasuredData* md)
{
	ui->measuredValueComboBox->clear();
	m_measuredValues.clear();
	ui->measuredValueComboBox->blockSignals(true);

	m_measuredValues = md->pointNames();
	for (int i = 0; i < m_measuredValues.count(); ++i) {
		ui->measuredValueComboBox->addItem(m_measuredValues.at(i));
	}
	ui->measuredValueComboBox->blockSignals(false);
}

void MeasuredDataPointSettingDialog::setCurrentMeasuredValue(QString sol)
{
	int index = m_measuredValues.indexOf(sol);
	if (index == -1) {
		// not set yet. select the first one.
		index = 0;
	}
	ui->measuredValueComboBox->setCurrentIndex(index);
	measuredValueChanged(index);
}

void MeasuredDataPointSettingDialog::setContour(ContourSettingWidget::Contour c)
{
	ui->contourWidget->setContour(c);
}

void MeasuredDataPointSettingDialog::setNumberOfDivision(int div)
{
	ui->colormapWidget->setDivisionNumber(div);
}

QString MeasuredDataPointSettingDialog::currentMeasuredValue()
{
	return m_measuredValues.at(ui->measuredValueComboBox->currentIndex());
}

ContourSettingWidget::Contour MeasuredDataPointSettingDialog::contour()
{
	return ui->contourWidget->contour();
}

int MeasuredDataPointSettingDialog::numberOfDivision()
{
	return ui->colormapWidget->divisionNumber();
}

const LookupTableContainer& MeasuredDataPointSettingDialog::lookupTable()
{
	return m_lookupTable;
}

void MeasuredDataPointSettingDialog::measuredValueChanged(int index)
{
	QString sol = m_measuredValues.at(index);
	LookupTableContainer* c = m_lookupTables.value(sol, 0);
	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void MeasuredDataPointSettingDialog::accept()
{
	if ((! ui->colormapWidget->autoRange()) &&
			(ui->colormapWidget->upperLimit() < ui->colormapWidget->lowerLimit())) {
		QMessageBox::critical(this, tr("Error"), tr("For value range, max value is smaller than min value."));
		return;
	}

	ui->colormapWidget->save();
	QDialog::accept();
}

void MeasuredDataPointSettingDialog::setFillUpper(bool fill)
{
	ui->colormapWidget->setFillUpper(fill);
}

void MeasuredDataPointSettingDialog::setFillLower(bool fill)
{
	ui->colormapWidget->setFillLower(fill);
}

bool MeasuredDataPointSettingDialog::fillUpper()
{
	return ui->colormapWidget->fillUpper();
}

bool MeasuredDataPointSettingDialog::fillLower()
{
	return ui->colormapWidget->fillLower();
}

void MeasuredDataPointSettingDialog::setScalarBarTitleMap(const QMap<QString, QString>& titlemap)
{
	m_scalarBarTitleMap = titlemap;
}

void MeasuredDataPointSettingDialog::setScalarBarSetting(const ScalarBarSetting& setting)
{
	m_scalarBarSetting = setting;
}

const ScalarBarSetting MeasuredDataPointSettingDialog::scalarBarSetting()
{
	return m_scalarBarSetting;
}

QString MeasuredDataPointSettingDialog::scalarBarTitle()
{
	return m_scalarBarTitleMap[currentMeasuredValue()];
}

void MeasuredDataPointSettingDialog::setOpacityPercent(int o)
{
	ui->transparencyWidget->setOpacity(o);
}

int MeasuredDataPointSettingDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void MeasuredDataPointSettingDialog::showColorBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setTitle(m_scalarBarTitleMap[currentMeasuredValue()]);
	dialog.setSetting(m_scalarBarSetting);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_scalarBarTitleMap[currentMeasuredValue()] = dialog.title();
	m_scalarBarSetting = dialog.setting();
}

int MeasuredDataPointSettingDialog::pointSize()
{
	return ui->pointSizeSpinBox->value();
}

void MeasuredDataPointSettingDialog::setPointSize(int pointSize)
{
	ui->pointSizeSpinBox->setValue(pointSize);
}

void MeasuredDataPointSettingDialog::setNoPolyData(bool noPolyData)
{
	ui->contourWidget->setNoPolyData(noPolyData);
}
