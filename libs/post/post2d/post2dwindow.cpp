#include "datamodel/post2dwindowgridtypedataitem.h"
#include "datamodel/post2dwindownodescalargroupdataitem.h"
#include "datamodel/post2dwindownodescalargrouptopdataitem.h"
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
#include <guibase/widget/itemselectingdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QColorDialog>
#include <QLabel>
#include <QMessageBox>
#include <QStandardItem>
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

	m_icon = QIcon(":/libs/post/post2d/images/iconVis2D.svg");
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
	pixmap.setDevicePixelRatio(devicePixelRatioF());

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

QList<QString> Post2dWindow::particleDrawingZones()
{
	QList<QString> ret;
	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	QList<Post2dWindowGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	for (int i = 0; i < gtItems.count(); ++i) {
		Post2dWindowGridTypeDataItem* gtItem = gtItems.at(i);
		for (auto zItem : gtItem->zoneDatas()) {
			Post2dWindowNodeVectorParticleGroupDataItem* pItem = zItem->particleDataItem();
			if (pItem->standardItem()->checkState() == Qt::Checked && pItem->target() != "") {
				ret.append(zItem->zoneName().c_str());
			}
		}
	}
	return ret;
}

bool Post2dWindow::checkShapeExportCondition(const std::string& zoneName) const
{
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	auto sItem = rItem->zoneDataItem(zoneName)->scalarGroupTopDataItem();
	auto scalars = sItem->selectedScalars();
	if (scalars.size() == 0) {
		QMessageBox::warning(window(), tr("Error"), tr("No contours have been defined"));
		return false;
	} else if (scalars.size() == 1) {
		m_exportScalarName = scalars.at(0).c_str();
	} if (scalars.size() > 1) {
		ItemSelectingDialog dialog(sItem->mainWindow());
		dialog.setMessage(tr("Please select which scalar to export:"));
		QStringList qstringScalars;
		for (const auto& s : scalars) {
			qstringScalars.append(s.c_str());
		}
		dialog.setItems(qstringScalars);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {
			return false;
		}
		m_exportScalarName = scalars.at(dialog.selectedIndex()).c_str();
	}
	return sItem->checkShapeExportCondition(m_exportScalarName);
}

bool Post2dWindow::checkKmlExportCondition(const std::string& zoneName) const
{
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	auto sItem = rItem->zoneDataItem(zoneName)->scalarGroupTopDataItem();
	auto scalars = sItem->selectedScalars();
	if (scalars.size() == 0) {
		QMessageBox::warning(window(), tr("Error"), tr("No Contour Figure is drawn now."));
		return false;
	} else if (scalars.size() == 1) {
		m_exportScalarName = scalars.at(0);
	} if (scalars.size() > 1) {
		ItemSelectingDialog dialog(sItem->mainWindow());
		dialog.setMessage(tr("Please select which scalar to export:"));
		dialog.setItems(scalars);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {
			return false;
		}
		m_exportScalarName = scalars.at(dialog.selectedIndex());
	}
	return sItem->checkKmlExportCondition(m_exportScalarName);
}

bool Post2dWindow::exportKMLHeader(QXmlStreamWriter& writer, const std::string& zonename)
{
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	auto sItem = rItem->zoneDataItem(zonename)->scalarGroupTopDataItem();
	return sItem->exportKMLHeader(writer, m_exportScalarName);
}

bool Post2dWindow::exportKMLFooter(QXmlStreamWriter& writer, const std::string& zoneName)
{
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	auto sItem = rItem->zoneDataItem(zoneName)->scalarGroupTopDataItem();
	return sItem->exportKMLFooter(writer, m_exportScalarName);
}

bool Post2dWindow::exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const std::string& zoneName, bool oneShot)
{
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	auto sItem = rItem->zoneDataItem(zoneName)->scalarGroupTopDataItem();
	return sItem->exportKMLForTimestep(writer, m_exportScalarName, index, time, oneShot);
}

bool Post2dWindow::isAutoParticleOutput() const
{
	bool output = false;
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	for (auto tItem : rItem->gridTypeDataItems()) {
		for (auto zItem : tItem->zoneDatas()) {
			auto pItem = zItem->particleDataItem();
			if (pItem == nullptr) {continue;}

			output = output || pItem->isOutput();
		}
	}
	return output;
}

void Post2dWindow::updateTmsList()
{
	dynamic_cast<Post2dWindowDataModel*> (m_dataModel)->updateTmsList();
}

bool Post2dWindow::exportContourFigureToShape(const QString& filePrefix, int index, double time, const std::string& zoneName)
{
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	auto sItem = rItem->zoneDataItem(zoneName)->scalarGroupTopDataItem();
	QString filePrefix2 = filePrefix;
	filePrefix2.append(QString("%1.shp").arg(index));
	return sItem->exportContourFigureToShape(m_exportScalarName, filePrefix2, time);
}

std::vector<std::string> Post2dWindow::discreteColorDrawingZones()
{
	std::vector<std::string> ret;
	auto rItem = dynamic_cast<Post2dWindowRootDataItem*>(m_dataModel->m_rootDataItem);
	auto gtItems = rItem->gridTypeDataItems();
	for (int i = 0; i < gtItems.size(); ++i) {
		auto zItems = gtItems.at(i)->zoneDatas();
		for (int j = 0; j < zItems.size(); ++j) {
			auto zItem = zItems.at(j);
			auto sItem = zItem->scalarGroupTopDataItem();
			for (auto item : sItem->childItems()) {
				auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
				if (typedi->standardItem()->checkState() == Qt::Checked) {
					ret.push_back(zItem->zoneName());
				}
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

void Post2dWindow::applyOffset(double x_diff, double y_diff)
{
	m_dataModel->applyOffset(x_diff, y_diff);
}
