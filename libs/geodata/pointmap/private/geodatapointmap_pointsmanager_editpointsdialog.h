#ifndef GEODATAPOINTMAP_POINTSMANAGER_EDITPOINTSDIALOG_H
#define GEODATAPOINTMAP_POINTSMANAGER_EDITPOINTSDIALOG_H

#include "geodatapointmap_pointsmanager.h"

#include <QDialog>

namespace Ui {
class GeoDataPointmap_PointsManager_EditPointsDialog;
}

class GeoDataPointmap::PointsManager::EditPointsDialog : public QDialog
{
	Q_OBJECT

public:
	EditPointsDialog(PointsManager* manager, QWidget *parent);
	~EditPointsDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void updateProperties();
	void apply();

	QUndoCommand* createModifyCommand(bool apply) const;

private:
	bool m_applied;
	PointsManager* m_manager;
	Ui::GeoDataPointmap_PointsManager_EditPointsDialog *ui;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_EDITPOINTSDIALOG_H
