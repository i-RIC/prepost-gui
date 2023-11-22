#include "ui_v4postzoneselectingdialog.h"

#include "../postcontainer/v4postzonedatacontainer.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "v4postzoneselectingdialog.h"

#include <algorithm>

v4PostZoneSelectingDialog::v4PostZoneSelectingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::v4PostZoneSelectingDialog)
{
	ui->setupUi(this);
	connect(ui->gridTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setupZoneList(int)));
}

v4PostZoneSelectingDialog::~v4PostZoneSelectingDialog()
{
	delete ui;
}

void v4PostZoneSelectingDialog::setContainers(const std::vector<v4PostZoneDataContainer*>& containers)
{
	for (auto cont : containers) {
		auto it = std::find(m_gridTypes.begin(), m_gridTypes.end(), cont->gridType());
		if (it == m_gridTypes.end()) {
			m_gridTypes.push_back(cont->gridType());
			std::vector<v4PostZoneDataContainer*> empty;
			m_zoneLists.insert({cont->gridType(), empty});
		}
		auto& list = m_zoneLists[cont->gridType()];
		list.push_back(cont);
	}
}

void v4PostZoneSelectingDialog::setupGridTypeComboBox()
{
	for (auto gType : m_gridTypes) {
		ui->gridTypeComboBox->addItem(gType->caption());
	}
	if (ui->gridTypeComboBox->count() <= 1) {
		ui->gridTypeComboBox->setDisabled(true);
	}
	setupZoneList(0);
}

void v4PostZoneSelectingDialog::setupZoneList(int index)
{
	ui->zoneList->clear();

	auto gt = m_gridTypes.at(index);
	for (auto c : m_zoneLists[gt]) {
		ui->zoneList->addItem(c->caption());
	}
}

SolverDefinitionGridType* v4PostZoneSelectingDialog::currentGridType() const
{
	int index = ui->gridTypeComboBox->currentIndex();
	return m_gridTypes.at(index);
}

std::string v4PostZoneSelectingDialog::gridTypeName() const
{
	return currentGridType()->name();
}

std::string v4PostZoneSelectingDialog::zoneName() const
{
	auto& list = m_zoneLists.at(currentGridType());
	return list.at(ui->zoneList->currentRow())->zoneName();
}
