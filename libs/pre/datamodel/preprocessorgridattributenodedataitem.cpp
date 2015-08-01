#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowprojectdataitem.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"

#include <guibase/contoursettingwidget.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditdialog.h>
#include <guicore/pre/gridcond/gridnodeattributepropertydialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/scalarstocolors/lookuptableeditwidget.h>
#include <guicore/scalarstocolors/scalarstocolorseditwidget.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/errormessage.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QFileDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>
#include <QStatusBar>
#include <QTextStream>
#include <QVector>
#include <QXmlStreamWriter>

#include <vtkCell.h>
#include <vtkCollectionIterator.h>
#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkVariant.h>

#include <string>

PreProcessorGridAttributeNodeDataItem::PreProcessorGridAttributeNodeDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent) :
	PreProcessorDataItem {cond->caption(), QIcon(":/libs/guibase/images/iconPaper.png"), parent}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);
	m_definingBoundingBox = false;
	m_condition = cond;
	m_contour = ContourSettingWidget::ColorFringe;
	m_numberOfDivision = 10;

	connect(this, SIGNAL(changed(PreProcessorGridAttributeNodeDataItem*)),
					parent, SLOT(exclusivelyCheck(PreProcessorGridAttributeNodeDataItem*)));

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

void PreProcessorGridAttributeNodeDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

QDialog* PreProcessorGridAttributeNodeDataItem::propertyDialog(QWidget* p)
{
	PreProcessorGridAttributeNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent());
	ScalarsToColorsEditWidget* stcWidget = m_condition->createScalarsToColorsEditWidget(0);
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(iRIC::toStr(m_condition->name()).c_str());
	LookupTableEditWidget* ltWidget = dynamic_cast<LookupTableEditWidget*>(stcWidget);
	if (ltWidget != nullptr) {
		ltWidget->showDivisionNumber();
	}
	stcWidget->setContainer(stc);
	GridNodeAttributePropertyDialog* dialog = new GridNodeAttributePropertyDialog(p);
	dialog->setScalarsToColorsEditWidget(stcWidget);
	dialog->setContour(contour());
	dialog->setNumberOfDivision(m_numberOfDivision);
	dialog->setOpacityPercent(gitem->opacity());
	dialog->setWindowTitle(tr("Grid Node Attribute Display Setting"));
	return dialog;
}

void PreProcessorGridAttributeNodeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridNodeAttributePropertyDialog* dialog = dynamic_cast<GridNodeAttributePropertyDialog*>(propDialog);

	m_contour = dialog->contour();
	m_numberOfDivision = dialog->numberOfDivision();
	PreProcessorGridAttributeNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent());
	gitem->setOpacity(dialog->opacityPercent());
	gitem->updateActorSettings();
}

void PreProcessorGridAttributeNodeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCustomModified = static_cast<bool>(node.toElement().attribute("isCustomModified", "0").toInt());
	QDomNode contourNode = iRIC::getChildNode(node, "Contour");
	if (! contourNode.isNull()) {
		loadContourFromProjectMainFile(contourNode);
	}
}

void PreProcessorGridAttributeNodeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString mod;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g != nullptr) {
		GridAttributeContainer* cont = g->gridRelatedCondition(m_condition->name());
		mod.setNum(static_cast<int>(cont->isCustomModified()));
		writer.writeAttribute("isCustomModified", mod);
	}

	writer.writeStartElement("Contour");
	saveContourToProjectMainFile(writer);
	writer.writeEndElement();
}

void PreProcessorGridAttributeNodeDataItem::loadFromCgnsFile(const int /*fn*/)
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g == nullptr) {return;}
	GridAttributeContainer* cont = g->gridRelatedCondition(m_condition->name());
	cont->setCustomModified(m_isCustomModified);
}

void PreProcessorGridAttributeNodeDataItem::loadContourFromProjectMainFile(const QDomNode& node)
{
	m_contour = (ContourSettingWidget::Contour) node.toElement().attribute("ContourType", "0").toInt();
	m_numberOfDivision = node.toElement().attribute("NumberOfDivision", "10").toInt();
}

void PreProcessorGridAttributeNodeDataItem::saveContourToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("ContourType", qstr.setNum((int) m_contour));
	writer.writeAttribute("NumberOfDivision", qstr.setNum(m_numberOfDivision));
}

void PreProcessorGridAttributeNodeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMouseMoveEvent(event, v);
	} else {
		dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->updateAttributeBrowser(QPoint(event->x(), event->y()), v);
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

void PreProcessorGridAttributeNodeDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		m_definingBoundingBox = true;
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMousePressEvent(event, v);
	}
}

void PreProcessorGridAttributeNodeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			tmpparent->nodeSelectingMouseReleaseEvent(event, v);
		}
		m_definingBoundingBox = false;
		dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->fixAttributeBrowser(QPoint(event->x(), event->y()), v);
	} else if (event->button() == Qt::RightButton) {
		delete menu;
		menu = new QMenu(projectData()->mainWindow());
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		bool vertexSelected = (tmpparent->selectedVertices().count() > 0);
		menu->addAction(m_editValueAction);
		m_editValueAction->setEnabled(vertexSelected);
		if (! m_condition->isOption() && dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition) == nullptr) {
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

void PreProcessorGridAttributeNodeDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->nodeSelectingKeyPressEvent(event, v);
}

void PreProcessorGridAttributeNodeDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gitem->nodeSelectingKeyReleaseEvent(event, v);
}

void PreProcessorGridAttributeNodeDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_exportAction);
	menu->addSeparator();
	PreProcessorGridAttributeNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent());
	menu->addAction(gitem->showAttributeBrowserAction());
}

void PreProcessorGridAttributeNodeDataItem::editValue()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridAttributeEditDialog* dialog = m_condition->editDialog(preProcessorWindow());
	dialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid nodes.")).arg(m_condition->caption()));
	PreProcessorGridTypeDataItem* tItem =
		dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	PreProcessorGeoDataGroupDataItemInterface* i = tItem->geoDataTop()->groupDataItem(m_condition->name());
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

void PreProcessorGridAttributeNodeDataItem::editVariation()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridAttributeVariationEditDialog* dialog = m_condition->variationEditDialog(preProcessorWindow());
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

void PreProcessorGridAttributeNodeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(true);
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->initAttributeBrowser();
	updateVisibility();
}

void PreProcessorGridAttributeNodeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(false);
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->clearAttributeBrowser();
}

void PreProcessorGridAttributeNodeDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->informDataChange(m_condition->name());
}

void PreProcessorGridAttributeNodeDataItem::openCrossSectionWindow()
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

void PreProcessorGridAttributeNodeDataItem::openVerticalCrossSectionWindow()
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

void PreProcessorGridAttributeNodeDataItem::updateCrossectionWindows()
{
	for (auto w_it = m_crosssectionWindows.begin(); w_it != m_crosssectionWindows.end(); ++w_it) {
		PreProcessorGridCrosssectionWindow* w = (*w_it)->window();
		w->setupData();
		w->updateView();
	}
}

void PreProcessorGridAttributeNodeDataItem::requestCrosssectionWindowDelete(PreProcessorGridCrosssectionWindowProjectDataItem* item)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		if (*it == item) {
			m_crosssectionWindows.erase(it);
			delete item;
			return;
		}
	}
}

void PreProcessorGridAttributeNodeDataItem::unregisterCrosssectionWindow(PreProcessorGridCrosssectionWindowProjectDataItem* item)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		if (*it == item) {
			m_crosssectionWindows.erase(it);
			return;
		}
	}
}

void PreProcessorGridAttributeNodeDataItem::informSelectedVerticesChanged(const QVector<vtkIdType>& vertices)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		PreProcessorGridCrosssectionWindowProjectDataItem* item = *it;
		item->window()->informSelectedVerticesChanged(vertices);
	}
}

void PreProcessorGridAttributeNodeDataItem::exportToFile()
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

void PreProcessorGridAttributeNodeDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	if (PreProcessorGridAttributeNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(this->parent())) {
		gitem->updateActorSettings();
	}
}

bool PreProcessorGridAttributeNodeDataItem::addToolBarButtons(QToolBar* toolbar)
{
	PreProcessorDataItem* item =
		dynamic_cast<PreProcessorDataItem*>(parent());
	return item->addToolBarButtons(toolbar);
}
