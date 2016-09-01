#include "ui_measureddatapointsettingdialog.h"

#include "measureddatapointsettingdialog.h"
#include "private/measureddatapointsettingdialog_impl.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <guicore/project/measured/measureddata.h>
#include <misc/stringtool.h>

#include <QMessageBox>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

MeasuredDataPointSettingDialog::MeasuredDataPointSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::MeasuredDataPointSettingDialog},
	impl {new Impl()}
{
	ui->setupUi(this);
	connect(ui->measuredValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showColorBarDialog()));
	connect(ui->contourWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ui->colormapWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));
	connect(ui->contourWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), this, SLOT(handleContourChange(ContourSettingWidget::Contour)));
}

MeasuredDataPointSettingDialog::~MeasuredDataPointSettingDialog()
{
	delete ui;
	delete impl;
}

void MeasuredDataPointSettingDialog::setData(MeasuredData* md)
{
	ComboBoxTool::setupItems(md->scalarNames(), ui->measuredValueComboBox);

	impl->m_targets = md->scalarNames();
}

void MeasuredDataPointSettingDialog::forceSelectPointsOnly()
{
	ui->contourWidget->forceSelectPointsOnly();
}

void MeasuredDataPointSettingDialog::setSetting(const MeasuredDataPointSetting &s)
{
	impl->m_setting = s;

	ui->measuredValueComboBox->setCurrentText(s.target);
	targetChanged(ui->measuredValueComboBox->currentIndex());

	ui->contourWidget->setContour(s.contour);
	ui->colormapWidget->setDivisionNumber(s.numberOfDivisions);
	ui->colormapWidget->setFillUpper(s.fillUpper);
	ui->colormapWidget->setFillLower(s.fillLower);
	ui->transparencyWidget->setOpacityPercent(s.opacity);
	ui->pointSizeSpinBox->setValue(s.pointSize);
}

void MeasuredDataPointSettingDialog::setScalarBarTitleMap(const std::unordered_map<std::string, QString>& titlemap)
{
	impl->m_scalarBarTitleMap = titlemap;
}

void MeasuredDataPointSettingDialog::setLookupTables(const std::unordered_map<std::string, LookupTableContainer*>& lookuptables)
{
	impl->m_lookupTables = lookuptables;
}

MeasuredDataPointSetting MeasuredDataPointSettingDialog::setting() const
{
	MeasuredDataPointSetting ret = impl->m_setting;

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
	return impl->m_lookupTable;
}

QString MeasuredDataPointSettingDialog::scalarBarTitle() const
{
	auto s = setting();
	return impl->m_scalarBarTitleMap.at(iRIC::toStr(s.target));
}

void MeasuredDataPointSettingDialog::targetChanged(int index)
{
	auto target = impl->m_targets.at(index);
	LookupTableContainer* c = impl->m_lookupTables.at(target);
	impl->m_lookupTable = *c;
	ui->colormapWidget->setContainer(&impl->m_lookupTable);
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

void MeasuredDataPointSettingDialog::handleContourChange(ContourSettingWidget::Contour c)
{
	ui->pointSizeSpinBox->setEnabled(c == ContourSettingWidget::Points);
}

void MeasuredDataPointSettingDialog::showColorBarDialog()
{
	ScalarBarDialog dialog(this);
	MeasuredDataPointSetting s = setting();

	dialog.setTitle(impl->m_scalarBarTitleMap[s.target]);
	dialog.setSetting(impl->m_setting.scalarBarSetting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	impl->m_scalarBarTitleMap[s.target] = dialog.title();
	impl->m_setting.scalarBarSetting = dialog.setting();
}
