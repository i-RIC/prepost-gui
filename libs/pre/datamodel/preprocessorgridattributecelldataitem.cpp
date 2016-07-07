#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "preprocessorgridattributecelldataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"

#include <guibase/widget/contoursettingwidget.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditdialog.h>
#include <guicore/pre/gridcond/gridcellattributepropertydialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorseditwidget.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QVector>
#include <QXmlStreamWriter>

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCollectionIterator.h>
#include <vtkRenderer.h>

#include <string>

PreProcessorGridAttributeCellDataItem::PreProcessorGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(cond->name(), cond->caption(), parent),
	m_isCustomModified {"isCustomModified", false},
	m_definingBoundingBox {false},
	m_condition {cond}
{
	m_editValueAction = new QAction(PreProcessorGridAttributeCellDataItem::tr("Edit value..."), this);
	m_editValueAction->setDisabled(true);
	connect(m_editValueAction, SIGNAL(triggered()), this, SLOT(editValue()));

	m_editVariationAction = new QAction(PreProcessorGridAttributeCellDataItem::tr("Edit value by specifying variation..."), this);
	m_editVariationAction->setDisabled(true);
	connect(m_editVariationAction, SIGNAL(triggered()), this, SLOT(editVariation()));
}

PreProcessorGridAttributeCellDataItem::~PreProcessorGridAttributeCellDataItem()
{}

QDialog* PreProcessorGridAttributeCellDataItem::propertyDialog(QWidget* p)
{
	PreProcessorGridAttributeCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent());
	ScalarsToColorsEditWidget* stcWidget = m_condition->createScalarsToColorsEditWidget(0);
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(m_condition->name());
	stcWidget->setContainer(stc);
	GridCellAttributePropertyDialog* dialog = new GridCellAttributePropertyDialog(p);
	dialog->setOpacityPercent(gitem->opacityPercent());
	dialog->setScalarsToColorsEditWidget(stcWidget);
	dialog->setWindowTitle(tr("Grid Cell Attribute Display Setting"));
	return dialog;
}

void PreProcessorGridAttributeCellDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridCellAttributePropertyDialog* dialog = dynamic_cast<GridCellAttributePropertyDialog*>(propDialog);

	PreProcessorGridAttributeCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent());
	gitem->setOpacityPercent(dialog->opacityPercent());
	gitem->updateActorSettings();
}

void PreProcessorGridAttributeCellDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCustomModified.load(node);
}

void PreProcessorGridAttributeCellDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g != nullptr) {
		GridAttributeContainer* cont = g->gridAttribute(m_condition->name());
		m_isCustomModified = cont->isCustomModified();
		m_isCustomModified.save(writer);
	}
}

void PreProcessorGridAttributeCellDataItem::loadFromCgnsFile(const int /*fn*/)
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g == nullptr) {return;}
	GridAttributeContainer* cont = g->gridAttribute(m_condition->name());
	cont->setCustomModified(m_isCustomModified);
}

void PreProcessorGridAttributeCellDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMouseMoveEvent(event, v);
	} else {
		dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->updateAttributeBrowser(QPoint(event->x(), event->y()), v);
	}
}

void PreProcessorGridAttributeCellDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		m_definingBoundingBox = true;
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMousePressEvent(event, v);
	}
}

void PreProcessorGridAttributeCellDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			tmpparent->cellSelectingMouseReleaseEvent(event, v);
		}
		m_definingBoundingBox = false;
		dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->fixAttributeBrowser(QPoint(event->x(), event->y()), v);
		v->setCursor(tmpparent->normalCursor());
	} else if (event->button() == Qt::RightButton) {
		PreProcessorGridAttributeCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent());
		delete menu;
		menu = new QMenu(projectData()->mainWindow());
		menu->addAction(m_editValueAction);
		m_editValueAction->setEnabled(tmpparent->selectedCells().count() > 0);
		if (! m_condition->isOption() && dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition) == nullptr) {
			menu->addAction(m_editVariationAction);
			m_editVariationAction->setEnabled(tmpparent->selectedCells().count() > 0);
		}
		menu->addSeparator();
		menu->addAction(gitem->showAttributeBrowserAction());
		menu->move(event->globalPos());
		menu->show();
	}
}

void PreProcessorGridAttributeCellDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->cellSelectingKeyPressEvent(event, v);
}

void PreProcessorGridAttributeCellDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->cellSelectingKeyReleaseEvent(event, v);
}

void PreProcessorGridAttributeCellDataItem::addCustomMenuItems(QMenu* menu)
{
	PreProcessorGridAttributeCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent());
	menu->addAction(gitem->showAttributeBrowserAction());
}

void PreProcessorGridAttributeCellDataItem::editValue()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridAttributeEditDialog* dialog = m_condition->editDialog(mainWindow());
	dialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid cells.")).arg(m_condition->caption()));
	PreProcessorGridTypeDataItem* tItem =
		dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	PreProcessorGeoDataGroupDataItemInterface* i = tItem->geoDataTop()->groupDataItem(m_condition->name());
	i->setupEditWidget(dialog->widget());
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	QVector<vtkIdType> targets = tmpparent->selectedCells();
	Grid* g = tmpparent->grid();
	dialog->scanAndSetDefault(g->gridAttribute(m_condition->name()), targets);

	if (QDialog::Accepted == dialog->exec()) {
		dialog->applyValue(g->gridAttribute(m_condition->name()), targets, g->vtkGrid()->GetCellData(), tmpparent);
	}
	delete dialog;
}

void PreProcessorGridAttributeCellDataItem::editVariation()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridAttributeVariationEditDialog* dialog = m_condition->variationEditDialog(mainWindow());
	dialog->setWindowTitle(QString(tr("Apply variation to %1").arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the variation of %1 at the selected grid nodes.")).arg(m_condition->caption()));
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	QVector<vtkIdType> targets = tmpparent->selectedCells();
	Grid* g = tmpparent->grid();

	if (QDialog::Accepted == dialog->exec()) {
		dialog->applyVariation(g->gridAttribute(m_condition->name()), targets, g->vtkGrid()->GetCellData(), tmpparent);
	}
	delete dialog;
}

void PreProcessorGridAttributeCellDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(true);
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->initAttributeBrowser();
	updateVisibility();
}

void PreProcessorGridAttributeCellDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(false);
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->clearAttributeBrowser();
}

void PreProcessorGridAttributeCellDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->informDataChange(m_condition->name());
}

bool PreProcessorGridAttributeCellDataItem::addToolBarButtons(QToolBar* toolbar)
{
	PreProcessorDataItem* item =
		dynamic_cast<PreProcessorDataItem*>(parent());
	return item->addToolBarButtons(toolbar);
}
