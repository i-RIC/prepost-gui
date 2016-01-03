#include "ui_measureddatapointsettingdialog.h"

#include "../project/measured/measureddata.h"
#include "measureddatapointsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <misc/stringtool.h>

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
	ComboBoxTool::setupItems(md->scalarNames(), ui->measuredValueComboBox);

	m_targets = md->scalarNames();
}

void MeasuredDataPointSettingDialog::forceSelectPointsOnly()
{
	ui->contourWidget->forceSelectPointsOnly();
}

void MeasuredDataPointSettingDialog::setSetting(const MeasuredDataPointSetting &s)
{
	m_setting = s;

	ui->measuredValueComboBox->setCurrentText(s.target);
	ui->contourWidget->setContour(s.contour);
	ui->colormapWidget->setDivisionNumber(s.numberOfDivisions);
	ui->colormapWidget->setFillUpper(s.fillUpper);
	ui->colormapWidget->setFillLower(s.fillLower);
	ui->transparencyWidget->setOpacityPercent(s.opacity);
	ui->pointSizeSpinBox->setValue(s.pointSize);

	targetChanged(ui->measuredValueComboBox->currentIndex());
}

void MeasuredDataPointSettingDialog::setScalarBarTitleMap(const std::unordered_map<std::string, QString>& titlemap)
{
	m_scalarBarTitleMap = titlemap;
}

void MeasuredDataPointSettingDialog::setLookupTables(const std::unordered_map<std::string, LookupTableContainer*>& lookuptables)
{
	m_lookupTables = lookuptables;
}

MeasuredDataPointSetting MeasuredDataPointSettingDialog::setting() const
{
	MeasuredDataPointSetting ret = m_setting;

	ret.target = ui->measuredValueComboBox->currentText();
	ret.contour = ui->contourWidget->contour();
	ret.numberOfDivisions = ui->colormapWidget->divisionNumber();
	ret.fillUpper = ui->colormapWidget->fillUpper();
	ret.fillLower = ui->colormapWidget->fillLower();
	ret.opacity = ui->transparencyWidget->opacityPercent();
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}

LookupTableContainer MeasuredDataPointSettingDialog::lookupTable() const
{
	return m_lookupTable;
}

QString MeasuredDataPointSettingDialog::scalarBarTitle() const
{
	auto s = setting();
	return m_scalarBarTitleMap.at(iRIC::toStr(s.target));
}

void MeasuredDataPointSettingDialog::targetChanged(int index)
{
	auto target = m_targets.at(index);
	LookupTableContainer* c = m_lookupTables.at(target);
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
	MeasuredDataPointSetting s = setting();

	dialog.setTitle(m_scalarBarTitleMap[s.target]);
	dialog.setSetting(m_setting.scalarBarSetting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_scalarBarTitleMap[s.target] = dialog.title();
	m_setting.scalarBarSetting = dialog.setting();
}
