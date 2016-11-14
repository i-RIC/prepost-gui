#include "datamodel/post2dwindowgridtypedataitem.h"
#include "datamodel/post2dwindownodescalargroupdataitem.h"
#include "datamodel/post2dwindownodevectorparticlegroupdataitem.h"
#include "datamodel/post2dwindowrootdataitem.h"
#include "datamodel/post2dwindowzonedataitem.h"
#include "post2dobjectbrowser.h"
#include "post2dpropertybrowser.h"
#include "post2dwindow.h"
#include "post2dwindowactionmanager.h"
#include "post2dwindowdatamodel.h"
#include "post2dwindowdatamodel.h"
#include "post2dwindowgraphicsview.h"
#include "post2dwindowprojectdataitem.h"

#include <guibase/colortool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QColorDialog>
#include <QLabel>
#include <QToolBar>
#include <QUndoCommand>

#include <vtkRenderer.h>

Post2dWindow::Post2dWindow(QWidget* parent, int index, Post2dWindowProjectDataItem* pdi)
	: PostProcessorWindow(parent, index, pdi)
{
	init();
}

Post2dWindow::~Post2dWindow()
{
	delete m_dataModel;
}

void Post2dWindow::init()
{
	setWindowTitle(tr("Post-processing (2D): %1").arg(m_index));

	m_icon = QIcon(":/libs/post/post2d/images/iconVis2D.png");
	// set default central widget.
	QLabel* label = new QLabel(this);
	setCentralWidget(label);

	m_actionManager = new Post2dWindowActionManager(this);

	m_objectBrowser = new Post2dObjectBrowser(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);

	m_propertyBrowser = new Post2dPropertyBrowser(this);
	m_propertyBrowser->hide();
	addDockWidget(Qt::LeftDockWidgetArea, m_propertyBrowser);

	connect(postSolutionInfo(), SIGNAL(updated()), this, SLOT(update()));

	m_initialState = saveState();
}

void Post2dWindow::setupDefaultGeometry(int index)
{
	QWidget* parent = parentWidget();
	if (parent->isMaximized()) {return;}
	parent->move(index * 30, index * 30);
	parent->resize(700, 500);
	m_propertyBrowser->hide();
	restoreState(m_initialState);
}

QPixmap Post2dWindow::snapshot()
{
	Post2dWindowGraphicsView* view = m_dataModel->graphicsView();
	QImage img = view->getImage();
	QPixmap pixmap = QPixmap::fromImage(img);
	if (m_isTransparent) { makeBackgroundTransparent(view, pixmap); }

	return pixmap;
}

vtkRenderWindow* Post2dWindow::getVtkRenderWindow() const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (m_dataModel->graphicsView()->mainRenderer());
	return r->GetRenderWindow();
}

QList<QMenu*> Post2dWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	menus.append(m_actionManager->measuredDataMenu());
	return menus;
}

QToolBar* Post2dWindow::getAdditionalToolBar() const
{
	return nullptr;
}

void Post2dWindow::cameraFit()
{
	m_dataModel->graphicsView()->cameraFit();
}

void Post2dWindow::cameraResetRotation()
{
	m_dataModel->graphicsView()->cameraResetRotation();
}

void Post2dWindow::cameraRotate90()
{
	m_dataModel->graphicsView()->cameraRotate90();
}

void Post2dWindow::cameraZoomIn()
{
	m_dataModel->graphicsView()->cameraZoomIn();
}

void Post2dWindow::cameraZoomOut()
{
	m_dataModel->graphicsView()->cameraZoomOut();
}

void Post2dWindow::cameraMoveLeft()
{
	m_dataModel->graphicsView()->cameraMoveLeft();
}

void Post2dWindow::cameraMoveRight()
{
	m_dataModel->graphicsView()->cameraMoveRight();
}

void Post2dWindow::cameraMoveUp()
{
	m_dataModel->graphicsView()->cameraMoveUp();
}

void Post2dWindow::cameraMoveDown()
{
	m_dataModel->graphicsView()->cameraMoveDown();
}

ObjectBrowser* Post2dWindow::objectBrowser() const
{
	return m_objectBrowser;
}

void Post2dWindow::editBackgroundColor()
{
	BackgroundColorEditInterface::editBackgroundColor(this);
}

void Post2dWindow::editZScale()
{
	m_dataModel->editZScale();
}

bool Post2dWindow::exportParticles(const QString& filePrefix, int fileIndex, double time, const QString& zoneName)
{
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post2dWindowZoneDataItem* zItem = rItem->zoneDataItem(iRIC::toStr(zoneName));
	Post2dWindowNodeVectorParticleGroupDataItem* pItem = zItem->particleDataItem();
	return pItem->exportParticles(filePrefix, fileIndex, time);
}

QList<QString> Post2dWindow::contourFigureDrawingZones()
{
	QList<QString> ret;
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	QList<Post2dWindowGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	for (int i = 0; i < gtItems.count(); ++i) {
		Post2dWindowGridTypeDataItem* gtItem = gtItems.at(i);
		QList<Post2dWindowZoneDataItem*> zItems = gtItem->zoneDatas();
		for (Post2dWindowZoneDataItem* zItem : zItems) {
			Post2dWindowNodeScalarGroupDataItem* gItem = zItem->scalarGroupDataItem();
			if (gItem->contour() == ContourSettingWidget::ContourFigure) {
				ret.append(zItem->zoneName().c_str());
			}
		}
	}
	return ret;
}

QList<QString> Post2dWindow::particleDrawingZones()
{
	QList<QString> ret;
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	QList<Post2dWindowGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	for (int i = 0; i < gtItems.count(); ++i) {
		Post2dWindowGridTypeDataItem* gtItem = gtItems.at(i);
		QList<Post2dWindowZoneDataItem*> zItems = gtItem->zoneDatas();
		for (int j = 0; j < zItems.count(); ++j) {
			Post2dWindowZoneDataItem* zItem = zItems.at(j);
			Post2dWindowNodeVectorParticleGroupDataItem* pItem = zItem->particleDataItem();
			if (pItem->standardItem()->checkState() == Qt::Checked && pItem->target() != "") {
				ret.append(zItem->zoneName().c_str());
			}
		}
	}
	return ret;
}

bool Post2dWindow::checkKmlExportCondition(const QString& zoneName) const
{
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post2dWindowZoneDataItem* zItem = rItem->zoneDataItem(iRIC::toStr(zoneName));
	Post2dWindowNodeScalarGroupDataItem* sItem = zItem->scalarGroupDataItem();
	return sItem->checkKmlExportCondition();
}

bool Post2dWindow::exportKMLHeader(QXmlStreamWriter& writer, const QString& zonename)
{
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post2dWindowZoneDataItem* zItem = rItem->zoneDataItem(iRIC::toStr(zonename));
	Post2dWindowNodeScalarGroupDataItem* sItem = zItem->scalarGroupDataItem();
	return sItem->exportKMLHeader(writer);
}

bool Post2dWindow::exportKMLFooter(QXmlStreamWriter& writer, const QString& zoneName)
{
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post2dWindowZoneDataItem* zItem = rItem->zoneDataItem(iRIC::toStr(zoneName));
	Post2dWindowNodeScalarGroupDataItem* sItem = zItem->scalarGroupDataItem();
	return sItem->exportKMLFooter(writer);
}

bool Post2dWindow::exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const QString& zoneName)
{
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post2dWindowZoneDataItem* zItem = rItem->zoneDataItem(iRIC::toStr(zoneName));
	Post2dWindowNodeScalarGroupDataItem* sItem = zItem->scalarGroupDataItem();
	return sItem->exportKMLForTimestep(writer, index, time);
}

void Post2dWindow::updateTmsList()
{
	dynamic_cast<Post2dWindowDataModel*> (m_dataModel)->updateTmsList();
}

bool Post2dWindow::exportContourFigureToShape(const QString& filePrefix, int index, double time, const QString& zoneName)
{
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	Post2dWindowZoneDataItem* zItem = rItem->zoneDataItem(iRIC::toStr(zoneName));
	Post2dWindowNodeScalarGroupDataItem* sItem = zItem->scalarGroupDataItem();
	QString filePrefix2 = filePrefix;
	filePrefix2.append(QString("%1.shp").arg(index));
	return sItem->exportContourFigureToShape(filePrefix2, time);
}

QList<QString> Post2dWindow::contourDrawingZones()
{
	QList<QString> ret;
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	QList<Post2dWindowGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	for (int i = 0; i < gtItems.count(); ++i) {
		Post2dWindowGridTypeDataItem* gtItem = gtItems.at(i);
		QList<Post2dWindowZoneDataItem*> zItems = gtItem->zoneDatas();
		for (int j = 0; j < zItems.count(); ++j) {
			Post2dWindowZoneDataItem* zItem = zItems.at(j);
			Post2dWindowNodeScalarGroupDataItem* sItem = zItem->scalarGroupDataItem();
			if (sItem->standardItem()->checkState() == Qt::Checked && sItem->target() != "") {
				ret.append(zItem->zoneName().c_str());
			}
		}
	}
	return ret;
}

bool Post2dWindow::hasTransparentPart()
{
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	return rItem->hasTransparentPart();
}

VTKGraphicsView* Post2dWindow::viewForBackgroundColor() const
{
	return m_dataModel->graphicsView();
}
