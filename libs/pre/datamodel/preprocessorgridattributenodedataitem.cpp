#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowprojectdataitem.h"
#include "attributebrowserhelper.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorgraphicsview.h"
#include "private/preprocessorgridattributenodedataitem_propertydialog.h"

#include <geodata/pointmap/geodatapointmaprealbuilder.h>
#include <guibase/widget/contoursettingwidget.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/pre/complex/gridcomplexconditiongroupeditdialog.h>
#include <guicore/pre/complex/gridcomplexconditiongrouprealeditwidget.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/qwidgetcontainer.h>
#include <misc/stringtool.h>
#include <misc/tpoexporter.h>
#include <misc/valuechangert.h>
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
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkVariant.h>

#include <iriclib_errorcodes.h>

#include <cfloat>
#include <string>

PreProcessorGridAttributeNodeDataItem::PreProcessorGridAttributeNodeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(cond->name(), cond->caption(), parent)
{
	m_definingBoundingBox = false;
	m_condition = cond;

	m_editValueAction = new QAction(PreProcessorGridAttributeNodeDataItem::tr("Edit value..."), this);
	m_editValueAction->setDisabled(true);
	connect(m_editValueAction, SIGNAL(triggered()), this, SLOT(editValue()));

	m_exportAction = new QAction(QIcon(":/libs/guibase/images/iconExport.svg"),PreProcessorGridAttributeNodeDataItem::tr("Export..."), this);
	connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportToFile()));

	m_generatePointMapAction = new QAction(PreProcessorGridAttributeNodeDataItem::tr("Generate point cloud data"), this);
	connect(m_generatePointMapAction, SIGNAL(triggered()), this, SLOT(generatePointMap()));

	m_editDifferenceAction = new QAction(PreProcessorGridAttributeNodeDataItem::tr("Edit value by specifying difference..."), this);
	m_editDifferenceAction->setDisabled(true);
	connect(m_editDifferenceAction, SIGNAL(triggered()), this, SLOT(editDifference()));

	m_editRatioAction = new QAction(PreProcessorGridAttributeNodeDataItem::tr("Edit value by specifying ratio..."), this);
	m_editRatioAction->setDisabled(true);
	connect(m_editRatioAction, SIGNAL(triggered()), this, SLOT(editRatio()));

	m_openXsectionWindowAction = new QAction(PreProcessorGridAttributeNodeDataItem::tr("Open &Cross Section Window"), this);
	m_openXsectionWindowAction->setDisabled(true);
	connect(m_openXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));

	m_openVXsectionWindowAction = new QAction(PreProcessorGridAttributeNodeDataItem::tr("Open &Longitudinal Cross Section Window"), this);
	m_openVXsectionWindowAction ->setDisabled(true);
	connect(m_openVXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openVerticalCrossSectionWindow()));

	m_groupEditDialog = new GridComplexConditionGroupEditDialog(mainWindow());
	m_groupEditDialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));

	auto gItem = geoDataGroupDataItem();
	GeoDataCreator* creator = gItem->getPointMapCreator();
	if (creator == nullptr) {
		m_generatePointMapAction->setDisabled(true);
	}
	m_colorMapToolBarWidgetController = gridTypeDataItem()->createToolBarWidgetController(cond->name(), mainWindow());
}

PreProcessorGridAttributeNodeDataItem::~PreProcessorGridAttributeNodeDataItem()
{
	delete m_groupEditDialog;
}

QDialog* PreProcessorGridAttributeNodeDataItem::propertyDialog(QWidget* p)
{
	auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	if (compAtt != nullptr && compAtt->isGrouped() == false) {
		return nullptr;
	}

	auto setting = colorMapSettingContainer();
	if (setting == nullptr) {return nullptr;}

	auto gitem = dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent());
	auto dialog = new PropertyDialog(gitem, p);
	dialog->setWindowTitle(tr("Grid Node Attribute Display Setting (%1)").arg(condition()->caption()));
	auto widget = m_condition->createColorMapSettingEditWidget(dialog);
	widget->setSetting(setting);
	dialog->setWidget(widget);

	dialog->setOpacity(gitem->opacity());
	dialog->resize(900, 700);

	return dialog;
}

void PreProcessorGridAttributeNodeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCustomModified = static_cast<bool>(node.toElement().attribute("isCustomModified", "0").toInt());
}

void PreProcessorGridAttributeNodeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString mod;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g != nullptr) {
		GridAttributeContainer* cont = g->gridAttribute(m_condition->name());
		mod.setNum(static_cast<int>(cont->isCustomModified()));
		writer.writeAttribute("isCustomModified", mod);
	}
}

int PreProcessorGridAttributeNodeDataItem::loadFromCgnsFile()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g == nullptr) {return IRIC_NO_ERROR;}

	GridAttributeContainer* cont = g->gridAttribute(m_condition->name());
	cont->setCustomModified(m_isCustomModified);

	return IRIC_NO_ERROR;
}

void PreProcessorGridAttributeNodeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_definingBoundingBox) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMouseMoveEvent(event, v);
	} else {
		auto setting = colorMapSettingContainer();
		if (setting != nullptr && setting->legendSetting()->getVisible()) {
			auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
			imgCtrl->handleMouseMoveEvent(this, event, v);
			if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
				return;
			}
		}

		dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->updateAttributeBrowser(event->pos(), v);
	}
}

void PreProcessorGridAttributeNodeDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr && setting->legendSetting()->getVisible()) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMousePressEvent(this, event, v);
		if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
			return;
		}
	}

	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		m_definingBoundingBox = true;
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMousePressEvent(event, v);
	}
}

void PreProcessorGridAttributeNodeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr && setting->legendSetting()->getVisible()) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMouseReleaseEvent(this, event, v);
	}

	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			auto v2 = dynamic_cast<VTK2DGraphicsView*> (v);
			tmpparent->nodeSelectingMouseReleaseEvent(event, v2);
		}
		m_definingBoundingBox = false;
		dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->fixAttributeBrowser(QPoint(event->x(), event->y()), v);
	} else if (event->button() == Qt::RightButton) {
		delete menu;
		menu = new QMenu(projectData()->mainWindow());
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		bool vertexSelected = (tmpparent->selectedVertices().size() > 0);
		menu->addAction(m_editValueAction);
		m_editValueAction->setEnabled(vertexSelected);
		bool groupedComplex = false;
		auto ccond = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
		if (ccond != nullptr) {groupedComplex = ccond->isGrouped();}

		if (! m_condition->isOption() && ! groupedComplex) {
			menu->addAction(m_editDifferenceAction);
			m_editDifferenceAction->setEnabled(vertexSelected);
			menu->addAction(m_editRatioAction);
			m_editRatioAction->setEnabled(vertexSelected);
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
	menu->addAction(m_generatePointMapAction);
	menu->addSeparator();
	menu->addAction(m_exportAction);
	menu->addSeparator();

	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gtitem)) {
		PreProcessorGridAttributeNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent());
		menu->addAction(gitem->showAttributeBrowserAction());
	}
}

void PreProcessorGridAttributeNodeDataItem::editValue()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}

	auto tItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	auto gridDataItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (compAtt != nullptr && compAtt->isGrouped() == false) {
		auto gItem = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemInterface*> (tItem->geoDataTop()->groupDataItem(m_condition->name()));
		if (gridDataItem->selectedVertices().size() > 2) {
			QMessageBox::warning(mainWindow(), tr("Warning"), tr("Please select only one node."));
			return;
		}
		auto selectedV = gridDataItem->selectedVertices().at(0);

		m_groupEditDialog->setWindowTitle(tr("Edit %1").arg(m_condition->caption()));
		m_groupEditDialog->setGroups(gItem->groups());
		m_groupEditDialog->setCurrentIndex(selectedV);
		m_groupEditDialog->exec();
	} else {
		GridAttributeEditDialog* dialog = m_condition->editDialog(mainWindow());
		dialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
		dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid nodes.")).arg(m_condition->caption()));
		PreProcessorGeoDataGroupDataItemInterface* i = tItem->geoDataTop()->groupDataItem(m_condition->name());
		i->setupEditWidget(dialog->widget());
		auto targets = gridDataItem->selectedVertices();
		Grid* g = gridDataItem->grid();
		dialog->scanAndSetDefault(g->gridAttribute(m_condition->name()), targets);

		if (QDialog::Accepted == dialog->exec()) {
			dialog->applyValue(g->gridAttribute(m_condition->name()), targets, g->vtkGrid()->GetPointData(), gridDataItem);
		}
		delete dialog;
	}
}

void PreProcessorGridAttributeNodeDataItem::editDifference()
{
	editVariation(GridAttributeVariationEditWidget::Difference, tr("difference"));
}

void PreProcessorGridAttributeNodeDataItem::editRatio()
{
	editVariation(GridAttributeVariationEditWidget::Ratio, tr("ratio"));
}

void PreProcessorGridAttributeNodeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(true);
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->initAttributeBrowser();

	GraphicsWindowDataItem::updateVisibility();
}

void PreProcessorGridAttributeNodeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(false);
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->clearAttributeBrowser();
}

SolverDefinitionGridAttribute* PreProcessorGridAttributeNodeDataItem::condition() const
{
	return m_condition;
}

void PreProcessorGridAttributeNodeDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(parent())->informDataChange(m_condition->name());
}

ColorMapSettingContainerI* PreProcessorGridAttributeNodeDataItem::colorMapSettingContainer() const
{
	auto typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	return typedi->colorMapSetting(m_condition->name());
}

ColorMapSettingToolBarWidgetController* PreProcessorGridAttributeNodeDataItem::colorMapSettingToolBarWidgetController() const
{
	return m_colorMapToolBarWidgetController;
}

void PreProcessorGridAttributeNodeDataItem::openCrossSectionWindow()
{
	auto gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());

	unsigned int index = gItem->selectedVertices().at(0);
	unsigned int i, j;

	Grid* g = gItem->grid();
	auto grid = dynamic_cast<Structured2DGrid*>(g);
	grid->getIJIndex(index, &i, &j);

	gItem->openCrossSectionWindow(PreProcessorGridCrosssectionWindow2::Direction::I, i);
}

void PreProcessorGridAttributeNodeDataItem::openVerticalCrossSectionWindow()
{
	auto gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());

	unsigned int index = gItem->selectedVertices().at(0);
	unsigned int i, j;

	Grid* g = gItem->grid();
	auto grid = dynamic_cast<Structured2DGrid*>(g);
	grid->getIJIndex(index, &i, &j);

	gItem->openCrossSectionWindow(PreProcessorGridCrosssectionWindow2::Direction::J, j);
}

void PreProcessorGridAttributeNodeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
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

void PreProcessorGridAttributeNodeDataItem::informSelectedVerticesChanged(const std::vector<vtkIdType>& vertices)
{
	for (auto item : m_crosssectionWindows) {
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
		TpoExporter exporter(mainWindow());
		if (! exporter.open(fname)) {
			throw ErrorMessage(tr("Error occured while opening the file."));
		}
		exporter.setOffset(offset());

		PreProcessorGridDataItem* gitem =  dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		vtkPointSet* vtkGrid = gitem->grid()->vtkGrid();
		vtkDataArray* da = vtkGrid->GetPointData()->GetArray(m_condition->name().c_str());

		// output values
		for (vtkIdType i = 0; i < vtkGrid->GetNumberOfPoints(); ++i) {
			double x[3];
			vtkGrid->GetPoint(i, x);
			double val = da->GetVariantValue(i).ToDouble();
			exporter.addPoint(x[0], x[1], val);
		}
		exporter.close();
		iricMainWindow()->statusBar()->showMessage(tr("Grid condition successfully exported to %1.").arg(QDir::toNativeSeparators(fname)), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
	} catch (ErrorMessage& message) {
		QMessageBox::critical(iricMainWindow(), tr("Error"), message);
	}
}

void PreProcessorGridAttributeNodeDataItem::generatePointMap()
{
	auto gItem = geoDataGroupDataItem();
	GeoDataCreator* creator = gItem->getPointMapCreator();
	if (creator == nullptr) {return;}

	auto gridDataItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	auto grid = gridDataItem->grid()->vtkGrid();
	auto points = grid->GetPoints();
	auto values = vtkDoubleArray::SafeDownCast(grid->GetPointData()->GetArray(m_condition->name().c_str()));

	GeoDataPointmapRealBuilder builder;
	builder.begin();

	double v[3], val;
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, v);
		val = values->GetValue(i);

		builder.addPoint(v[0], v[1], val);
	}

	auto item = gItem->buildGeoDataDataItem();
	GeoData* data = builder.end(item, creator, gItem->condition());
	creator->setNameAndDefaultCaption(gItem->childItems(), data);

	item->setGeoData(data);
	gItem->addGeoData(item);

	QMessageBox::information(mainWindow(), tr("Information"), tr("%1 generated.").arg(data->caption()));
}

void PreProcessorGridAttributeNodeDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	if (PreProcessorGridAttributeNodeGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(this->parent())) {
		gitem->updateActorSetting();
	}
}

void PreProcessorGridAttributeNodeDataItem::updateVisibility(bool /*visible*/)
{
	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);
	groupDataItem()->gridDataItem()->gridTypeDataItem()->updateColorBarVisibility(condition()->name());
}

PreProcessorGridTypeDataItem* PreProcessorGridAttributeNodeDataItem::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
}

PreProcessorGeoDataGroupDataItemInterface* PreProcessorGridAttributeNodeDataItem::geoDataGroupDataItem() const
{
	return gridTypeDataItem()->geoDataTop()->groupDataItem(m_condition->name());
}

PreProcessorGridAttributeNodeGroupDataItem* PreProcessorGridAttributeNodeDataItem::groupDataItem() const
{
	return dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*> (parent());
}

void PreProcessorGridAttributeNodeDataItem::editVariation(GridAttributeVariationEditWidget::Mode mode, const QString& typeName)
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridAttributeVariationEditDialog* dialog = m_condition->variationEditDialog(mainWindow());
	if (dialog == nullptr) {return;}
	dialog->setWindowTitle(QString(tr("Apply %1 to %2").arg(typeName).arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the %1 of %2 at the selected grid nodes.")).arg(typeName).arg(m_condition->caption()));
	dialog->widget()->setMode(mode);
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	auto targets = tmpparent->selectedVertices();
	Grid* g = tmpparent->grid();

	if (QDialog::Accepted == dialog->exec()) {
		auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
		if (compAtt != nullptr && compAtt->isGrouped() == false) {
			auto tItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
			auto gItem = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemInterface*> (tItem->geoDataTop()->groupDataItem(m_condition->name()));
			auto w = dynamic_cast<GridComplexConditionGroupRealEditWidget*> (dialog->widget());
			w->applyVariation(targets, gItem->groups());
			iRICUndoStack::instance().clear();
		} else {
			dialog->applyVariation(g->gridAttribute(m_condition->name()), targets, g->vtkGrid()->GetPointData(), tmpparent);
		}
	}
	delete dialog;
}

bool PreProcessorGridAttributeNodeDataItem::addToolBarButtons(QToolBar* toolBar)
{
	auto opacityW = groupDataItem()->opacityWidget();
	opacityW->setParent(toolBar);
	opacityW->show();
	toolBar->addWidget(opacityW);

	toolBar->addSeparator();

	auto cmwContainer = groupDataItem()->colorMapWidgetContainer();
	cmwContainer->setParent(toolBar);
	cmwContainer->show();
	toolBar->addWidget(cmwContainer);
	cmwContainer->setWidget(m_colorMapToolBarWidgetController->widget());

	Grid* grid = groupDataItem()->gridDataItem()->grid();
	GridAttributeContainer* cont = grid->gridAttribute(condition()->name());
	const auto& selectWidgets = cont->dimensions()->selectWidgets();
	if (selectWidgets.size() > 0) {
		toolBar->addSeparator();

		for (int i = 0; i < selectWidgets.size(); ++i) {
			GridAttributeDimensionSelectWidget* w = selectWidgets.at(i);
			QAction* action = toolBar->addWidget(w);
			action->setVisible(true);
		}
	}

	return true;
}
