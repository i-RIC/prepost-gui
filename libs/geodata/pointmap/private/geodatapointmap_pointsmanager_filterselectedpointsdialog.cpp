#include "geodatapointmap_pointsmanager_filterselectedpointsdialog.h"
#include "ui_geodatapointmap_pointsmanager_filterselectedpointsdialog.h"

namespace {

std::unordered_set<vtkIdType> filterGreaterThan(vtkDoubleArray* values, const std::unordered_set<vtkIdType>& indices, double threshold)
{
	std::unordered_set<vtkIdType> ret;
	for (auto index : indices) {
		double v = values->GetValue(index);
		if (v >= threshold) {
			ret.insert(index);
		}
	}
	return ret;
}

std::unordered_set<vtkIdType> filterLessThan(vtkDoubleArray* values, const std::unordered_set<vtkIdType>& indices, double threshold)
{
	std::unordered_set<vtkIdType> ret;
	for (auto index : indices) {
		double v = values->GetValue(index);
		if (v <= threshold) {
			ret.insert(index);
		}
	}
	return ret;
}

} // namespace

GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::FilterSelectedPointsDialog(PointsManager* manager, QWidget *parent) :
	QDialog (parent),
	m_selectedVerticesBackup {manager->selectedPointIndicesUnorderedSet()},
	m_manager {manager},
	ui(new Ui::GeoDataPointmap_PointsManager_FilterSelectedPointsDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &FilterSelectedPointsDialog::handleButtonClick);

	updateProperties();
}

GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::~FilterSelectedPointsDialog()
{
	delete ui;
}

void GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::accept()
{
	m_manager->setSelectedPointIndices(filter());
	m_manager->m_parent->renderGraphicsView();

	QDialog::accept();
}

void GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::reject()
{
	m_manager->setSelectedPointIndices(m_selectedVerticesBackup);
	m_manager->m_parent->renderGraphicsView();

	QDialog::reject();
}

void GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::updateProperties()
{
	std::vector<vtkIdType> indices;
	for (auto index : m_selectedVerticesBackup) {
		indices.push_back(index);
	}
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
	ui->numPointsValueLabel->setText(QString::number(m_selectedVerticesBackup.size()));

	double average = (min + max) * 0.5;
	ui->thresholdEdit->setValue(average);
}

void GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::apply()
{
	m_manager->setSelectedPointIndices(filter());
	m_manager->m_parent->renderGraphicsView();
}

std::unordered_set<vtkIdType> GeoDataPointmap::PointsManager::FilterSelectedPointsDialog::filter() const
{
	double threshold = ui->thresholdEdit->value();
	auto vals = m_manager->values();

	if (ui->greaterRadioButton->isChecked()) {
		return filterGreaterThan(vals, m_selectedVerticesBackup, threshold);
	} else if (ui->lessRadioButton->isChecked()) {
		return filterLessThan(vals, m_selectedVerticesBackup, threshold);
	}
}
