#include "preprocessorgridrelatedconditioncelldataitem.h"
#include "preprocessorgridrelatedconditioncellgroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorrawdatatopdataitem.h"
#include "preprocessorrawdatagroupdataitem.h"
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcomplexcondition.h>
#include <guibase/contoursettingwidget.h>
#include <guicore/scalarstocolors/scalarstocolorseditwidget.h>
#include "preprocessorgriddataitem.h"
#include "../preprocessorwindow.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/gridrelatedcellconditionpropertydialog.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioneditdialog.h>
#include <guicore/pre/gridcond/base/gridrelatedconditionvariationeditdialog.h>
#include "../preprocessorgraphicsview.h"
#include <misc/xmlsupport.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <misc/stringtool.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>

#include <QStandardItem>
#include <QXmlStreamWriter>
#include <QDomNode>
#include <QVector>
#include <QMouseEvent>
#include <QMenu>

#include <vtkCollectionIterator.h>
#include <vtkRenderer.h>
#include <vtkCellData.h>
#include <vtkCell.h>
#include <string>

PreProcessorGridRelatedConditionCellDataItem::PreProcessorGridRelatedConditionCellDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent)
	: PreProcessorDataItem(cond->caption(), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;
	m_definingBoundingBox = false;
	m_condition = cond;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	connect(this, SIGNAL(changed(PreProcessorGridRelatedConditionCellDataItem*)),
					parent, SLOT(exclusivelyCheck(PreProcessorGridRelatedConditionCellDataItem*)));

	m_editValueAction = new QAction(tr("Edit value..."), this);
	m_editValueAction->setDisabled(true);
	connect(m_editValueAction, SIGNAL(triggered()), this, SLOT(editValue()));

	m_editVariationAction = new QAction(tr("Edit value by specifying variation..."), this);
	m_editVariationAction->setDisabled(true);
	connect(m_editVariationAction, SIGNAL(triggered()), this, SLOT(editVariation()));
}

void PreProcessorGridRelatedConditionCellDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

QDialog* PreProcessorGridRelatedConditionCellDataItem::propertyDialog(QWidget* p)
{
	PreProcessorGridRelatedConditionCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent());
	ScalarsToColorsEditWidget* stcWidget = m_condition->createScalarsToColorsEditWidget(0);
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(iRIC::toStr(m_condition->name()).c_str());
	stcWidget->setContainer(stc);
	GridRelatedCellConditionPropertyDialog* dialog = new GridRelatedCellConditionPropertyDialog(p);
	dialog->setOpacityPercent(gitem->opacityPercent());
	dialog->setScalarsToColorsEditWidget(stcWidget);
	dialog->setWindowTitle(tr("Grid Cell Attribute Display Setting"));
	return dialog;
}

void PreProcessorGridRelatedConditionCellDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridRelatedCellConditionPropertyDialog* dialog = dynamic_cast<GridRelatedCellConditionPropertyDialog*>(propDialog);

	PreProcessorGridRelatedConditionCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent());
	gitem->setOpacityPercent(dialog->opacityPercent());
	gitem->updateActorSettings();
}

void PreProcessorGridRelatedConditionCellDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCustomModified = static_cast<bool>(node.toElement().attribute("isCustomModified", "0").toInt());
}

void PreProcessorGridRelatedConditionCellDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString mod;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g != nullptr) {
		GridRelatedConditionContainer* cont = g->gridRelatedCondition(m_condition->name());
		mod.setNum(static_cast<int>(cont->isCustomModified()));
		writer.writeAttribute("isCustomModified", mod);
	}
}

void PreProcessorGridRelatedConditionCellDataItem::loadFromCgnsFile(const int /*fn*/)
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g == nullptr) {return;}
	GridRelatedConditionContainer* cont = g->gridRelatedCondition(m_condition->name());
	cont->setCustomModified(m_isCustomModified);
}

void PreProcessorGridRelatedConditionCellDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMouseMoveEvent(event, v);
	} else {
		dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent())->updateAttributeBrowser(QPoint(event->x(), event->y()), v);
	}
	/*
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		if (tmpparent->grid()->isMasked()){
			v->setCursor(Qt::ForbiddenCursor);
		} else {
			v->setCursor(Qt::ArrowCursor);
		}
	*/
}

void PreProcessorGridRelatedConditionCellDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		m_definingBoundingBox = true;
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMousePressEvent(event, v);
	}
}

void PreProcessorGridRelatedConditionCellDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			tmpparent->cellSelectingMouseReleaseEvent(event, v);
		}
		m_definingBoundingBox = false;
		dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent())->fixAttributeBrowser(QPoint(event->x(), event->y()), v);
		v->setCursor(tmpparent->normalCursor());
	} else if (event->button() == Qt::RightButton) {
		PreProcessorGridRelatedConditionCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent());
		if (menu != nullptr) {delete menu;}
		menu = new QMenu(projectData()->mainWindow());
		menu->addAction(m_editValueAction);
		m_editValueAction->setEnabled(tmpparent->selectedCells().count() > 0);
		if (! m_condition->isOption() && dynamic_cast<SolverDefinitionGridRelatedComplexCondition*>(m_condition) == nullptr) {
			menu->addAction(m_editVariationAction);
			m_editVariationAction->setEnabled(tmpparent->selectedCells().count() > 0);
		}
		menu->addSeparator();
		menu->addAction(gitem->showAttributeBrowserAction());
		menu->move(event->globalPos());
		menu->show();
	}
}

void PreProcessorGridRelatedConditionCellDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->cellSelectingKeyPressEvent(event, v);
}

void PreProcessorGridRelatedConditionCellDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->cellSelectingKeyReleaseEvent(event, v);
}

void PreProcessorGridRelatedConditionCellDataItem::addCustomMenuItems(QMenu* menu)
{
	PreProcessorGridRelatedConditionCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent());
	menu->addAction(gitem->showAttributeBrowserAction());
}

void PreProcessorGridRelatedConditionCellDataItem::editValue()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridRelatedConditionEditDialog* dialog = m_condition->editDialog(preProcessorWindow());
	dialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid cells.")).arg(m_condition->caption()));
	PreProcessorGridTypeDataItem* tItem =
		dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	PreProcessorRawDataGroupDataItemInterface* i = tItem->rawdataTop()->groupDataItem(m_condition->name());
	i->setupEditWidget(dialog->widget());
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	QVector<vtkIdType> targets = tmpparent->selectedCells();
	Grid* g = tmpparent->grid();
	dialog->scanAndSetDefault(g->gridRelatedCondition(m_condition->name()), targets);

	if (QDialog::Accepted == dialog->exec()) {
		dialog->applyValue(g->gridRelatedCondition(m_condition->name()), targets, g->vtkGrid()->GetCellData(), tmpparent);
	}
	delete dialog;
}

void PreProcessorGridRelatedConditionCellDataItem::editVariation()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridRelatedConditionVariationEditDialog* dialog = m_condition->variationEditDialog(preProcessorWindow());
	dialog->setWindowTitle(QString(tr("Apply variation to %1").arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the variation of %1 at the selected grid nodes.")).arg(m_condition->caption()));
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	QVector<vtkIdType> targets = tmpparent->selectedCells();
	Grid* g = tmpparent->grid();

	if (QDialog::Accepted == dialog->exec()) {
		dialog->applyVariation(g->gridRelatedCondition(m_condition->name()), targets, g->vtkGrid()->GetCellData(), tmpparent);
	}
	delete dialog;
}

void PreProcessorGridRelatedConditionCellDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(true);
	dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent())->initAttributeBrowser();
	updateVisibility();
}

void PreProcessorGridRelatedConditionCellDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(false);
	dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent())->clearAttributeBrowser();
}

void PreProcessorGridRelatedConditionCellDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGridRelatedConditionCellGroupDataItem*>(parent())->informDataChange(m_condition->name());
}


bool PreProcessorGridRelatedConditionCellDataItem::addToolBarButtons(QToolBar* toolbar)
{
	PreProcessorDataItem* item =
		dynamic_cast<PreProcessorDataItem*>(parent());
	return item->addToolBarButtons(toolbar);
}
