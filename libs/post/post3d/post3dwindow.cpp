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

#include <QAction>
#include <QColorDialog>
#include <QLabel>
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

vtkRenderWindow* Post3dWindow::getVtkRenderWindow()
{
	return m_dataModel->graphicsView()->mainRenderer()->GetRenderWindow();
}

QList<QMenu*> Post3dWindow::getAdditionalMenus()
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}

ObjectBrowser* Post3dWindow::objectBrowser()
{
	return m_objectBrowser;
}

void Post3dWindow::cameraFit()
{
	m_dataModel->fit();
}

void Post3dWindow::cameraResetRotation()
{
	m_dataModel->resetRotation();
}

void Post3dWindow::cameraRotate90()
{
	m_dataModel->rotate90();
}

void Post3dWindow::cameraZoomIn()
{
	m_dataModel->zoomIn();
}

void Post3dWindow::cameraZoomOut()
{
	m_dataModel->zoomOut();
}

void Post3dWindow::cameraMoveLeft()
{
	m_dataModel->moveLeft();
}

void Post3dWindow::cameraMoveRight()
{
	m_dataModel->moveRight();
}

void Post3dWindow::cameraMoveUp()
{
	m_dataModel->moveUp();
}

void Post3dWindow::cameraMoveDown()
{
	m_dataModel->moveDown();
}

void Post3dWindow::cameraXYPlane()
{
	m_dataModel->toXYPlane();
}

void Post3dWindow::cameraYZPlane()
{
	m_dataModel->toYZPlane();
}

void Post3dWindow::cameraZXPlane()
{
	m_dataModel->toZXPlane();
}

class Post3dWindowEditBackgroundColorCommand : public QUndoCommand
{
public:
	Post3dWindowEditBackgroundColorCommand(double oldc[3], double newc[3], Post3dWindow* w)
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
	Post3dWindow* m_window;
};

void Post3dWindow::editBackgroundColor()
{
	double vtkOldColor[3];

	m_dataModel->graphicsView()->mainRenderer()->GetBackground(vtkOldColor);
	QColor oldcolor;
	iRIC::VTKColorToQColor(vtkOldColor, oldcolor);
	QColor newcolor = QColorDialog::getColor(oldcolor, this, tr("Background Color"));
	if (! newcolor.isValid()) {return;}
	double vtkNewColor[3];
	iRIC::QColorToVTKColor(newcolor, vtkNewColor);
	iRICUndoStack::instance().push(new Post3dWindowEditBackgroundColorCommand(vtkOldColor, vtkNewColor, this));
}

void Post3dWindow::editZScale()
{
	m_dataModel->editZScale();
}

bool Post3dWindow::exportParticles(const QString& filename, int fileIndex, double time, const QString& zonename)
{
	Post3dWindowRootDataItem* rItem = dynamic_cast<Post3dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post3dWindowZoneDataItem* zItem = rItem->zoneDataItem(zonename);
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
			if (pItem->standardItem()->checkState() == Qt::Checked && pItem->currentSolution() != "") {
				ret.append(zItem->zoneName());
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

const QColor Post3dWindow::backgroundColor() const
{
	double vtkColor[3];
	m_dataModel->graphicsView()->mainRenderer()->GetBackground(vtkColor);
	QColor qColor;
	iRIC::VTKColorToQColor(vtkColor, qColor);
	return qColor;
}

void Post3dWindow::setBackgroundColor(QColor& c)
{
	double vtkColor[3];
	iRIC::QColorToVTKColor(c, vtkColor);
	m_dataModel->graphicsView()->mainRenderer()->SetBackground(vtkColor);
}
