#include "ui_cgnszoneselectdialog.h"

#include "cgnszoneselectdialog.h"

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

void CgnsZoneSelectDialog::setZones(const QList<int>& zoneids, const QList<QString>& zonenames)
{
	for (auto it = zonenames.begin(); it != zonenames.end(); ++it) {
		ui->comboBox->addItem(*it);
	}
	m_zoneIds = zoneids;
}

int CgnsZoneSelectDialog::zoneId()
{
	return m_zoneIds.at(ui->comboBox->currentIndex());
}
