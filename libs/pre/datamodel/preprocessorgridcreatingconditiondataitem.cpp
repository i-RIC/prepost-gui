#include "../factory/gridcreatingconditionfactory.h"
#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridcreatingconditionalgorithmselectdialog.h"
#include "preprocessorgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"

#include <gridcreatingcondition/centerandwidth/gridcreatingconditioncenterandwidthdialog.h>
#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

PreProcessorGridCreatingConditionDataItem::PreProcessorGridCreatingConditionDataItem(PreProcessorDataItem* dataitem) :
	PreProcessorGridCreatingConditionDataItemInterface {dataitem}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	// create default condition.
	// @todo Currently, the default condition is the algorithm
	// that is found first, that is compatible with the type of grid.
	PreProcessorGridTypeDataItem* gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());

	// setup actions.
	m_createAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("&Create Grid..."), this);
	m_createAction->setDisabled(true);
	connect(m_createAction, SIGNAL(triggered()), this, SLOT(createGrid()));

	m_switchAlgorithmAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("&Select Algorithm to Create Grid..."), this);
	connect(m_switchAlgorithmAction, SIGNAL(triggered()), this, SLOT(switchAlgorithm()));

	m_clearAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("Reset to &Default..."), this);
	connect(m_clearAction, SIGNAL(triggered()), this, SLOT(resetCondition()));
	m_deleteAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("&Delete Grid Creating Condition..."), this);
	m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteCondition()));

	GridCreatingConditionFactory& factory = GridCreatingConditionFactory::instance(iricMainWindow());
	QList<GridCreatingConditionCreator*> cList = factory.compatibleCreators(*(gTypeItem->gridType()));

	// default condition is not set.
	m_condition = nullptr;
	if (cList.count() < 1) {
		// users can not switch algorithmn to create grid.
		m_switchAlgorithmAction->setDisabled(true);
	}
}

PreProcessorGridCreatingConditionDataItem::~PreProcessorGridCreatingConditionDataItem()
{
	if (m_condition) {
		delete m_condition;
	}
}
void PreProcessorGridCreatingConditionDataItem::setCondition(GridCreatingCondition* condition)
{
	m_condition = condition;
}

void PreProcessorGridCreatingConditionDataItem::addCustomMenuItems(QMenu* menu)
{
	if (m_condition == nullptr) {
		menu->addAction(m_switchAlgorithmAction);
		return;
	}
	menu->addAction(m_createAction);
	menu->addSeparator();
	menu->addAction(m_switchAlgorithmAction);
	menu->addSeparator();
	menu->addAction(m_deleteAction);
}

bool PreProcessorGridCreatingConditionDataItem::addToolBarButtons(QToolBar* tb)
{
	if (m_condition == nullptr) {return false;}
	return m_condition->addToolBarButtons(tb);
}

void PreProcessorGridCreatingConditionDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_condition == nullptr) {return;}
	m_condition->assignActorZValues(range);
}

void PreProcessorGridCreatingConditionDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (m_condition != nullptr) {
		delete m_condition;
	}
	m_condition = GridCreatingConditionFactory::instance(iricMainWindow()).restore(node, this);
	m_createAction->setDisabled(true);
	if (m_condition != nullptr) {
		m_condition->setupActors();
		m_condition->informDeselection(dataModel()->graphicsView());
		m_condition->assignActorZValues(m_zDepthRange);
		m_condition->setupMenu();
		connect(m_condition, SIGNAL(gridCreated(Grid*)), this, SLOT(handleNewGrid(Grid*)));
		connect(m_condition, SIGNAL(tmpGridCreated(Grid*)), this, SLOT(handleTmpGrid(Grid*)));
		if (m_condition->init()) {
			m_createAction->setEnabled(true);
		} else {
			// error occured while initializing.
			delete m_condition;
			m_condition = nullptr;
		}
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorGridCreatingConditionDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	if (m_condition != nullptr) {
		writer.writeAttribute("name", m_condition->name());
		m_condition->saveToProjectMainFile(writer);
	}
}

void PreProcessorGridCreatingConditionDataItem::innerUpdate2Ds()
{
	if (m_condition != nullptr) {
		m_condition->update2Ds();
	}
}

void PreProcessorGridCreatingConditionDataItem::createGrid()
{
	if (m_condition == nullptr) {return;}
	PreProcessorGridAndGridCreatingConditionDataItem* tmp_parent = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	PreProcessorGridDataItemInterface* gridDataItem = tmp_parent->gridDataItem();
	// this operation is not possible while the solver is running.
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	if (gridDataItem->grid() != nullptr) {
		if (QMessageBox::No == QMessageBox::warning(
					dataModel()->mainWindow(),
					tr("Warning"),
					tr("This operation will discard the grid that already exists. Are you sure?"),
					QMessageBox::Yes | QMessageBox::No,
					QMessageBox::No)) {
			// The user said no to confirm dialog, so abort.
			return;
		}
	}
	// each algorithmn can select whether to show dialog or not.
	bool ok = m_condition->create(preProcessorWindow());
	if (! ok) {return;}
	PreProcessorBCSettingGroupDataItem* bcsgItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent())->bcSettingGroupDataItem();
	bcsgItem->executeMapping();
	// succeeded.
	iRICUndoStack::instance().clear();
}

void PreProcessorGridCreatingConditionDataItem::silentDeleteCondition()
{
	if (m_condition == nullptr) {return;}
	delete m_condition;
	m_condition = nullptr;
	m_createAction->setDisabled(true);
}

void PreProcessorGridCreatingConditionDataItem::deleteCondition()
{
	int ret = QMessageBox::warning(
							dataModel()->mainWindow(),
							tr("Warning"),
							tr("Are you sure you want to discard the grid creating condition?"),
							QMessageBox::Yes | QMessageBox::No,
							QMessageBox::No);
	if (ret == QMessageBox::No) {return;}
	if (m_condition == nullptr) {return;}
	delete m_condition;
	m_condition = nullptr;
	m_createAction->setDisabled(true);
	renderGraphicsView();

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();
}

void PreProcessorGridCreatingConditionDataItem::resetCondition()
{
	int ret = QMessageBox::information(
							dataModel()->mainWindow(),
							tr("Confirm"),
							tr("Are you sure you want to reset the grid creating condition to the default status?"),
							QMessageBox::Yes | QMessageBox::No,
							QMessageBox::No);
	if (ret == QMessageBox::No) {return;}
	if (m_condition == nullptr) {return;}
	m_condition->clear();
	renderGraphicsView();

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	m_condition->showInitialDialog();
}

void PreProcessorGridCreatingConditionDataItem::handleNewGrid(Grid* newgrid)
{
	if (newgrid == nullptr) {return;}
	PreProcessorGridAndGridCreatingConditionDataItem* tmp_parent = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	tmp_parent->setupGridDataItem(newgrid);
	PreProcessorGridDataItemInterface* gridDataItem = tmp_parent->gridDataItem();
	bool ok = gridDataItem->setGrid(newgrid);
	if (! ok) {
		delete newgrid;
		return;
	}
	emit gridCreated();
}

void PreProcessorGridCreatingConditionDataItem::handleTmpGrid(Grid* tmpgrid)
{
	if (tmpgrid == nullptr) {return;}
	PreProcessorGridAndGridCreatingConditionDataItemInterface* tmp_parent = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	PreProcessorGridDataItemInterface* gridDataItem = tmp_parent->gridDataItem();
	bool ok = gridDataItem->setGrid(tmpgrid);
	if (! ok) {
		delete tmpgrid;
	}
}

void PreProcessorGridCreatingConditionDataItem::moveGrid()
{}

void PreProcessorGridCreatingConditionDataItem::handleStandardItemClicked()
{
	if (m_condition == nullptr) {return;}
	m_condition->handleStandardItemClicked();
}

void PreProcessorGridCreatingConditionDataItem::handleStandardItemDoubleClicked()
{
	if (m_condition == nullptr) {return;}
	m_condition->handleStandardItemDoubleClicked();
}

void PreProcessorGridCreatingConditionDataItem::informSelection(VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->informSelection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::informDeselection(VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->informDeselection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::viewOperationEnded(VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->viewOperationEnded(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->keyPressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->mouseDoubleClickEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_condition == nullptr) {return;}
	m_condition->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

QStringList PreProcessorGridCreatingConditionDataItem::containedFiles()
{
	if (m_condition == nullptr) {return QStringList();}
	return m_condition->containedFiles();
}

QMenu* PreProcessorGridCreatingConditionDataItem::menu()
{
	if (m_condition != nullptr) {
		return m_condition->menu();
	} else {
		return nullptr;
	}
}

void PreProcessorGridCreatingConditionDataItem::switchAlgorithm()
{
	if (m_condition != nullptr && QMessageBox::Cancel == QMessageBox::warning(mainWindow(), tr("Warning"), tr("When you switch algorithm to create grid, the grid and the grid creating condition you created will be discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)) {
		return;
	}
	PreProcessorGridTypeDataItem* gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	GridCreatingConditionFactory& factory = GridCreatingConditionFactory::instance(iricMainWindow());
	QList<GridCreatingConditionCreator*> cList = factory.compatibleCreators(*(gTypeItem->gridType()));
	PreProcessorGridCreatingConditionAlgorithmSelectDialog dialog;
	dialog.setCreators(cList);
	if (m_condition != nullptr) {
		dialog.setCurrent(m_condition->creator());
	}
	if (QDialog::Accepted != dialog.exec()) {
		// canceled.
		return;
	}
	GridCreatingConditionCreator* newCreator = dialog.selectedCreator();
	// Check whether the new creator is really usable
	if (! newCreator->checkAvailability(this)) {
		// not available.
		return;
	}
	GridCreatingCondition* newcond = dialog.selectedCreator()->create(this);
	newcond->setupActors();
	newcond->assignActorZValues(m_zDepthRange);
	newcond->setupMenu();
	connect(newcond, SIGNAL(gridCreated(Grid*)), this, SLOT(handleNewGrid(Grid*)));
	connect(newcond, SIGNAL(tmpGridCreated(Grid*)), this, SLOT(handleTmpGrid(Grid*)));
	bool ret = newcond->init();
	if (ret) {
		// initialization succeeded.
		if (m_condition != nullptr) {
			m_condition->informDeselection(dataModel()->graphicsView());
			delete m_condition;
		}
		m_condition = newcond;
	} else {
		// initialization failed.
		delete newcond;
		return;
	}
	updateVisibilityWithoutRendering();
	m_createAction->setEnabled(true);
	dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent())->gridDataItem()->silentDeleteGrid();

	// switching grid creating algorithmn is not undo-able.
	iRICUndoStack::instance().clear();
	// Select this node in object browser.
	dataModel()->objectBrowserView()->select(m_standardItem->index());
	m_condition->showInitialDialog();
}

void PreProcessorGridCreatingConditionDataItem::doApplyOffset(double x, double y)
{
	if (m_condition == nullptr) {return;}
	m_condition->applyOffset(x, y);
}
