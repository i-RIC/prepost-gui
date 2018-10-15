#include "datamodel/post3dwindowgridtypedataitem.h"
#include "datamodel/post3dwindownodevectorparticlegroupdataitem.h"
#include "datamodel/post3dwindowrootdataitem.h"
#include "datamodel/post3dwindowzonedataitem.h"
#include "post3dobjectbrowser.h"
#include "post3dwindow.h"
#include "post3dwindowactionmanager.h"
#include "post3dwindowdatamodel.h"
#include "post3dwindowdatamodel.h"
#include "post3dwindowgraphicsview.h"
#include "post3dwindowprojectdataitem.h"

#include <guibase/colortool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QColorDialog>
#include <QLabel>
#include <QStandardItem>
#include <QToolBar>
#include <QUndoCommand>

#include <vtkRenderer.h>

Post3dWindow::Post3dWindow(QWidget* parent, int index, Post3dWindowProjectDataItem* pdi)
	: PostProcessorWindow(parent, index, pdi)
{
	init();
}

Post3dWindow::~Post3dWindow()
{
	delete m_dataModel;
}

void Post3dWindow::init()
{
	setWindowTitle(tr("Post-processing (3D): %1").arg(m_index));

	m_icon = QIcon(":/libs/post/post3d/images/iconVis3D.png");
	// set default central widget.
	QLabel* label = new QLabel(this);
	setCentralWidget(label);

	m_actionManager = new Post3dWindowActionManager(this);

	m_objectBrowser = new Post3dObjectBrowser(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);

	connect(postSolutionInfo(), SIGNAL(updated()), this, SLOT(update()));

	m_initialState = saveState();
}

void Post3dWindow::setupDefaultGeometry(int index)
{
	QWidget* parent = parentWidget();
	if (parent->isMaximized()) {return;}
	parent->move(index * 30, index * 30);
	parent->resize(700, 500);
	restoreState(m_initialState);
}

QPixmap Post3dWindow::snapshot()
{
	Post3dWindowGraphicsView* view = m_dataModel->graphicsView();
	QImage img = view->getImage();
	QPixmap pixmap = QPixmap::fromImage(img);
	if (m_isTransparent) { makeBackgroundTransparent(view, pixmap); }

	return pixmap;
}

vtkRenderWindow* Post3dWindow::getVtkRenderWindow() const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (m_dataModel->graphicsView()->mainRenderer());
	return r->GetRenderWindow();
}

QList<QMenu*> Post3dWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}

QToolBar* Post3dWindow::getAdditionalToolBar() const
{
	return nullptr;
}

void Post3dWindow::cameraFit()
{
	m_dataModel->graphicsView()->cameraFit();
}

void Post3dWindow::cameraResetRotation()
{
	m_dataModel->graphicsView()->cameraResetRotation();
}

void Post3dWindow::cameraRotate90()
{
	m_dataModel->graphicsView()->cameraRotate90();
}

void Post3dWindow::cameraZoomIn()
{
	m_dataModel->graphicsView()->cameraZoomIn();
}

void Post3dWindow::cameraZoomOut()
{
	m_dataModel->graphicsView()->cameraZoomOut();
}

void Post3dWindow::cameraMoveLeft()
{
	m_dataModel->graphicsView()->cameraMoveLeft();
}

void Post3dWindow::cameraMoveRight()
{
	m_dataModel->graphicsView()->cameraMoveRight();
}

void Post3dWindow::cameraMoveUp()
{
	m_dataModel->graphicsView()->cameraMoveUp();
}

void Post3dWindow::cameraMoveDown()
{
	m_dataModel->graphicsView()->cameraMoveDown();
}

void Post3dWindow::cameraXYPlane()
{
	m_dataModel->graphicsView()->cameraToXYPlane();
}

void Post3dWindow::cameraYZPlane()
{
	m_dataModel->graphicsView()->cameraToYZPlane();
}

void Post3dWindow::cameraZXPlane()
{
	m_dataModel->graphicsView()->cameraToZXPlane();
}

void Post3dWindow::cameraParallelProjection()
{
	m_dataModel->graphicsView()->cameraParallelProjection();
}

void Post3dWindow::cameraPerspectiveProjection()
{
	m_dataModel->graphicsView()->cameraPerspectiveProjection();
}

void Post3dWindow::updateProjectionMenu(QAction* parallel, QAction* perspective)
{
	m_dataModel->graphicsView()->updateProjectionMenu(parallel, perspective);
}

void Post3dWindow::editBackgroundColor()
{
	BackgroundColorEditInterface::editBackgroundColor(this);
}

void Post3dWindow::editZScale()
{
	m_dataModel->editZScale();
}

bool Post3dWindow::exportParticles(const QString& filename, int fileIndex, double time, const QString& zonename)
{
	Post3dWindowRootDataItem* rItem = dynamic_cast<Post3dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post3dWindowZoneDataItem* zItem = rItem->zoneDataItem(iRIC::toStr(zonename));
	Post3dWindowNodeVectorParticleGroupDataItem* pItem = zItem->particleGroupDataItem();
	return pItem->exportParticles(filename, fileIndex, time);
}

QList<QString> Post3dWindow::particleDrawingZones()
{
	QList<QString> ret;
	Post3dWindowRootDataItem* rItem = dynamic_cast<Post3dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	QList<Post3dWindowGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	for (int i = 0; i < gtItems.count(); ++i) {
		Post3dWindowGridTypeDataItem* gtItem = gtItems.at(i);
		QList<Post3dWindowZoneDataItem*> zItems = gtItem->zoneDatas();
		for (int j = 0; j < zItems.count(); ++j) {
			Post3dWindowZoneDataItem* zItem = zItems.at(j);
			Post3dWindowNodeVectorParticleGroupDataItem* pItem = zItem->particleGroupDataItem();
			if (pItem->standardItem()->checkState() == Qt::Checked && pItem->target() != "") {
				ret.append(zItem->zoneName().c_str());
			}
		}
	}
	return ret;
}

bool Post3dWindow::hasTransparentPart()
{
	Post3dWindowRootDataItem* rItem = dynamic_cast<Post3dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	return rItem->hasTransparentPart();
}

ObjectBrowser* Post3dWindow::objectBrowser() const
{
	return m_objectBrowser;
}

VTKGraphicsView* Post3dWindow::viewForBackgroundColor() const
{
	return m_dataModel->graphicsView();
}
