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
#include "private/preprocessorgridcreatingconditiondataitem_impl.h"

#include <gridcreatingcondition/centerandwidth/gridcreatingconditioncenterandwidthdialog.h>
#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingconditionio.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

PreProcessorGridCreatingConditionDataItem::PreProcessorGridCreatingConditionDataItem(PreProcessorDataItem* dataitem) :
	PreProcessorGridCreatingConditionDataItemInterface {dataitem},
	impl {new Impl {}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	// create default condition.
	// @todo Currently, the default condition is the algorithm
	// that is found first, that is compatible with the type of grid.
	PreProcessorGridTypeDataItem* gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());

	// setup actions.
	impl->m_createAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("&Create Grid..."), this);
	impl->m_createAction->setDisabled(true);
	connect(impl->m_createAction, SIGNAL(triggered()), this, SLOT(createGrid()));

	impl->m_switchAlgorithmAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("&Select Algorithm to Create Grid..."), this);
	connect(impl->m_switchAlgorithmAction, SIGNAL(triggered()), this, SLOT(switchAlgorithm()));

	impl->m_clearAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("Reset to &Default..."), this);
	connect(impl->m_clearAction, SIGNAL(triggered()), this, SLOT(resetCondition()));
	impl->m_deleteAction = new QAction(PreProcessorGridCreatingConditionDataItem::tr("&Delete Grid Creating Condition..."), this);
	impl->m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteCondition()));

	impl->m_importAction = new QAction(QIcon(":/libs/guibase/images/iconImport.png"), PreProcessorGridCreatingConditionDataItem::tr("&Import..."), this);
	connect(impl->m_importAction, SIGNAL(triggered()), this, SLOT(importData()));

	impl->m_exportAction = new QAction(QIcon(":/libs/guibase/images/iconExport.png"), PreProcessorGridCreatingConditionDataItem::tr("&Export..."), this);
	connect(impl->m_exportAction, SIGNAL(triggered()), this, SLOT(exportData()));

	GridCreatingConditionFactory& factory = GridCreatingConditionFactory::instance(iricMainWindow());
	QList<GridCreatingConditionCreator*> cList = factory.compatibleCreators(*(gTypeItem->gridType()));

	// default condition is not set.
	impl->m_condition = nullptr;
	if (cList.count() < 1) {
		// users can not switch algorithmn to create grid.
		impl->m_switchAlgorithmAction->setDisabled(true);
	}
}

PreProcessorGridCreatingConditionDataItem::~PreProcessorGridCreatingConditionDataItem()
{
	delete impl->m_condition;
	delete impl;
}

GridCreatingCondition* PreProcessorGridCreatingConditionDataItem::condition() const
{
	return impl->m_condition;
}

void PreProcessorGridCreatingConditionDataItem::setCondition(GridCreatingCondition* condition)
{
	impl->m_condition = condition;
}

PreProcessorGridTypeDataItemInterface* PreProcessorGridCreatingConditionDataItem::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItemInterface*> (parent()->parent());
}

void PreProcessorGridCreatingConditionDataItem::addCustomMenuItems(QMenu* menu)
{
	if (impl->m_condition == nullptr) {
		menu->addAction(impl->m_switchAlgorithmAction);
		menu->addSeparator();
		menu->addAction(impl->m_importAction);
		return;
	}
	menu->addAction(impl->m_createAction);
	menu->addSeparator();
	menu->addAction(impl->m_switchAlgorithmAction);
	menu->addSeparator();
	menu->addAction(impl->m_importAction);
	menu->addAction(impl->m_exportAction);
	menu->addSeparator();
	menu->addAction(impl->m_deleteAction);
}

bool PreProcessorGridCreatingConditionDataItem::addToolBarButtons(QToolBar* tb)
{
	if (impl->m_condition == nullptr) {return false;}
	return impl->m_condition->addToolBarButtons(tb);
}

void PreProcessorGridCreatingConditionDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->assignActorZValues(range);
}

void PreProcessorGridCreatingConditionDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (impl->m_condition != nullptr) {
		delete impl->m_condition;
	}
	impl->m_condition = GridCreatingConditionFactory::instance(iricMainWindow()).restore(node, this);
	impl->m_createAction->setDisabled(true);
	if (impl->m_condition != nullptr) {
		impl->m_condition->setupActors();
		impl->m_condition->informDeselection(dataModel()->graphicsView());
		impl->m_condition->assignActorZValues(m_zDepthRange);
		impl->m_condition->setupMenu();
		connect(impl->m_condition, SIGNAL(gridCreated(Grid*)), this, SLOT(handleNewGrid(Grid*)));
		connect(impl->m_condition, SIGNAL(tmpGridCreated(Grid*)), this, SLOT(handleTmpGrid(Grid*)));
		if (impl->m_condition->init()) {
			impl->m_createAction->setEnabled(true);
		} else {
			// error occured while initializing.
			delete impl->m_condition;
			impl->m_condition = nullptr;
		}
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorGridCreatingConditionDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	if (impl->m_condition != nullptr) {
		writer.writeAttribute("name", impl->m_condition->name());
		impl->m_condition->saveToProjectMainFile(writer);
	}
}

void PreProcessorGridCreatingConditionDataItem::innerUpdate2Ds()
{
	if (impl->m_condition != nullptr) {
		impl->m_condition->update2Ds();
	}
}

void PreProcessorGridCreatingConditionDataItem::createGrid()
{
	if (impl->m_condition == nullptr) {return;}
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
	bool ok = impl->m_condition->create(preProcessorWindow());
	if (! ok) {return;}
	PreProcessorBCSettingGroupDataItem* bcsgItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent())->bcSettingGroupDataItem();
	bcsgItem->executeMapping();
	// succeeded.
	iRICUndoStack::instance().clear();
}

void PreProcessorGridCreatingConditionDataItem::silentDeleteCondition()
{
	if (impl->m_condition == nullptr) {return;}
	delete impl->m_condition;
	impl->m_condition = nullptr;
	impl->m_createAction->setDisabled(true);
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
	if (impl->m_condition == nullptr) {return;}
	delete impl->m_condition;
	impl->m_condition = nullptr;
	impl->m_createAction->setDisabled(true);
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
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->clear();
	renderGraphicsView();

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	impl->m_condition->showInitialDialog();
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

QAction* PreProcessorGridCreatingConditionDataItem::createAction() const
{
	return impl->m_createAction;
}

QAction* PreProcessorGridCreatingConditionDataItem::switchAction() const
{
	return impl->m_switchAlgorithmAction;
}

QAction* PreProcessorGridCreatingConditionDataItem::deleteAction() const
{
	return impl->m_deleteAction;
}

QAction* PreProcessorGridCreatingConditionDataItem::clearAction() const
{
	return impl->m_clearAction;
}

QAction* PreProcessorGridCreatingConditionDataItem::importAction() const
{
	return impl->m_importAction;
}

QAction* PreProcessorGridCreatingConditionDataItem::exportAction() const
{
	return impl->m_exportAction;
}

void PreProcessorGridCreatingConditionDataItem::handleStandardItemDoubleClicked()
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->handleStandardItemDoubleClicked();
}

void PreProcessorGridCreatingConditionDataItem::informSelection(VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->informSelection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::informDeselection(VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->informDeselection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::viewOperationEnded(VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->viewOperationEnded(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->keyPressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->mouseDoubleClickEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGridCreatingConditionDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

QStringList PreProcessorGridCreatingConditionDataItem::containedFiles()
{
	if (impl->m_condition == nullptr) {return QStringList();}
	return impl->m_condition->containedFiles();
}

QMenu* PreProcessorGridCreatingConditionDataItem::menu()
{
	if (impl->m_condition != nullptr) {
		return impl->m_condition->menu();
	} else {
		return nullptr;
	}
}

void PreProcessorGridCreatingConditionDataItem::switchAlgorithm()
{
	if (impl->m_condition != nullptr && QMessageBox::Cancel == QMessageBox::warning(mainWindow(), tr("Warning"), tr("When you switch algorithm to create grid, the grid and the grid creating condition you created will be discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)) {
		return;
	}
	PreProcessorGridTypeDataItem* gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	GridCreatingConditionFactory& factory = GridCreatingConditionFactory::instance(iricMainWindow());
	QList<GridCreatingConditionCreator*> cList = factory.compatibleCreators(*(gTypeItem->gridType()));
	PreProcessorGridCreatingConditionAlgorithmSelectDialog dialog;
	dialog.setCreators(cList);
	if (impl->m_condition != nullptr) {
		dialog.setCurrent(impl->m_condition->creator());
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
		if (impl->m_condition != nullptr) {
			impl->m_condition->informDeselection(dataModel()->graphicsView());
			delete impl->m_condition;
		}
		impl->m_condition = newcond;
	} else {
		// initialization failed.
		delete newcond;
		return;
	}
	updateVisibilityWithoutRendering();
	impl->m_createAction->setEnabled(true);
	dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent())->gridDataItem()->silentDeleteGrid();

	// switching grid creating algorithmn is not undo-able.
	iRICUndoStack::instance().clear();
	// Select this node in object browser.
	dataModel()->objectBrowserView()->select(m_standardItem->index());
	impl->m_condition->showInitialDialog();
}

void PreProcessorGridCreatingConditionDataItem::importData()
{
	QString fname = QFileDialog::getOpenFileName(iricMainWindow(), tr("Select file to import"), LastIODirectory::get(), tr("iRIC grid creating condition file(*.igcc)"));
	if (fname.isNull()) {return;}

	GridCreatingConditionFactory& factory = GridCreatingConditionFactory::instance(iricMainWindow());
	GridCreatingCondition* newcond = GridCreatingConditionIO::importData(&factory, this, fname, projectData()->workDirectory());

	auto gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	QList<GridCreatingConditionCreator*> cList = factory.compatibleCreators(*(gTypeItem->gridType()));
	if (! cList.contains(newcond->creator())) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The grid creating condition in this file is not compatible with the solver"));
		delete newcond;
		return;
	}

	newcond->setupActors();
	newcond->assignActorZValues(m_zDepthRange);
	newcond->setupMenu();
	connect(newcond, SIGNAL(gridCreated(Grid*)), this, SLOT(handleNewGrid(Grid*)));
	connect(newcond, SIGNAL(tmpGridCreated(Grid*)), this, SLOT(handleTmpGrid(Grid*)));

	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());

	bool ret = newcond->init();
	if (ret) {
		if (impl->m_condition != nullptr) {
			impl->m_condition->informDeselection(dataModel()->graphicsView());
			delete impl->m_condition;
		}
		impl->m_condition = newcond;
	}
	updateVisibilityWithoutRendering();
	impl->m_createAction->setEnabled(true);
	iRICUndoStack::instance().clear();

	dataModel()->objectBrowserView()->select(m_standardItem->index());
}

void PreProcessorGridCreatingConditionDataItem::exportData()
{
	if (impl->m_condition == nullptr) {
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("No data to export."));
		return;
	}

	QString fname = QFileDialog::getSaveFileName(iricMainWindow(), tr("Select file to export"), LastIODirectory::get(), tr("iRIC grid creating condition file(*.igcc)"));
	if (fname.isNull()) {return;}

	GridCreatingConditionIO::exportData(impl->m_condition, fname, projectData()->workDirectory());
}

void PreProcessorGridCreatingConditionDataItem::doApplyOffset(double x, double y)
{
	if (impl->m_condition == nullptr) {return;}
	impl->m_condition->applyOffset(x, y);
}
