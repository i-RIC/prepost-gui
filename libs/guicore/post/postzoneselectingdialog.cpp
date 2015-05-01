#include "postzoneselectingdialog.h"
#include "ui_postzoneselectingdialog.h"
#include "../postcontainer/postzonedatacontainer.h"
#include "../solverdef/solverdefinitiongridtype.h"

PostZoneSelectingDialog::PostZoneSelectingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PostZoneSelectingDialog)
{
	ui->setupUi(this);
	connect(ui->gridTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setupZoneList(int)));
}

PostZoneSelectingDialog::~PostZoneSelectingDialog()
{
	delete ui;
}

void PostZoneSelectingDialog::setContainers(const QList<PostZoneDataContainer*>& containers)
{
	for (auto it = containers.begin(); it != containers.end(); ++it) {
		PostZoneDataContainer* cont = *it;
		if (! m_gridTypes.contains(cont->gridType())) {
			m_gridTypes.append(cont->gridType());
			QList<PostZoneDataContainer*> list;
			m_zoneLists.insert(cont->gridType(), list);
		}
		QList<PostZoneDataContainer*>& list = m_zoneLists[cont->gridType()];
		list.append(cont);
	}
}

void PostZoneSelectingDialog::setupGridTypeComboBox()
{
	for (auto it = m_gridTypes.begin(); it != m_gridTypes.end(); ++it) {
		SolverDefinitionGridType* gtype = *it;
		ui->gridTypeComboBox->addItem(gtype->caption());
	}
	if (ui->gridTypeComboBox->count() <= 1) {
		ui->gridTypeComboBox->setDisabled(true);
	}
	setupZoneList(0);
}

void PostZoneSelectingDialog::setupZoneList(int index)
{
	SolverDefinitionGridType* gt = m_gridTypes.at(index);
	const QList<PostZoneDataContainer*>& list = m_zoneLists.value(gt);
	ui->zoneList->clear();
	for (auto it = list.begin(); it != list.end(); ++it) {
		PostZoneDataContainer* c = *it;
		ui->zoneList->addItem(c->caption());
	}
}

QString PostZoneSelectingDialog::gridTypeName()
{
	int index = ui->gridTypeComboBox->currentIndex();
	SolverDefinitionGridType* gt = m_gridTypes.at(index);
	return gt->name();
}

QString PostZoneSelectingDialog::zoneName()
{
	int index = ui->gridTypeComboBox->currentIndex();
	SolverDefinitionGridType* gt = m_gridTypes.at(index);
	const QList<PostZoneDataContainer*>& list = m_zoneLists.value(gt);
	int index2 = ui->zoneList->currentRow();
	PostZoneDataContainer* cont = list.at(index2);
	return cont->zoneName();
}
