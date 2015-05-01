#include "preprocessorgridrelatedconditionnodedataitem.h"
#include "preprocessorgridrelatedconditionnodegroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorrawdatatopdataitem.h"
#include "preprocessorrawdatagroupdataitem.h"
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcomplexcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcomplexcondition.h>
#include <guibase/contoursettingwidget.h>
#include "preprocessorgriddataitem.h"
#include "../preprocessorwindow.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/gridrelatednodeconditionpropertydialog.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioneditdialog.h>
#include <guicore/pre/gridcond/base/gridrelatedconditionvariationeditdialog.h>
#include "../preprocessorgraphicsview.h"
#include <misc/xmlsupport.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/scalarstocolors/scalarstocolorseditwidget.h>
#include <guicore/scalarstocolors/lookuptableeditwidget.h>
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowprojectdataitem.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.h"
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>

#include <misc/errormessage.h>
#include <misc/stringtool.h>
#include <misc/lastiodirectory.h>

#include <QStandardItem>
#include <QXmlStreamWriter>
#include <QDomNode>
#include <QVector>
#include <QMouseEvent>
#include <QMenu>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextStream>
#include <QFileDialog>

#include <vtkCollectionIterator.h>
#include <vtkRenderer.h>
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkVariant.h>
#include <string>

PreProcessorGridRelatedConditionNodeDataItem::PreProcessorGridRelatedConditionNodeDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent)
	: PreProcessorDataItem(cond->caption(), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;
	m_definingBoundingBox = false;
	m_condition = cond;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	m_contour = ContourSettingWidget::ColorFringe;
	m_numberOfDivision = 10;

	connect(this, SIGNAL(changed(PreProcessorGridRelatedConditionNodeDataItem*)),
					parent, SLOT(exclusivelyCheck(PreProcessorGridRelatedConditionNodeDataItem*)));

	m_editValueAction = new QAction(tr("Edit value..."), this);
	m_editValueAction->setDisabled(true);
	connect(m_editValueAction, SIGNAL(triggered()), this, SLOT(editValue()));

	m_exportAction = new QAction(QIcon(":/libs/guibase/images/iconExport.png"),tr("Export..."), this);
	connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportToFile()));

	m_editVariationAction = new QAction(tr("Edit value by specifying variation..."), this);
	m_editVariationAction->setDisabled(true);
	connect(m_editVariationAction, SIGNAL(triggered()), this, SLOT(editVariation()));

	m_openXsectionWindowAction = new QAction(tr("Open &Crosssection Window"), this);
	m_openXsectionWindowAction->setDisabled(true);
	connect(m_openXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));

	m_openVXsectionWindowAction = new QAction(tr("Open &Vertical Crosssection Window"), this);
	m_openVXsectionWindowAction ->setDisabled(true);
	connect(m_openVXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openVerticalCrossSectionWindow()));
}

void PreProcessorGridRelatedConditionNodeDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

QDialog* PreProcessorGridRelatedConditionNodeDataItem::propertyDialog(QWidget* p)
{
	PreProcessorGridRelatedConditionNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent());
	ScalarsToColorsEditWidget* stcWidget = m_condition->createScalarsToColorsEditWidget(0);
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(iRIC::toStr(m_condition->name()).c_str());
	LookupTableEditWidget* ltWidget = dynamic_cast<LookupTableEditWidget*>(stcWidget);
	if (ltWidget != nullptr) {
		ltWidget->showDivisionNumber();
	}
	stcWidget->setContainer(stc);
	GridRelatedNodeConditionPropertyDialog* dialog = new GridRelatedNodeConditionPropertyDialog(p);
	dialog->setScalarsToColorsEditWidget(stcWidget);
	dialog->setContour(contour());
	dialog->setNumberOfDivision(m_numberOfDivision);
	dialog->setOpacityPercent(gitem->opacityPercent());
	dialog->setWindowTitle(tr("Grid Node Attribute Display Setting"));
	return dialog;
}

void PreProcessorGridRelatedConditionNodeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridRelatedNodeConditionPropertyDialog* dialog = dynamic_cast<GridRelatedNodeConditionPropertyDialog*>(propDialog);

	m_contour = dialog->contour();
	m_numberOfDivision = dialog->numberOfDivision();
	PreProcessorGridRelatedConditionNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent());
	gitem->setOpacityPercent(dialog->opacityPercent());
	gitem->updateActorSettings();
}

void PreProcessorGridRelatedConditionNodeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCustomModified = static_cast<bool>(node.toElement().attribute("isCustomModified", "0").toInt());
	QDomNode contourNode = iRIC::getChildNode(node, "Contour");
	if (! contourNode.isNull()) {
		loadContourFromProjectMainFile(contourNode);
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString mod;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g != nullptr) {
		GridRelatedConditionContainer* cont = g->gridRelatedCondition(m_condition->name());
		mod.setNum(static_cast<int>(cont->isCustomModified()));
		writer.writeAttribute("isCustomModified", mod);
	}

	writer.writeStartElement("Contour");
	saveContourToProjectMainFile(writer);
	writer.writeEndElement();
}

void PreProcessorGridRelatedConditionNodeDataItem::loadFromCgnsFile(const int /*fn*/)
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g == nullptr) {return;}
	GridRelatedConditionContainer* cont = g->gridRelatedCondition(m_condition->name());
	cont->setCustomModified(m_isCustomModified);
}

void PreProcessorGridRelatedConditionNodeDataItem::loadContourFromProjectMainFile(const QDomNode& node)
{
	m_contour = (ContourSettingWidget::Contour) node.toElement().attribute("ContourType", "0").toInt();
	m_numberOfDivision = node.toElement().attribute("NumberOfDivision", "10").toInt();
}

void PreProcessorGridRelatedConditionNodeDataItem::saveContourToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("ContourType", qstr.setNum((int) m_contour));
	writer.writeAttribute("NumberOfDivision", qstr.setNum(m_numberOfDivision));
}

void PreProcessorGridRelatedConditionNodeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMouseMoveEvent(event, v);
	} else {
		dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent())->updateAttributeBrowser(QPoint(event->x(), event->y()), v);
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

void PreProcessorGridRelatedConditionNodeDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		m_definingBoundingBox = true;
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMousePressEvent(event, v);
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			tmpparent->nodeSelectingMouseReleaseEvent(event, v);
		}
		m_definingBoundingBox = false;
		dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent())->fixAttributeBrowser(QPoint(event->x(), event->y()), v);
	} else if (event->button() == Qt::RightButton) {
		if (menu != nullptr) {delete menu;}
		menu = new QMenu(projectData()->mainWindow());
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		bool vertexSelected = (tmpparent->selectedVertices().count() > 0);
		menu->addAction(m_editValueAction);
		m_editValueAction->setEnabled(vertexSelected);
		if (! m_condition->isOption() && dynamic_cast<SolverDefinitionGridRelatedComplexCondition*>(m_condition) == nullptr) {
			menu->addAction(m_editVariationAction);
			m_editVariationAction->setEnabled(vertexSelected);
		}

		Structured2DGrid* sgrid = dynamic_cast<Structured2DGrid*>(tmpparent->grid());

		if (sgrid != nullptr) {
			menu->addSeparator();
			menu->addAction(m_openXsectionWindowAction);
			m_openXsectionWindowAction->setEnabled(vertexSelected);

			menu->addAction(m_openVXsectionWindowAction);
			m_openVXsectionWindowAction->setEnabled(vertexSelected);
		}
		menu->addSeparator();
		menu->move(event->globalPos());
		menu->show();
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->nodeSelectingKeyPressEvent(event, v);
}

void PreProcessorGridRelatedConditionNodeDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->nodeSelectingKeyReleaseEvent(event, v);
}

void PreProcessorGridRelatedConditionNodeDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_exportAction);
	menu->addSeparator();
	PreProcessorGridRelatedConditionNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent());
	menu->addAction(gitem->showAttributeBrowserAction());
}

void PreProcessorGridRelatedConditionNodeDataItem::editValue()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridRelatedConditionEditDialog* dialog = m_condition->editDialog(preProcessorWindow());
	dialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid nodes.")).arg(m_condition->caption()));
	PreProcessorGridTypeDataItem* tItem =
		dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	PreProcessorRawDataGroupDataItemInterface* i = tItem->rawdataTop()->groupDataItem(m_condition->name());
	i->setupEditWidget(dialog->widget());
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	QVector<vtkIdType> targets = tmpparent->selectedVertices();
	Grid* g = tmpparent->grid();
	dialog->scanAndSetDefault(g->gridRelatedCondition(m_condition->name()), targets);

	if (QDialog::Accepted == dialog->exec()) {
		dialog->applyValue(g->gridRelatedCondition(m_condition->name()), targets, g->vtkGrid()->GetPointData(), tmpparent);
	}
	delete dialog;
}

void PreProcessorGridRelatedConditionNodeDataItem::editVariation()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridRelatedConditionVariationEditDialog* dialog = m_condition->variationEditDialog(preProcessorWindow());
	if (dialog == nullptr) {return;}
	dialog->setWindowTitle(QString(tr("Apply variation to %1").arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the variation of %1 at the selected grid nodes.")).arg(m_condition->caption()));
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	QVector<vtkIdType> targets = tmpparent->selectedVertices();
	Grid* g = tmpparent->grid();

	if (QDialog::Accepted == dialog->exec()) {
		dialog->applyVariation(g->gridRelatedCondition(m_condition->name()), targets, g->vtkGrid()->GetPointData(), tmpparent);
	}
	delete dialog;
}

void PreProcessorGridRelatedConditionNodeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(true);
	dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent())->initAttributeBrowser();
	updateVisibility();
}

void PreProcessorGridRelatedConditionNodeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(false);
	dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent())->clearAttributeBrowser();
}

void PreProcessorGridRelatedConditionNodeDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(parent())->informDataChange(m_condition->name());
}

void PreProcessorGridRelatedConditionNodeDataItem::openCrossSectionWindow()
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (gItem->selectedVertices().count() > 1) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("To open a Crosssection Window, Please select only one grid node."));
		return;
	}
	unsigned int index = gItem->selectedVertices().at(0);
	unsigned int i, j;

	Grid* g = gItem->grid();
	Structured2DGrid* grid = dynamic_cast<Structured2DGrid*>(g);
	grid->getIJIndex(index, &i, &j);
	PreProcessorGridCrosssectionWindowProjectDataItem* pdi = new PreProcessorGridCrosssectionWindowProjectDataItem(this, m_condition->name(), preProcessorWindow());

	pdi->window()->setTarget(PreProcessorGridCrosssectionWindow::dirJ, i);
	m_crosssectionWindows.append(pdi);
	QMdiArea* cent = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
	QMdiSubWindow* container = cent->addSubWindow(pdi->window());
	container->setWindowIcon(pdi->window()->icon());

	container->show();
	container->setFocus();
	pdi->window()->cameraFit();
}

void PreProcessorGridRelatedConditionNodeDataItem::openVerticalCrossSectionWindow()
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (gItem->selectedVertices().count() > 1) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("To open a Crosssection Window, Please select only one grid node."));
		return;
	}
	unsigned int index = gItem->selectedVertices().at(0);
	unsigned int i, j;

	Grid* g = gItem->grid();
	Structured2DGrid* grid = dynamic_cast<Structured2DGrid*>(g);
	grid->getIJIndex(index, &i, &j);
	PreProcessorGridCrosssectionWindowProjectDataItem* pdi = new PreProcessorGridCrosssectionWindowProjectDataItem(this, m_condition->name(), preProcessorWindow());

	pdi->window()->setTarget(PreProcessorGridCrosssectionWindow::dirI, j);
	m_crosssectionWindows.append(pdi);
	QMdiArea* cent = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
	QMdiSubWindow* container = cent->addSubWindow(pdi->window());
	container->show();
	container->setFocus();
	pdi->window()->cameraFit();
}

void PreProcessorGridRelatedConditionNodeDataItem::updateCrossectionWindows()
{
	for (auto w_it = m_crosssectionWindows.begin(); w_it != m_crosssectionWindows.end(); ++w_it) {
		PreProcessorGridCrosssectionWindow* w = (*w_it)->window();
		w->setupData();
		w->updateView();
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::requestCrosssectionWindowDelete(PreProcessorGridCrosssectionWindowProjectDataItem* item)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		if (*it == item) {
			m_crosssectionWindows.erase(it);
			delete item;
			return;
		}
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::unregisterCrosssectionWindow(PreProcessorGridCrosssectionWindowProjectDataItem* item)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		if (*it == item) {
			m_crosssectionWindows.erase(it);
			return;
		}
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::informSelectedVerticesChanged(const QVector<vtkIdType>& vertices)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		PreProcessorGridCrosssectionWindowProjectDataItem* item = *it;
		item->window()->informSelectedVerticesChanged(vertices);
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::exportToFile()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}

	QString dir = LastIODirectory::get();
	QString filter(tr("Topography File (*.tpo)"));

	QString fname = QFileDialog::getSaveFileName(iricMainWindow(), tr("Export as Topography Data"), dir, filter);
	if (fname == "") { return; }

	try {
		QFile file(fname);
		if (! file.open(QFile::WriteOnly | QFile::Text)) {
			throw ErrorMessage(tr("Error occured while opening the file."));
		}
		QTextStream stream(&file);

		PreProcessorGridDataItem* gitem =  dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		vtkPointSet* vtkGrid = gitem->grid()->vtkGrid();
		vtkDataArray* da = vtkGrid->GetPointData()->GetArray(iRIC::toStr(m_condition->name()).c_str());
		// output datacount;
		stream << vtkGrid->GetNumberOfPoints() << endl;

		// output values
		for (vtkIdType i = 0; i < vtkGrid->GetNumberOfPoints(); ++i) {
			double x[3];
			vtkGrid->GetPoint(i, x);
			double val = da->GetVariantValue(i).ToDouble();
			stream << x[0] << " " << x[1] << " " << val << endl;
		}
		file.close();
		iricMainWindow()->statusBar()->showMessage(tr("Grid condition successfully exported to %1.").arg(QDir::toNativeSeparators(fname)), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
	} catch (ErrorMessage& message) {
		QMessageBox::critical(iricMainWindow(), tr("Error"), message);
	}
}

void PreProcessorGridRelatedConditionNodeDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	if (PreProcessorGridRelatedConditionNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridRelatedConditionNodeGroupDataItem*>(this->parent())) {
		gitem->updateActorSettings();
	}
}

bool PreProcessorGridRelatedConditionNodeDataItem::addToolBarButtons(QToolBar* toolbar)
{
	PreProcessorDataItem* item =
		dynamic_cast<PreProcessorDataItem*>(parent());
	return item->addToolBarButtons(toolbar);
}
