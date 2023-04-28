#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "attributebrowserhelper.h"
#include "preprocessorgridattributecelldataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "private/preprocessorgridattributecelldataitem_propertydialog.h"

#include <geodata/pointmap/geodatapointmaprealbuilder.h>
#include <guibase/widget/contoursettingwidget.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/complex/gridcomplexconditiongroupeditdialog.h>
#include <guicore/pre/complex/gridcomplexconditiongrouprealeditwidget.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/tpoexporter.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>
#include <QStatusBar>
#include <QVector>
#include <QXmlStreamWriter>

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkCollectionIterator.h>
#include <vtkRenderer.h>

#include <iriclib_errorcodes.h>

#include <string>

PreProcessorGridAttributeCellDataItem::PreProcessorGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(cond->name(), cond->caption(), parent),
	m_condition {cond},
	m_isCustomModified {"isCustomModified", false},
	m_definingBoundingBox {false}
{
	m_editValueAction = new QAction(PreProcessorGridAttributeCellDataItem::tr("Edit value..."), this);
	m_editValueAction->setDisabled(true);
	connect(m_editValueAction, SIGNAL(triggered()), this, SLOT(editValue()));

	m_exportAction = new QAction(QIcon(":/libs/guibase/images/iconExport.svg") ,PreProcessorGridAttributeCellDataItem::tr("Export..."), this);
	connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportToFile()));

	m_generatePointMapAction = new QAction(PreProcessorGridAttributeCellDataItem::tr("Generate point cloud data"), this);
	connect(m_generatePointMapAction, SIGNAL(triggered()), this, SLOT(generatePointMap()));

	m_editDifferenceAction = new QAction(PreProcessorGridAttributeCellDataItem::tr("Edit value by specifying difference..."), this);
	m_editDifferenceAction->setDisabled(true);
	connect(m_editDifferenceAction, SIGNAL(triggered()), this, SLOT(editDifference()));

	m_editRatioAction = new QAction(PreProcessorGridAttributeCellDataItem::tr("Edit value by specifying ratio..."), this);
	m_editRatioAction->setDisabled(true);
	connect(m_editRatioAction, SIGNAL(triggered()), this, SLOT(editRatio()));

	auto gItem = geoDataGroupDataItem();
	GeoDataCreator* creator = gItem->getPointMapCreator();
	if (creator == nullptr) {
		m_generatePointMapAction->setDisabled(true);
	}

	auto imgSetting = gridTypeDataItem()->colorMapSetting(condition()->name())->legendSetting()->imgSetting();
	imgSetting->controller()->addItem(this);

	m_colorMapToolBarWidgetController = gridTypeDataItem()->createToolBarWidgetController(cond->name(), mainWindow());
}

PreProcessorGridAttributeCellDataItem::~PreProcessorGridAttributeCellDataItem()
{
	auto gtItem = gridTypeDataItem();
	if (gtItem != nullptr) {
		auto imgSetting = gtItem->colorMapSetting(condition()->name())->legendSetting()->imgSetting();
		imgSetting->controller()->removeItem(this);
	}
}

QDialog* PreProcessorGridAttributeCellDataItem::propertyDialog(QWidget* p)
{
	auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	if (compAtt != nullptr && compAtt->isGrouped() == false) {
		return nullptr;
	}

	auto setting = colorMapSettingContainer();
	if (setting == nullptr) {return nullptr;}

	auto gitem = dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent());
	auto dialog = new PropertyDialog(gitem, p);
	dialog->setWindowTitle(tr("Grid Cell Attribute Display Setting (%1)").arg(condition()->caption()));
	auto widget = m_condition->createColorMapSettingEditWidget(dialog);
	widget->setSetting(setting);
	dialog->setWidget(widget);

	dialog->setOpacityPercent(gitem->opacityPercent());

	return dialog;
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

int PreProcessorGridAttributeCellDataItem::loadFromCgnsFile()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Grid* g = tmpparent->grid();
	if (g == nullptr) {return IRIC_NO_ERROR;}

	GridAttributeContainer* cont = g->gridAttribute(m_condition->name());
	cont->setCustomModified(m_isCustomModified);

	return IRIC_NO_ERROR;
}

void PreProcessorGridAttributeCellDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	auto v = dataModel()->graphicsView();
	colorMapSettingContainer()->legendSetting()->imgSetting()->apply(v);
}

void PreProcessorGridAttributeCellDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMouseMoveEvent(event, v);
		if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
			return;
		}
	}

	if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMouseMoveEvent(event, v);
	} else {
		dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->updateAttributeBrowser(QPoint(event->x(), event->y()), v);
	}
}

void PreProcessorGridAttributeCellDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMousePressEvent(event, v);
		if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
			return;
		}
	}

	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		m_definingBoundingBox = true;
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMousePressEvent(event, v);
	}
}

void PreProcessorGridAttributeCellDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMouseReleaseEvent(event, v);
		if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
			return;
		}
	}

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
		bool cellSelected = tmpparent->selectedCells().count() > 0;
		menu->addAction(m_editValueAction);
		m_editValueAction->setEnabled(cellSelected);
		bool nonGroupedComplex = false;
		auto ccond = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
		if (ccond != nullptr) {nonGroupedComplex = ccond->isGrouped();}

		if (! m_condition->isOption() || nonGroupedComplex) {
			menu->addAction(m_editDifferenceAction);
			m_editDifferenceAction->setEnabled(cellSelected);
			menu->addAction(m_editRatioAction);
			m_editRatioAction->setEnabled(cellSelected);
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
	menu->addAction(m_generatePointMapAction);
	menu->addSeparator();
	menu->addAction(m_exportAction);
	menu->addSeparator();

	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gtitem)) {
		PreProcessorGridAttributeCellGroupDataItem* gitem = dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent());
		menu->addAction(gitem->showAttributeBrowserAction());
	}
}

void PreProcessorGridAttributeCellDataItem::editValue()
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
		GridComplexConditionGroupEditDialog dialog(mainWindow());
		dialog.setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
		dialog.setGroups(gItem->groups());
		dialog.setCurrentIndex(selectedV);
		dialog.exec();
	} else {
		GridAttributeEditDialog* dialog = m_condition->editDialog(mainWindow());
		dialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
		dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid cells.")).arg(m_condition->caption()));
		PreProcessorGeoDataGroupDataItemInterface* i = tItem->geoDataTop()->groupDataItem(m_condition->name());
		i->setupEditWidget(dialog->widget());
		QVector<vtkIdType> targets = gridDataItem->selectedCells();
		Grid* g = gridDataItem->grid();
		dialog->scanAndSetDefault(g->gridAttribute(m_condition->name()), targets);

		if (QDialog::Accepted == dialog->exec()) {
			dialog->applyValue(g->gridAttribute(m_condition->name()), targets, g->vtkGrid()->GetCellData(), gridDataItem);
		}
		delete dialog;
	}
}

void PreProcessorGridAttributeCellDataItem::editDifference()
{
	editVariation(GridAttributeVariationEditWidget::Difference, tr("difference"));
}

void PreProcessorGridAttributeCellDataItem::editRatio()
{
	editVariation(GridAttributeVariationEditWidget::Ratio, tr("ratio"));
}

void PreProcessorGridAttributeCellDataItem::exportToFile()
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
		vtkDataArray* da = vtkGrid->GetCellData()->GetArray(m_condition->name().c_str());

		// output values
		for (vtkIdType i = 0; i < vtkGrid->GetNumberOfCells(); ++i) {
			double val = da->GetVariantValue(i).ToDouble();
			double xavg[3];
			double x[3];

			xavg[0] = 0; xavg[1] = 0; xavg[2] = 0;

			auto cell = vtkGrid->GetCell(i);
			vtkIdType numP = cell->GetNumberOfPoints();
			for (int j = 0; j < numP; ++j) {
				vtkIdType vId = cell->GetPointId(j);
				vtkGrid->GetPoint(vId, x);
				for (int k = 0; k < 2; ++k) {
					xavg[k] += x[k];
				}
			}
			for (int k = 0; k < 2; ++k) {
				xavg[k] /= numP;
			}

			exporter.addPoint(xavg[0], xavg[1], val);
		}
		exporter.close();
		iricMainWindow()->statusBar()->showMessage(tr("Grid condition successfully exported to %1.").arg(QDir::toNativeSeparators(fname)), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
	} catch (ErrorMessage& message) {
		QMessageBox::critical(iricMainWindow(), tr("Error"), message);
	}
}

void PreProcessorGridAttributeCellDataItem::generatePointMap()
{
	auto gItem = geoDataGroupDataItem();
	GeoDataCreator* creator = gItem->getPointMapCreator();
	if (creator == nullptr) {return;}

	auto gridDataItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	auto grid = gridDataItem->grid()->vtkGrid();
	auto points = grid->GetPoints();
	auto values = vtkDoubleArray::SafeDownCast(grid->GetCellData()->GetArray(m_condition->name().c_str()));

	GeoDataPointmapRealBuilder builder;
	builder.begin();

	double xavg[3], x[3], val;

	for (vtkIdType i = 0; i < grid->GetNumberOfCells(); ++i) {
		xavg[0] = 0; xavg[1] = 0; xavg[2] = 0;
		auto cell = grid->GetCell(i);
		vtkIdType numP = cell->GetNumberOfPoints();
		for (int j = 0; j < numP; ++j) {
			vtkIdType vId = cell->GetPointId(j);
			points->GetPoint(vId, x);
			for (int k = 0; k < 2; ++k) {
				xavg[k] += x[k];
			}
		}
		for (int k = 0; k < 2; ++k) {
			xavg[k] /= numP;
		}
		val = values->GetValue(i);

		builder.addPoint(x[0], x[1], val);
	}

	auto item = gItem->buildGeoDataDataItem();
	GeoData* data = builder.end(item, creator, gItem->condition());
	creator->setNameAndDefaultCaption(gItem->childItems(), data);

	item->setGeoData(data);
	gItem->addGeoData(item);

	QMessageBox::information(mainWindow(), tr("Information"), tr("%1 generated.").arg(data->caption()));
}

void PreProcessorGridAttributeCellDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void PreProcessorGridAttributeCellDataItem::informSelection(VTKGraphicsView* v)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(true);
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->initAttributeBrowser();
	GraphicsWindowDataItem::updateVisibility();
}

void PreProcessorGridAttributeCellDataItem::informDeselection(VTKGraphicsView* v)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(false);
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->clearAttributeBrowser();
}

void PreProcessorGridAttributeCellDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(parent())->informDataChange(m_condition->name());
}

PreProcessorGridTypeDataItem* PreProcessorGridAttributeCellDataItem::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
}

PreProcessorGeoDataGroupDataItemInterface* PreProcessorGridAttributeCellDataItem::geoDataGroupDataItem() const
{
	return gridTypeDataItem()->geoDataTop()->groupDataItem(m_condition->name());
}

PreProcessorGridAttributeCellGroupDataItem* PreProcessorGridAttributeCellDataItem::groupDataItem() const
{
	return dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*> (parent());
}

void PreProcessorGridAttributeCellDataItem::editVariation(GridAttributeVariationEditWidget::Mode mode, const QString& typeName)
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	GridAttributeVariationEditDialog* dialog = m_condition->variationEditDialog(mainWindow());
	if (dialog == nullptr) {return;}
	dialog->setWindowTitle(QString(tr("Apply %1 to %2").arg(typeName).arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the %1 of %2 at the selected grid cells.")).arg(typeName).arg(m_condition->caption()));
	dialog->widget()->setMode(mode);
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	QVector<vtkIdType> targets = tmpparent->selectedCells();
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
			dialog->applyVariation(g->gridAttribute(m_condition->name()), targets, g->vtkGrid()->GetCellData(), tmpparent);
		}
	}
	delete dialog;
}

bool PreProcessorGridAttributeCellDataItem::addToolBarButtons(QToolBar* toolbar)
{
	PreProcessorDataItem* item =
		dynamic_cast<PreProcessorDataItem*>(parent());
	return item->addToolBarButtons(toolbar);
}

ColorMapSettingContainerI* PreProcessorGridAttributeCellDataItem::colorMapSettingContainer() const
{
	auto typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
	return typedi->colorMapSetting(m_condition->name());
}

ColorMapSettingToolBarWidgetController* PreProcessorGridAttributeCellDataItem::colorMapSettingToolBarWidgetController() const
{
	return m_colorMapToolBarWidgetController;
}
