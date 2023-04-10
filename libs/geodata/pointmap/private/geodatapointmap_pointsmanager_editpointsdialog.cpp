#include "geodatapointmap_pointsmanager_editpointscommand.h"
#include "geodatapointmap_pointsmanager_editpointsdialog.h"
#include "geodatapointmap_pointsmanager_editsinglepointcommand.h"
#include "ui_geodatapointmap_pointsmanager_editpointsdialog.h"

#include <misc/iricundostack.h>

GeoDataPointmap::PointsManager::EditPointsDialog::EditPointsDialog(PointsManager* manager, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_manager {manager},
	ui(new Ui::GeoDataPointmap_PointsManager_EditPointsDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &EditPointsDialog::handleButtonClick);

	updateProperties();
}

GeoDataPointmap::PointsManager::EditPointsDialog::~EditPointsDialog()
{
	delete ui;
}

void GeoDataPointmap::PointsManager::EditPointsDialog::accept()
{
	m_manager->m_parent->pushModifyCommand(createModifyCommand(false));

	QDialog::accept();
}

void GeoDataPointmap::PointsManager::EditPointsDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void GeoDataPointmap::PointsManager::EditPointsDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataPointmap::PointsManager::EditPointsDialog::updateProperties()
{
	auto indices = m_manager->selectedPointIndices();
	if (indices.size() == 0) {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
		ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
		return;
	}
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);

	auto vals = m_manager->values();
	double min;
	double max;

	bool first = true;
	for (auto index : indices) {
		double v = vals->GetValue(index);
		if (first || v > max) {max = v;}
		if (first || v < min) {min = v;}

		first = false;
	}

	ui->minValueLabel->setText(QString::number(min));
	ui->maxValueLabel->setText(QString::number(max));
	ui->numPointsValueLabel->setText(QString::number(indices.size()));

	if (indices.size() > 1) {
		ui->xEdit->setDisabled(true);
		ui->yEdit->setDisabled(true);
	} else if (indices.size() == 1) {
		auto index = *indices.begin();
		double v[3];
		auto points = m_manager->points()->GetPoints();
		points->GetPoint(index, v);
		ui->xEdit->setValue(v[0]);
		ui->yEdit->setValue(v[1]);

		auto values = m_manager->values();
		double val = values->GetValue(index);
		ui->valueEdit->setValue(val);
	}
}

void GeoDataPointmap::PointsManager::EditPointsDialog::apply()
{
	m_manager->m_parent->pushModifyCommand(createModifyCommand(true));
	m_applied = true;
}

QUndoCommand* GeoDataPointmap::PointsManager::EditPointsDialog::createModifyCommand(bool apply) const
{
	auto indices = m_manager->selectedPointIndices();
	if (indices.size() > 1) {
		return new EditPointsCommand(apply, ui->valueEdit->value(), indices, m_manager);
	} else if (indices.size() == 1) {
		auto index = *indices.begin();
		return new EditSinglePointCommand(apply, ui->xEdit->value(), ui->yEdit->value(), ui->valueEdit->value(), index, m_manager);
	}
}
