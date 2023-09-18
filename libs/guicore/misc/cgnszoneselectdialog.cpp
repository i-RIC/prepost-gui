#include "ui_cgnszoneselectdialog.h"

#include "cgnszoneselectdialog.h"

#include <h5cgnszone.h>

CgnsZoneSelectDialog::CgnsZoneSelectDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::CgnsZoneSelectDialog)
{
	ui->setupUi(this);
}

CgnsZoneSelectDialog::~CgnsZoneSelectDialog()
{
	delete ui;
}

void CgnsZoneSelectDialog::setMessage(const QString& message)
{
	ui->label->setText(message);
}

void CgnsZoneSelectDialog::setZones(const std::vector<iRICLib::H5CgnsZone*>& zones)
{
	m_zones = zones;

	ui->comboBox->clear();
	for (auto zone : zones) {
		ui->comboBox->addItem(zone->name().c_str());
	}
}

iRICLib::H5CgnsZone* CgnsZoneSelectDialog::selectedZone() const
{
	return m_zones.at(ui->comboBox->currentIndex());
}
