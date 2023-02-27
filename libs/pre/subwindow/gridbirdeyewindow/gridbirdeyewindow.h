#ifndef GRIDBIRDEYEWINDOW_H
#define GRIDBIRDEYEWINDOW_H

#include "../../datamodel/preprocessorgriddataitem.h"

#include <QMainWindow>
#include <QIcon>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/bgcolor/backgroundcoloreditvtkinterface.h>

class GridBirdEyeWindowGraphicsView;
class GridBirdEyeWindowActionManager;
class GridBirdEyeWindowDataModel;

class QAction;

/// This class represents the two-dimensional post-processing window.
class GridBirdEyeWindow :
	public QMainWindow,
	public SnapshotEnabledWindowInterface,
	public AdditionalMenuWindowI,
	public BackgroundColorEditVtkInterface
{
	Q_OBJECT

public:
	GridBirdEyeWindow(QWidget* parent, PreProcessorGridDataItem* item);
	~GridBirdEyeWindow();

	QPixmap snapshot() override;
	vtkRenderWindow* getVtkRenderWindow() const override;

	QList<QMenu*> getAdditionalMenus() const override;
	virtual QToolBar* getAdditionalToolBar() const override;

	void updateGrid();
	const QIcon& icon() const;

public slots:
	void cameraFit();
	void cameraZoomIn();
	void cameraZoomOut();
	void cameraMoveLeft();
	void cameraMoveRight();
	void cameraMoveUp();
	void cameraMoveDown();
	void cameraXYPlane();
	void cameraYZPlane();
	void cameraZXPlane();
	void cameraParallelProjection();
	void cameraPerspectiveProjection();
	void updateProjectionMenu(QAction* parallel, QAction* perspective);

	void editBackgroundColor();
	void editDisplaySetting();
	void editZScale();

private:
	void init(PreProcessorGridDataItem* item);
	VTKGraphicsView* viewForBackgroundColor() const override;

	GridBirdEyeWindowGraphicsView* m_graphicsView;
	GridBirdEyeWindowActionManager* m_actionManager;
	GridBirdEyeWindowDataModel* m_dataModel;
	QIcon m_icon;

public:
	friend class GridBirdEyeWindowActionManager;
};

#endif // GRIDBIRDEYEWINDOW_H
