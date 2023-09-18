#ifndef GRIDBIRDEYEWINDOW_H
#define GRIDBIRDEYEWINDOW_H

#include "../../datamodel/preprocessorgriddataitem.h"

#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/qmainwindowwithsnapshot.h>
#include <guicore/base/windowwithobjectbrowseri.h>
#include <guicore/bgcolor/backgroundcoloreditvtki.h>

#include <QIcon>

class GridBirdEyeObjectBrowser;
class GridBirdEyeWindowActionManager;
class GridBirdEyeWindowDataModel;
class GridBirdEyeWindowProjectDataItem;

class QAction;

/// This class represents the two-dimensional post-processing window.
class GridBirdEyeWindow :
	public QMainWindowWithSnapshot,
	public AdditionalMenuWindowI,
	public BackgroundColorEditVtkI,
	public WindowWithObjectBrowserI
{
	Q_OBJECT

public:
	GridBirdEyeWindow(QWidget* parent, PreProcessorGridDataItem* item);
	~GridBirdEyeWindow();

	ObjectBrowser* objectBrowser() const override;

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	vtkRenderWindow* getVtkRenderWindow() const override;

	QList<QMenu*> getAdditionalMenus() const override;
	const std::shared_ptr<QToolBar>& getAdditionalToolBar() const override;

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

	GridBirdEyeObjectBrowser* m_objectBrowser;
	GridBirdEyeWindowDataModel* m_dataModel;
	GridBirdEyeWindowActionManager* m_actionManager;

	GridBirdEyeWindowProjectDataItem* m_projectDataItem;
	QIcon m_icon;

public:
	friend class GridBirdEyeWindowActionManager;
	friend class GridBirdEyeWindowProjectDataItem;
};

#endif // GRIDBIRDEYEWINDOW_H
