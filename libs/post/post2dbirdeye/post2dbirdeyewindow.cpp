#include "datamodel/post2dbirdeyewindowgridtypedataitem.h"
#include "datamodel/post2dbirdeyewindowrootdataitem.h"
#include "datamodel/post2dbirdeyewindowzonedataitem.h"
#include "post2dbirdeyeobjectbrowser.h"
#include "post2dbirdeyewindow.h"
#include "post2dbirdeyewindowactionmanager.h"
#include "post2dbirdeyewindowdatamodel.h"
#include "post2dbirdeyewindowdatamodel.h"
#include "post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowprojectdataitem.h"

#include <guibase/colortool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QColorDialog>
#include <QLabel>
#include <QToolBar>
#include <QUndoCommand>

#include <vtkRenderer.h>

Post2dBirdEyeWindow::Post2dBirdEyeWindow(QWidget* parent, int index, Post2dBirdEyeWindowProjectDataItem* pdi)
	: PostProcessorWindow(parent, index, pdi)
{
	init();
}

Post2dBirdEyeWindow::~Post2dBirdEyeWindow()
{
	delete m_dataModel;
}

void Post2dBirdEyeWindow::init()
{
	setWindowTitle(tr("Bird's-Eye Post-processing (2D): %1").arg(m_index));

	m_icon = QIcon(":/libs/post/post2dbirdeye/images/iconVis2DBirdEye.png");
	// set default central widget.
	QLabel* label = new QLabel(this);
	setCentralWidget(label);

	m_actionManager = new Post2dBirdEyeWindowActionManager(this);

	m_objectBrowser = new Post2dBirdEyeObjectBrowser(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);

	connect(postSolutionInfo(), SIGNAL(updated()), this, SLOT(update()));

	m_initialState = saveState();
}

void Post2dBirdEyeWindow::setupDefaultGeometry(int index)
{
	QWidget* parent = parentWidget();
	if (parent->isMaximized()) {return;}
	parent->move(index * 30, index * 30);
	parent->resize(700, 500);
	restoreState(m_initialState);
}

QPixmap Post2dBirdEyeWindow::snapshot()
{
	Post2dBirdEyeWindowGraphicsView* view = m_dataModel->graphicsView();
	QImage img = view->getImage();
	QPixmap pixmap = QPixmap::fromImage(img);
	if (m_isTransparent) { makeBackgroundTransparent(view, pixmap); }

	return pixmap;
}

vtkRenderWindow* Post2dBirdEyeWindow::getVtkRenderWindow() const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (m_dataModel->graphicsView()->mainRenderer());
	return r->GetRenderWindow();
}

QList<QMenu*> Post2dBirdEyeWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}

void Post2dBirdEyeWindow::cameraFit()
{
	m_dataModel->fit();
}

void Post2dBirdEyeWindow::cameraZoomIn()
{
	m_dataModel->zoomIn();
}

void Post2dBirdEyeWindow::cameraZoomOut()
{
	m_dataModel->zoomOut();
}

void Post2dBirdEyeWindow::cameraMoveLeft()
{
	m_dataModel->moveLeft();
}

void Post2dBirdEyeWindow::cameraMoveRight()
{
	m_dataModel->moveRight();
}

void Post2dBirdEyeWindow::cameraMoveUp()
{
	m_dataModel->moveUp();
}

void Post2dBirdEyeWindow::cameraMoveDown()
{
	m_dataModel->moveDown();
}

void Post2dBirdEyeWindow::cameraXYPlane()
{
	m_dataModel->toXYPlane();
}

void Post2dBirdEyeWindow::cameraYZPlane()
{
	m_dataModel->toYZPlane();
}

void Post2dBirdEyeWindow::cameraZXPlane()
{
	m_dataModel->toZXPlane();
}

class Post2dBirdEyeWindowEditBackgroundColorCommand : public QUndoCommand
{
public:
	Post2dBirdEyeWindowEditBackgroundColorCommand(double oldc[3], double newc[3], Post2dBirdEyeWindow* w)
		: QUndoCommand(QObject::tr("Edit Background Color")) {
		for (int i = 0; i < 3; ++i) {
			m_oldColor[i] = oldc[i];
			m_newColor[i] = newc[i];
		}
		m_window = w;
	}
	void undo() {
		m_window->m_dataModel->graphicsView()->mainRenderer()->SetBackground(m_oldColor);
	}
	void redo() {
		m_window->m_dataModel->graphicsView()->mainRenderer()->SetBackground(m_newColor);
	}
private:
	double m_oldColor[3];
	double m_newColor[3];
	Post2dBirdEyeWindow* m_window;
};

void Post2dBirdEyeWindow::editBackgroundColor()
{
	double vtkOldColor[3];

	m_dataModel->graphicsView()->mainRenderer()->GetBackground(vtkOldColor);
	QColor oldcolor;
	iRIC::VTKColorToQColor(vtkOldColor, oldcolor);
	QColor newcolor = QColorDialog::getColor(oldcolor, this, tr("Background Color"));
	if (! newcolor.isValid()) {return;}
	double vtkNewColor[3];
	iRIC::QColorToVTKColor(newcolor, vtkNewColor);
	iRICUndoStack::instance().push(new Post2dBirdEyeWindowEditBackgroundColorCommand(vtkOldColor, vtkNewColor, this));
}

/*
bool Post2dBirdEyeWindow::exportParticles(const QString &filename, double time, const QString& zonename)
{
	Post2dBirdEyeWindowRootDataItem* rItem = dynamic_cast<Post2dBirdEyeWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post2dBirdEyeWindowZoneDataItem* zItem = rItem->zoneDataItem(zonename);
	Post2dBirdEyeWindowNodeVectorParticleGroupDataItem* pItem = zItem->particleDataItem();
	return pItem->exportParticles(filename, time);
}
*/

/*
QList<QString> Post2dBirdEyeWindow::particleDrawingZones()
{
	QList<QString> ret;
	Post2dBirdEyeWindowRootDataItem* rItem = dynamic_cast<Post2dBirdEyeWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	QList<Post2dBirdEyeWindowGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	for (int i = 0; i < gtItems.count(); ++i){
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = gtItems.at(i);
		QList<Post2dBirdEyeWindowZoneDataItem*> zItems = gtItem->zoneDatas();
		for (int j = 0; j < zItems.count(); ++j){
			Post2dBirdEyeWindowZoneDataItem* zItem = zItems.at(j);
			Post2dBirdEyeWindowNodeVectorParticleGroupDataItem* pItem = zItem->particleDataItem();
			if (pItem->standardItem()->checkState() == Qt::Checked && pItem->currentSolution() != ""){
				ret.append(zItem->zoneName());
			}
		}
	}
	return ret;
}
*/

void Post2dBirdEyeWindow::editZScale()
{
	m_dataModel->editZScale();
}

const QColor Post2dBirdEyeWindow::backgroundColor() const
{
	double vtkColor[3];
	m_dataModel->graphicsView()->mainRenderer()->GetBackground(vtkColor);
	QColor qColor;
	iRIC::VTKColorToQColor(vtkColor, qColor);
	return qColor;
}

void Post2dBirdEyeWindow::setBackgroundColor(QColor& c)
{
	double vtkColor[3];
	iRIC::QColorToVTKColor(c, vtkColor);
	m_dataModel->graphicsView()->mainRenderer()->SetBackground(vtkColor);
}

ObjectBrowser* Post2dBirdEyeWindow::objectBrowser() const
{
	return m_objectBrowser;
}
