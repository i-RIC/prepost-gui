#include "datamodel/preprocessorgridandgridcreatingconditiondataitem.h"
#include "datamodel/preprocessorgriddataitem.h"
#include "datamodel/preprocessorgridtypedataitem.h"
#include "datamodel/preprocessorinputconditiondataitem.h"
#include "datamodel/preprocessorrootdataitem.h"
#include "preobjectbrowser.h"
#include "preobjectbrowserview.h"
#include "preprocessordatamodel.h"
#include "preprocessorgraphicsview.h"
#include "preprocessorwindow.h"
#include "preprocessorwindowactionmanager.h"
#include "preprocessorwindowprojectdataitem.h"
#include "prepropertybrowser.h"

#include <guibase/colortool.h>
#include <guibase/irictoolbar.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectmainfile.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QFile>
#include <QGraphicsView>
#include <QLabel>
#include <QMenu>
#include <QTextStream>
#include <QToolBar>
#include <QUndoCommand>

#include <vtkRenderer.h>

PreProcessorWindow::PreProcessorWindow(QWidget* parent)
	: PreProcessorWindowInterface(parent)
{
	init();
}

void PreProcessorWindow::init()
{
	setMinimumSize(480, 360);
	setWindowTitle(tr("Pre-processing Window"));

	// setup graphics view
	m_graphicsView = new PreProcessorGraphicsView(this);
	setCentralWidget(m_graphicsView);
	connect(m_graphicsView, SIGNAL(worldPositionChangedForStatusBar(QVector2D)), this, SIGNAL(worldPositionChangedForStatusBar(QVector2D)));

	m_dataModel = 0;
	m_actionManager = new PreProcessorWindowActionManager(this);
	m_objectBrowser = new PreObjectBrowser(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);
	m_propertyBrowser = new PrePropertyBrowser(this);
	m_propertyBrowser->hide();
	addDockWidget(Qt::LeftDockWidgetArea, m_propertyBrowser);

	m_initialState = saveState();

	m_isFirstHiding = true;
	m_isLastHiding = false;
}

void PreProcessorWindow::setProjectData(ProjectData* d)
{
	m_projectDataItem = new PreProcessorWindowProjectDataItem(this, d->mainfile());
}

PreProcessorWindowProjectDataItem* PreProcessorWindow::projectDataItem()
{
	return m_projectDataItem;
}

void PreProcessorWindow::setupDefaultGeometry()
{
	QWidget* parent = parentWidget();
	parent->move(0, 0);
	if (! parent->isMaximized()) {
		parent->resize(700, 500);
	}
	m_propertyBrowser->hide();
	restoreState(m_initialState);
}

void PreProcessorWindow::importCalcCondition()
{
	model()->importCalcCondition();
}

void PreProcessorWindow::exportCalcCondition()
{
	model()->exportCalcCondition();
}

void PreProcessorWindow::showCalcConditionDialog()
{
	model()->showCalcConditionDialog();
}

bool PreProcessorWindow::importInputCondition(const QString& filename)
{
	return model()->importInputCondition(filename);
}

bool PreProcessorWindow::exportInputCondition(const QString& filename)
{
	return model()->exportInputCondition(filename);
}

bool PreProcessorWindow::isInputConditionSet()
{
	return model()->isInputConditionSet();
}

PreProcessorWindow::GridState PreProcessorWindow::checkGridState()
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(model()->rootDataItem());
	QList<PreProcessorGridTypeDataItem*> gridTypeDataItems = root->gridTypeDataItems();
	bool ngexists = false;
	bool okexists = false;
	for (auto it = gridTypeDataItems.begin(); it != gridTypeDataItems.end(); ++it) {
		PreProcessorGridTypeDataItem* typeItem = (*it);
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = typeItem->conditions();
		for (auto it2 = conds.begin(); it2 != conds.end(); ++it2) {
			PreProcessorGridAndGridCreatingConditionDataItemInterface* cond = *it2;
			PreProcessorGridDataItemInterface* g = cond->gridDataItem();
			if (g->grid() == 0) {
				ngexists = true;
			} else {
				okexists = true;
			}
		}
	}
	if (okexists) {
		if (ngexists) {
			return PreProcessorWindow::grPartiallyUnfinished;
		} else {
			return PreProcessorWindow::grFinished;
		}
	} else {
		if (ngexists) {
			return PreProcessorWindow::grUnfinished;
		} else {
			// there is no grid item.
			// we do not need to set up grids.
			return PreProcessorWindow::grFinished;
		}
	}
}

void PreProcessorWindow::checkCalculationConditionImportSourceUpdate()
{
	PreProcessorDataModel* m = model();
	auto icItem = m->rootDataItem()->inputConditionDataItem();
	icItem->checkImportSourceUpdate();
}

QString PreProcessorWindow::checkGrid(bool detail)
{
	QString ret;
	PreProcessorDataModel* m = model();
	PreProcessorRootDataItem* root = m->rootDataItem();
	QList<PreProcessorGridTypeDataItem*> gridTypeDataItems = root->gridTypeDataItems();
	QList<QString> gridNames;
	QList<QString> gridMessages;
	QFile logFile(m->projectData()->absoluteFileName("gridcheck.txt"));
	logFile.open(QFile::WriteOnly | QFile::Text);
	QTextStream logStream(&logFile);
	for (auto it = gridTypeDataItems.begin(); it != gridTypeDataItems.end(); ++it) {
		PreProcessorGridTypeDataItem* typeItem = (*it);
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = typeItem->conditions();
		for (auto it2 = conds.begin(); it2 != conds.end(); ++it2) {
			PreProcessorGridAndGridCreatingConditionDataItemInterface* cond = *it2;
			PreProcessorGridDataItemInterface* g = cond->gridDataItem();
			Grid* grid = g->grid();
			if (grid == 0) {
				gridNames.append(cond->caption());
				QString msg = "<ul>";
				msg += "<li>" + tr("Grid is not created or imported yet.") + "</li>";
				msg += "</ul>";
				gridMessages.append(msg);
				continue;
			}
			if (detail) {
				logStream << tr("Checking grid %1 ...").arg(cond->caption()) << endl;
				QStringList messages = grid->checkShape(logStream);
				if (messages.count() > 0) {
					gridNames.append(cond->caption());
					QString msg;
					msg = "<ul>";
					for (int i = 0; i < messages.count(); ++i) {
						QString tmpmsg = messages.at(i);
						msg += "<li>" + tmpmsg + "</li>";
					}
					msg += "</ul>";
					gridMessages.append(msg);
				}
			}
		}
	}
	logFile.close();
	if (gridNames.count() == 0) {
		return ret;
	} else if (gridNames.count() == 1) {
		ret = gridMessages.at(0);
		return ret;
	} else {
		ret = "<ul>";
		for (int i = 0; i < gridNames.count(); ++i) {
			ret += "<li>";
			gridNames.at(i);
			gridMessages.at(i);
			ret += "</li>";
		}
		ret += "</ul>";
		return ret;
	}
}

QPixmap PreProcessorWindow::snapshot()
{
	PreProcessorGraphicsViewInterface* view = m_dataModel->graphicsView();
	QImage img = view->getImage();
	QPixmap pixmap = QPixmap::fromImage(img);
	if (m_isTransparent) { makeBackgroundTransparent(view, pixmap); }

	return pixmap;
}

vtkRenderWindow* PreProcessorWindow::getVtkRenderWindow() const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (m_graphicsView->mainRenderer());
	return r->GetRenderWindow();
}

QList<QMenu*> PreProcessorWindow::getAdditionalMenus() const
{
	PreProcessorDataModel* model = dynamic_cast<PreProcessorDataModel*>(m_dataModel);
	QList<QMenu*> menus;
	if (m_dataModel != nullptr) {
		menus.append(model->additionalMenus());
	}
	return menus;
}

QToolBar* PreProcessorWindow::getAdditionalToolBar() const
{
	if (m_dataModel == nullptr) {return nullptr;}
	return m_dataModel->operationToolBar();
}

void PreProcessorWindow::cameraFit()
{
	m_dataModel->graphicsView()->cameraFit();
}

void PreProcessorWindow::cameraResetRotation()
{
	m_dataModel->graphicsView()->cameraResetRotation();
}

void PreProcessorWindow::cameraRotate90()
{
	m_dataModel->graphicsView()->cameraRotate90();
}

void PreProcessorWindow::cameraZoomIn()
{
	m_dataModel->graphicsView()->cameraZoomIn();
}

void PreProcessorWindow::cameraZoomOut()
{
	m_dataModel->graphicsView()->cameraZoomOut();
}

void PreProcessorWindow::cameraMoveLeft()
{
	m_dataModel->graphicsView()->cameraMoveLeft();
}

void PreProcessorWindow::cameraMoveRight()
{
	m_dataModel->graphicsView()->cameraMoveRight();
}

void PreProcessorWindow::cameraMoveUp()
{
	m_dataModel->graphicsView()->cameraMoveUp();
}

void PreProcessorWindow::cameraMoveDown()
{
	m_dataModel->graphicsView()->cameraMoveDown();
}

void PreProcessorWindow::editBackgroundColor()
{
	BackgroundColorEditInterface::editBackgroundColor(this);
}

class PreProcessorWindowCloseCommand : public QUndoCommand
{
public:
	PreProcessorWindowCloseCommand(PreProcessorWindow* pre)
		: QUndoCommand(QObject::tr("Close PreProcessor Window")) {
		m_preWindow = pre;
	}
	void undo() {
		m_preWindow->parentWidget()->show();
	}
	void redo() {
		m_preWindow->parentWidget()->hide();
	}
private:
	PreProcessorWindow* m_preWindow;
};

void PreProcessorWindow::closeEvent(QCloseEvent* e)
{
	iRICUndoStack::instance().push(new PreProcessorWindowCloseCommand(this));
	e->ignore();
}

void PreProcessorWindow::showEvent(QShowEvent* /*e*/)
{
	ProjectMainFile* mainfile = dynamic_cast<ProjectMainFile*>(m_projectDataItem->parent());
	if (mainfile == nullptr) {return;}
	mainfile->addRenderer(m_dataModel->graphicsView()->mainRenderer());
}

void PreProcessorWindow::hideEvent(QHideEvent* /*e*/)
{
	if (m_isFirstHiding) {
		m_isFirstHiding = false;
		return;
	}
	if (m_isLastHiding) {
		m_isLastHiding = false;
		return;
	}
	if (m_projectDataItem == 0) {return;}
	ProjectMainFile* mainfile = dynamic_cast<ProjectMainFile*>(m_projectDataItem->parent());
	if (mainfile == 0) {return;}
	mainfile->removeRenderer(m_dataModel->graphicsView()->mainRenderer());
}

void PreProcessorWindow::handleAdditionalMenusUpdate(const QList<QMenu*>& m)
{
	QList<QMenu*> menus;
	menus.append(m);
	emit additionalMenusUpdated(menus);
}

QMenu* PreProcessorWindow::calcCondMenu() const
{
	return m_actionManager->calcCondMenu();
}

void PreProcessorWindow::addGridCreatingConditionImportMenu(QMenu* menu)
{
	PreProcessorDataModel* m = model();
	if (m == nullptr) {
		// add dummy disabled menu
		QAction* no = menu->addAction(tr("Gr&id Creating Condition..."));
		no->setDisabled(true);
		return;
	}
	m->addGridCreatingConditionImportMenu(menu);
}

void PreProcessorWindow::addGridCreatingConditionExportMenu(QMenu* menu)
{
	model()->addGridCreatingConditionExportMenu(menu);
}

void PreProcessorWindow::addGridImportMenu(QMenu* menu)
{
	PreProcessorDataModel* m = model();
	if (m == nullptr) {
		// add dummy disabled menu
		QAction* no = menu->addAction(tr("Grid..."));
		no->setDisabled(true);
		return;
	}
	m->addGridImportMenu(menu);
}

void PreProcessorWindow::addGridExportMenu(QMenu* menu)
{
	model()->addGridExportMenu(menu);
}

void PreProcessorWindow::setupGeoDataImportMenu()
{
	PreProcessorDataModel* m = model();
	if (m == nullptr) {return;}
	QMenu* menu = dynamic_cast<QMenu*>(sender());
	m->setupGeoDataImportMenu(menu);
}

void PreProcessorWindow::setupGeoDataImportFromWebMenu()
{
	PreProcessorDataModel* m = model();
	if (m == nullptr) {return;}
	QMenu* menu = dynamic_cast<QMenu*>(sender());
	m->setupGeoDataImportFromWebMenu(menu);
}

void PreProcessorWindow::setupGeoDataExportMenu()
{
	QMenu* menu = dynamic_cast<QMenu*>(sender());
	model()->setupGeoDataExportMenu(menu);
}

void PreProcessorWindow::setupHydraulicDataImportMenu()
{
	PreProcessorDataModel* m = model();
	if (m == nullptr) {return;}
	QMenu* menu = dynamic_cast<QMenu*>(sender());
	m->setupHydraulicDataImportMenu(menu);
}


void PreProcessorWindow::informUnfocusRiverCrosssectionWindows()
{
	PreProcessorDataModel* m = model();
	if (m == nullptr) {return;}
	m->informUnfocusRiverCrosssectionWindows();
}

bool PreProcessorWindow::isSetupCorrectly() const
{
	return model()->isSetupCorrectly();
}

bool PreProcessorWindow::checkMappingStatus()
{
	return model()->checkMappingStatus();
}

PreProcessorDataModelInterface* PreProcessorWindow::dataModel() const
{
	return m_dataModel;
}

void PreProcessorWindow::updateTmsList()
{
	auto m = model();
	if (m == nullptr) {return;}

	m->updateTmsList();
}

PreProcessorDataModel* PreProcessorWindow::model() const
{
	if (m_dataModel == nullptr){return nullptr;}
	return dynamic_cast<PreProcessorDataModel*>(m_dataModel);
}

VTKGraphicsView* PreProcessorWindow::viewForBackgroundColor() const
{
	return m_graphicsView;
}

ObjectBrowser* PreProcessorWindow::objectBrowser() const
{
	return m_objectBrowser;
}
