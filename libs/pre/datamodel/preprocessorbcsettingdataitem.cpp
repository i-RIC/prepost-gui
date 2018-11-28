#include "../preprocessorgraphicsview.h"
#include "preprocessorbcdataitem.h"
#include "preprocessorbcsettingdataitem.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>
#include <geodata/polygon/geodatapolygonabstractpolygon.h>
#include <geodata/polygon/geodatapolygonregionpolygon.h>

#include <vtkCell.h>

#include <QAction>
#include <QFile>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>

PreProcessorBCSettingDataItem::PreProcessorBCSettingDataItem(PreProcessorBCDataItem* item, GraphicsWindowDataItem* parent) :
	PreProcessorGeoDataDataItemInterface {item->standardItem()->text(), QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_bcDataItem {item}
{
	if (item->hideSetting()) {
		QStandardItem* p = m_standardItem->parent();
		p->removeRow(m_standardItem->row());
		m_standardItem = nullptr;
	} else {
		setupStandardItem(Checked, NotReorderable, Deletable);
		setIsCommandExecuting(true);
		m_standardItem->setEditable(true);
		setIsCommandExecuting(false);
	}
	m_editAction = new QAction(PreProcessorBCSettingDataItem::tr("&Edit Condition..."), this);

	m_polygon = new GeoDataPolygon(this, 0, 0);
	m_polygon->setupMenu();
	m_polygon->setMapping(GeoDataPolygonColorSettingDialog::Arbitrary);

	m_rightClickingMenu = new QMenu(nullptr);
	m_rightClickingMenu->setTitle(PreProcessorBCSettingDataItem::tr("Polygon"));
	m_rightClickingMenu->addAction(m_editAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_polygon->addVertexAction());
	m_rightClickingMenu->addAction(m_polygon->removeVertexAction());
	m_rightClickingMenu->addAction(m_polygon->coordEditAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_polygon->holeModeAction());
	m_rightClickingMenu->addAction(m_polygon->deleteAction());

	PreProcessorBCSettingGroupDataItem* gditem = dynamic_cast<PreProcessorBCSettingGroupDataItem*>(parent);
	connect(item, SIGNAL(itemUpdated()), this, SLOT(updateItem()));
	connect(item, SIGNAL(destroyed()), gditem, SLOT(updateItems()));
	connect(m_editAction, SIGNAL(triggered()), item, SLOT(showDialog()));

	updateFilename();
}

PreProcessorBCSettingDataItem::~PreProcessorBCSettingDataItem()
{
	delete m_polygon;
	delete m_rightClickingMenu;
}

void PreProcessorBCSettingDataItem::loadFromCgnsFile(const int /*fn*/)
{
}

void PreProcessorBCSettingDataItem::saveToCgnsFile(const int /*fn*/)
{
}

void PreProcessorBCSettingDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{
}

void PreProcessorBCSettingDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{
	m_bcDataItem->setMapped(m_polygon->isMapped());
	updateFilename();
}

void PreProcessorBCSettingDataItem::handleStandardItemDoubleClicked()
{
	m_bcDataItem->showDialog();
}

void PreProcessorBCSettingDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
	m_bcDataItem->standardItem()->setText(m_standardItem->text());
	m_bcDataItem->handleStandardItemChange();
}

void PreProcessorBCSettingDataItem::informSelection(VTKGraphicsView* v)
{
	if (m_polygon->selectedPolygon() == nullptr) {
		m_polygon->setSelectMode(GeoDataPolygon::smPolygon);
		m_polygon->setSelectedPolygon(m_polygon->regionPolygon());
	}
	m_polygon->informSelection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorBCSettingDataItem::informDeselection(VTKGraphicsView* v)
{
	m_polygon->informDeselection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorBCSettingDataItem::viewOperationEnded(VTKGraphicsView* v)
{
	m_polygon->viewOperationEnded(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorBCSettingDataItem::mouseDoubleClickEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/)
{
	if (m_polygon->mouseEventMode() != GeoDataPolygon::meDefining) {return;}
	if (m_polygon->selectMode() != GeoDataPolygon::smPolygon) {return;}

	m_polygon->definePolygon(true, true);
	if (m_polygon->selectedPolygon() == m_polygon->regionPolygon()) {
		m_bcDataItem->showDialog();
	}
}

void PreProcessorBCSettingDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_polygon->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorBCSettingDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_polygon->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
	m_dragStartPoint = QPoint(event->x(), event->y());
}

void PreProcessorBCSettingDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (iRIC::isNear(m_dragStartPoint, event->pos()) && event->button() == Qt::RightButton) {
		m_rightClickingMenu->move(event->globalPos());
		m_rightClickingMenu->show();
	} else {
		m_polygon->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
	}
}

void PreProcessorBCSettingDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* /*v*/)
{
	if (! iRIC::isEnterKey(event->key())) {return;}
	if (m_polygon->mouseEventMode() != GeoDataPolygon::meDefining) {return;}
	if (m_polygon->selectMode() != GeoDataPolygon::smPolygon) {return;}

	m_polygon->definePolygon(false, true);
	if (m_polygon->selectedPolygon() == m_polygon->regionPolygon()) {
		m_bcDataItem->showDialog();
	}
}

void PreProcessorBCSettingDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	m_polygon->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorBCSettingDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editAction);
}

void PreProcessorBCSettingDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_polygon->assignActorZValues(range);
}
void PreProcessorBCSettingDataItem::updateItem()
{
	if (! m_bcDataItem->hideSetting()) {
		m_standardItem->setText(m_bcDataItem->standardItem()->text());
	}

	m_polygon->setOpacity(m_bcDataItem->opacityPercent());
	m_polygon->setColor(m_bcDataItem->color());
}

void PreProcessorBCSettingDataItem::loadExternalData(const QString& filename)
{
	if (! QFile::exists(filename)) {return;}
	m_polygon->loadExternalData(filename);
	updateItem();
}

void PreProcessorBCSettingDataItem::saveExternalData(const QString& filename)
{
	m_polygon->saveExternalData(filename);
}

void PreProcessorBCSettingDataItem::updateFilename()
{
	setFilename(QString("%1%2.dat").arg(m_bcDataItem->condition()->name().c_str()).arg(m_bcDataItem->projectNumber()));
}

void PreProcessorBCSettingDataItem::executeMapping(bool noDraw, WaitDialog* dialog)
{
	PreProcessorGridAndGridCreatingConditionDataItem* gccdItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent()->parent());
	PreProcessorGridDataItemInterface* gitem = gccdItem->gridDataItem();
	Grid* grid = gitem->grid();
	if (grid == nullptr && ! noDraw) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("Mapping can not be executed when there is no grid."));
		if (dialog != nullptr) {
			dialog->setProgress(dialog->progress() + 1);
		}
		return;
	}
	if (m_bcDataItem->hideSetting()) {
		// when this is hidden, mapping is not executed.
		return;
	}

	vtkPointSet* pset = grid->vtkGrid();
	vtkPointSet* polygonGrid = m_polygon->polyData();
	double bounds[6];
	polygonGrid->GetBounds(bounds);

	QSet<vtkIdType> vertices;

	for (vtkIdType vid = 0; vid != pset->GetNumberOfPoints(); ++vid) {
		double p[3];
		pset->GetPoint(vid, p);
		if (p[0] < bounds[0] ||
				p[0] > bounds[1] ||
				p[1] < bounds[2] ||
				p[1] > bounds[3]) {
			// this point is not selected.
			continue;
		}
		vtkIdType cellid;
		double pcoords[3];
		double weights[3];
		int subid;
		cellid = polygonGrid->FindCell(p, 0, 0, 1e-4, subid, pcoords, weights);
		bool in = (cellid >= 0);
		if (! in) {
			// not found. but if the grid is ugly, sometimes FindCell is not reliable.
			for (cellid = 0; ! in && cellid < polygonGrid->GetNumberOfCells(); ++cellid) {
				vtkCell* tmpcell = polygonGrid->GetCell(cellid);
				double dist2;
				double closestPoint[3];
				int ret = tmpcell->EvaluatePosition(p, &(closestPoint[0]), subid, pcoords, dist2, weights);
				in = (ret == 1);
			}
		}
		if (in) {
			vertices.insert(vid);
		}
	}
	m_bcDataItem->assignIndices(vertices);
	if (! noDraw) {
		renderGraphicsView();
	}
	m_polygon->setMapped();
}

void PreProcessorBCSettingDataItem::loadData()
{
	ProjectDataItem::loadExternalData();
	if (m_bcDataItem->mapped()) {
		m_polygon->setMapped();
	}
}

void PreProcessorBCSettingDataItem::doApplyOffset(double x, double y)
{
	this->m_polygon->doApplyOffset(x, y);
}
