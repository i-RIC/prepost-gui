#include "ui_postzoneselectingdialog.h"

#include "../postcontainer/postzonedatacontainer.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "postzoneselectingdialog.h"

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
	for (PostZoneDataContainer* cont : containers) {
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
	for (SolverDefinitionGridType* gtype : m_gridTypes) {
		ui->gridTypeComboBox->addItem(gtype->caption());
	}
	if (ui->gridTypeComboBox->count() <= 1) {
		ui->gridTypeComboBox->setDisabled(true);
	}
	setupZoneList(0);
}

void PostZoneSelectingDialog::setupZoneList(int index)
{
	ui->zoneList->clear();

	SolverDefinitionGridType* gt = m_gridTypes.at(index);
	for (PostZoneDataContainer* c : m_zoneLists.value(gt)) {
		ui->zoneList->addItem(c->caption());
	}
}

SolverDefinitionGridType* PostZoneSelectingDialog::currentGridType() const
{
	int index = ui->gridTypeComboBox->currentIndex();
	return m_gridTypes.at(index);
}

QString PostZoneSelectingDialog::gridTypeName() const
{
	return currentGridType()->name();
}

QString PostZoneSelectingDialog::zoneName() const
{
	const QList<PostZoneDataContainer*>& list = m_zoneLists.value(currentGridType());
	return list.at(ui->zoneList->currentRow())->zoneName();
}
