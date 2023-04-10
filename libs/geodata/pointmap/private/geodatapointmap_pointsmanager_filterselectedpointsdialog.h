#ifndef GEODATAPOINTMAP_POINTSMANAGER_FILTERSELECTEDPOINTSDIALOG_H
#define GEODATAPOINTMAP_POINTSMANAGER_FILTERSELECTEDPOINTSDIALOG_H

#include "geodatapointmap_pointsmanager.h"

#include <QDialog>

#include <unordered_set>

namespace Ui {
class GeoDataPointmap_PointsManager_FilterSelectedPointsDialog;
}

class GeoDataPointmap::PointsManager::FilterSelectedPointsDialog : public QDialog
{
	Q_OBJECT

public:
	FilterSelectedPointsDialog(PointsManager* manager, QWidget *parent);
	~FilterSelectedPointsDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void updateProperties();
	void apply();

	std::unordered_set<vtkIdType> filter() const;

	std::unordered_set<vtkIdType> m_selectedVerticesBackup;

	PointsManager* m_manager;
	Ui::GeoDataPointmap_PointsManager_FilterSelectedPointsDialog *ui;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_FILTERSELECTEDPOINTSDIALOG_H
