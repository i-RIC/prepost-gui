#include "gridcomplexconditiongroup.h"
#include "gridcomplexconditiongroupeditdialog.h"
#include "ui_gridcomplexconditiongroupeditdialog.h"
#include "../../project/inputcond/inputconditioncontainerset.h"

#include <QMessageBox>

GridComplexConditionGroupEditDialog::GridComplexConditionGroupEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridComplexConditionGroupEditDialog),
	m_currentIndex {-1},
	m_modified {false}
{
	ui->setupUi(this);
	connect(ui->targetComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleIndexChange(int)));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridComplexConditionGroupEditDialog::~GridComplexConditionGroupEditDialog()
{
	delete ui;
}

void GridComplexConditionGroupEditDialog::setGroups(const std::vector<GridComplexConditionGroup*>& groups)
{
	m_groups = groups;
	for (auto g : groups) {
		m_backups.push_back(g->containerSet()->clone());
	}
	updateTargetComboBox();
}

void GridComplexConditionGroupEditDialog::setCurrentIndex(int index)
{
	if (m_currentIndex != -1) {
		auto group = m_groups.at(m_currentIndex);
		group->containerSet()->disconnect(this);
	}
	auto group = m_groups.at(index);
	connect(group->containerSet(), SIGNAL(modified()), this, SLOT(handleModified()));
	setTargetGroup(group);

	ui->targetComboBox->setCurrentIndex(index);
	m_currentIndex = index;
}

void GridComplexConditionGroupEditDialog::handleIndexChange(int index)
{
	setCurrentIndex(index);
}

void GridComplexConditionGroupEditDialog::handleModified()
{
	m_modified = true;
	updateTargetComboBox();
}

void GridComplexConditionGroupEditDialog::accept()
{
	clearBackups();
	ui->editWidget->setGroup(nullptr);
	QDialog::accept();
}

void GridComplexConditionGroupEditDialog::reject()
{
	if (m_modified) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Modifications you made will be discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) {return;}
	}

	for (int i = 0; i < m_groups.size(); ++i) {
		auto g = m_groups.at(i);
		g->containerSet()->copyValues(m_backups.at(i));
	}
	clearBackups();
	ui->editWidget->setGroup(nullptr);
	QDialog::reject();
}

void GridComplexConditionGroupEditDialog::setTargetGroup(GridComplexConditionGroup* group)
{
	ui->editWidget->setGroup(group);
}

void GridComplexConditionGroupEditDialog::updateTargetComboBox()
{
	auto cb = ui->targetComboBox;
	cb->blockSignals(true);
	cb->clear();

	for (int i = 0; i < m_groups.size(); ++i) {
		auto g = m_groups.at(i);
		QString groupName = QString::number(i + 1);
		auto cs = g->containerSet();
		auto name = cs->container("name");
		if (name != nullptr) {
			auto strName = dynamic_cast<InputConditionContainerString*> (name);
			if (strName !=  nullptr) {
				groupName = strName->value();
			}
		}
		cb->addItem(groupName);
	}

	if (m_currentIndex != -1) {
		cb->setCurrentIndex(m_currentIndex);
	}

	cb->blockSignals(false);
}

void GridComplexConditionGroupEditDialog::clearBackups()
{
	for (auto backup : m_backups) {
		delete backup;
	}
	m_backups.clear();
}
