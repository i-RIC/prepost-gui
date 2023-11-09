#include "geodatapolydatagroup.h"
#include "geodatapolydatagroupcreator.h"
#include "geodatapolydatagroupcopysettingdialog.h"
#include "geodatapolydatagrouppolydata.h"
#include "ui_geodatapolydatagroupcopysettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/pre/gridcond/base/gridattributestringconverter.h>

#include <QStandardItemModel>

GeoDataPolyDataGroupCopySettingDialog::GeoDataPolyDataGroupCopySettingDialog(GeoDataPolyDataGroup* orig, GeoDataPolyDataGroup* copy, QWidget *parent) :
	QDialog(parent),
	m_originalData {orig},
	m_copyData {copy},
	m_model {new QStandardItemModel(this)},
	ui(new Ui::GeoDataPolyDataGroupCopySettingDialog)
{
	ui->setupUi(this);
	ui->tableView->setModel(m_model);

	int copyValCol = 2;

	auto origAlldata = orig->allData();
	m_model->setRowCount(static_cast<int> (origAlldata.size()));

	if (orig->gridAttribute()->isReferenceInformation()) {
		copyValCol = 1;
		m_model->setColumnCount(2);
		m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
		m_model->setHeaderData(1, Qt::Horizontal, copy->gridAttribute()->caption());
	} else {
		m_model->setColumnCount(3);
		m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
		m_model->setHeaderData(1, Qt::Horizontal, orig->gridAttribute()->caption());
		m_model->setHeaderData(2, Qt::Horizontal, copy->gridAttribute()->caption());

		auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemI*> (orig->parent()->parent());
		auto origAtt = m_originalData->gridAttribute();
		auto converter = origAtt->stringConverter();
		i->setupStringConverter(converter);

		for (int i = 0; i < origAlldata.size(); ++i) {
			auto data = origAlldata.at(i);
			m_model->setData(m_model->index(i, 1), converter->convert(data->value()));
		}
		delete converter;
	}
	// set name and default value
	QVariant defaultVal = copy->gridAttribute()->variantDefaultValue();
	auto copyAtt = copy->gridAttribute();
	auto geoDataGroup = dynamic_cast<PreProcessorGeoDataGroupDataItemI*> (copy->parent()->parent());
	auto adata = copy->allData();
	for (int i = 0; i < adata.size(); ++i) {
		auto data = adata.at(i);
		m_model->setData(m_model->index(i, 0), data->name());
		m_model->setData(m_model->index(i, copyValCol), defaultVal);

		auto w = copyAtt->editWidget(ui->tableView);
		geoDataGroup->setupEditWidget(w);
		w->setVariantValue(defaultVal);

		ui->tableView->setIndexWidget(m_model->index(i, copyValCol), w);
	}

	m_allEditWidget = copyAtt->editWidget(this);
	m_allEditWidget->setVariantValue(defaultVal);
	ui->editWidget->setWidget(m_allEditWidget);
}

GeoDataPolyDataGroupCopySettingDialog::~GeoDataPolyDataGroupCopySettingDialog()
{
	delete ui;
}

void GeoDataPolyDataGroupCopySettingDialog::accept()
{
	if (ui->allRadioButton->isChecked()) {
		auto v = m_allEditWidget->variantValue();
		auto adata = m_copyData->allData();
		for (int i = 0; i < adata.size(); ++i) {
			auto data = adata.at(i);
			data->setValue(v);
		}
	} else {
		int copyValCol = m_model->columnCount() - 1;
		auto adata = m_copyData->allData();
		for (int i = 0; i < adata.size(); ++i) {
			auto data = adata.at(i);
			auto editor = dynamic_cast<GridAttributeEditWidget*> (ui->tableView->indexWidget(m_model->index(i, copyValCol)));
			data->setValue(editor->variantValue());
		}
	}
	QDialog::accept();
}
