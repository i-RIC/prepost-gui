#ifndef POST3DWINDOW_H
#define POST3DWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/windowwithobjectbrowserinterface.h>
#include <guicore/bgcolor/backgroundcoloreditvtkinterface.h>
#include <postbase/autoparticlewindowi.h>
#include <postbase/particleexportwindowi.h>

class QAction;
class QToolBar;
class Post3dObjectBrowser;
class Post3dWindowDataModel;
class Post3dWindowProjectDataItem;
class Post3dWindowActionManager;

/// This class represents the two-dimensional post-processing window.
class Post3dWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowI,
	public WindowWithObjectBrowserInterface,
	public ParticleExportWindowI,
	public BackgroundColorEditVtkInterface,
	public AutoParticleWindowI
{
	Q_OBJECT

public:
	Post3dWindow(QWidget* parent, int index, Post3dWindowProjectDataItem* pdi);
	~Post3dWindow();

	QPixmap snapshot() override;
	vtkRenderWindow* getVtkRenderWindow() const override;

	QList<QMenu*> getAdditionalMenus() const override;
	const std::shared_ptr<QToolBar>& getAdditionalToolBar() const override;

	ObjectBrowser* objectBrowser() const override;
	bool exportParticles(const QString& filename, int fileIndex, double time, const QString& zonename) override;
	QList<QString> particleDrawingZones() override;
	bool isAutoParticleOutput() const override;
	bool hasTransparentPart();

public slots:
	void cameraFit();
	void cameraResetRotation();
	void cameraRotate90();
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
	void editZScale();

private:
	void init();
	VTKGraphicsView* viewForBackgroundColor() const override;
	void setupDefaultGeometry(int index) override;
	Post3dObjectBrowser* m_objectBrowser;
	Post3dWindowDataModel* m_dataModel;
	Post3dWindowActionManager* m_actionManager;
	QByteArray m_initialState;

public:
	friend class Post3dWindowProjectDataItem;
	friend class Post3dWindowActionManager;
	friend class Post3dWindowDataModel;
};

#endif // POST3DWINDOW_H
