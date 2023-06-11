#ifndef GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESSETTINGDIALOG_H
#define GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESSETTINGDIALOG_H

#include "geodatapointmap_tinmanager.h"

#include <QDialog>

namespace Ui {
class GeoDataPointmap_TINManager_RemoveTrianglesSettingDialog;
}

class GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog : public QDialog
{
	Q_OBJECT

public:
	RemoveTrianglesSettingDialog(TINManager* tinManager, QWidget *parent);
	~RemoveTrianglesSettingDialog();

public slots:
	void accept() override;

private slots:
	void preview();

private:
	RemoveTrianglesSetting setting() const;
	void setSetting(const RemoveTrianglesSetting& setting);

	void setupTinActor();

	vtkPolyData* m_tin;
	vtkActor* m_tinActor;

	TINManager* m_tinManager;
	Ui::GeoDataPointmap_TINManager_RemoveTrianglesSettingDialog *ui;
};

#endif // GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESSETTINGDIALOG_H
