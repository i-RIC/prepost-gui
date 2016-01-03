#include "ui_measureddatapointsettingdialog.h"

#include "../project/measureddata.h"
#include "measureddatapointsettingdialog.h"

#include <guibase/scalarbardialog.h>

#include <QMessageBox>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

MeasuredDataPointSettingDialog::MeasuredDataPointSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::MeasuredDataPointSettingDialog}
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

void MeasuredDataPointSettingDialog::forceSelectPointsOnly()
{
	ui->contourWidget->forceSelectPointsOnly();
}

void MeasuredDataPointSettingDialog::setSetting(const MeasuredDataPointGroupDataItem::Setting& s)
{
	m_setting = s;

	// numberOfDivisions
	ui->colormapWidget->setDivisionNumber(s.numberOfDivisions);

	// currentMeasuredValue
	int index = m_measuredValues.indexOf(s.currentMeasuredValue);
	if (index == -1) {
		index = 0;
	}
	ui->measuredValueComboBox->setCurrentIndex(index);
	measuredValueChanged(index);

	// contour
	ui->contourWidget->setContour(s.contour);

	// fillUpper
	ui->colormapWidget->setFillUpper(s.fillUpper);

	// fillLower
	ui->colormapWidget->setFillLower(s.fillLower);

	// pointSize
	ui->pointSizeSpinBox->setValue(s.pointSize);

	// opacity
	ui->transparencyWidget->setOpacityPercent(s.opacity);
}

void MeasuredDataPointSettingDialog::setScalarBarTitleMap(const QMap<QString, QString>& titlemap)
{
	m_scalarBarTitleMap = titlemap;
}

void MeasuredDataPointSettingDialog::setLookupTables(const QMap<QString, LookupTableContainer*>& lookuptables)
{
	m_lookupTables = lookuptables;
}

MeasuredDataPointGroupDataItem::Setting MeasuredDataPointSettingDialog::setting() const
{
	MeasuredDataPointGroupDataItem::Setting ret = m_setting;

	// numberOfDivisions
	ret.numberOfDivisions = ui->colormapWidget->divisionNumber();

	// currentMeasuredValue
	ret.currentMeasuredValue = m_measuredValues.at(ui->measuredValueComboBox->currentIndex());

	// contour
	ret.contour = ui->contourWidget->contour();

	// fillUpper
	ret.fillUpper = ui->colormapWidget->fillUpper();

	// fillLower
	ret.fillLower = ui->colormapWidget->fillLower();

	// pointSize
	ret.pointSize = ui->pointSizeSpinBox->value();

	// opacity
	ret.opacity = ui->transparencyWidget->opacityPercent();

	return ret;
}

LookupTableContainer MeasuredDataPointSettingDialog::lookupTable() const
{
	return m_lookupTable;
}

QString MeasuredDataPointSettingDialog::scalarBarTitle() const
{
	MeasuredDataPointGroupDataItem::Setting s = setting();
	return m_scalarBarTitleMap[s.currentMeasuredValue];
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

void MeasuredDataPointSettingDialog::showColorBarDialog()
{
	ScalarBarDialog dialog(this);
	MeasuredDataPointGroupDataItem::Setting s = setting();

	dialog.setTitle(m_scalarBarTitleMap[s.currentMeasuredValue]);
	dialog.setSetting(m_setting.scalarBarSetting);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_scalarBarTitleMap[s.currentMeasuredValue] = dialog.title();
	m_setting.scalarBarSetting = dialog.setting();
}
