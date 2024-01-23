#include "../preprocessorgraphicsview.h"
#include "attributebrowserhelper.h"
#include "preprocessorgridattributeabstractcelldataitem.h"
#include "preprocessorgridattributeabstractcellgroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "private/preprocessorgridattributeabstractcelldataitem_propertydialog.h"
#include "public/preprocessorgriddataitem_selectedcellscontroller.h"

#include <geodata/pointmap/geodatapointmaprealbuilder.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/direction/directionsettingeditwidget.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/complex/gridcomplexconditiongroupeditdialog.h>
#include <guicore/pre/complex/gridcomplexconditiongrouprealeditwidget.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/qwidgetcontainer.h>
#include <misc/tpoexporter.h>
#include <misc/valuechangert.h>
#include <misc/xmlsupport.h>

#include <iriclib_errorcodes.h>

#include <vtkPolyDataMapper.h>

PreProcessorGridAttributeAbstractCellDataItem::PreProcessorGridAttributeAbstractCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(cond->name(), cond->caption(), parent),
	m_directionSetting {},
	m_directionActor {vtkActor::New()},
	m_condition {cond},
	m_isCustomModified {"isCustomModified", false},
	m_definingBoundingBox {false},
	m_editValueAction {new QAction(tr("Edit value..."), this)},
	m_exportAction {new QAction(QIcon(":/libs/guibase/images/iconExport.svg"), tr("Export..."), this)},
	m_generatePointMapAction {new QAction(tr("Generate point cloud data"), this)},
	m_editDifferenceAction {new QAction(tr("Edit value by specifying difference..."), this)},
	m_editRatioAction {new QAction(tr("Edit value by specifying ratio..."), this)},
	m_colorMapToolBarWidgetController {nullptr}
{
	m_editValueAction->setDisabled(true);
	m_editDifferenceAction->setDisabled(true);
	m_editRatioAction->setDisabled(true);

	connect(m_editValueAction, &QAction::triggered, this, &PreProcessorGridAttributeAbstractCellDataItem::editValue);
	connect(m_exportAction, &QAction::triggered, this, &PreProcessorGridAttributeAbstractCellDataItem::exportToFile);
	connect(m_generatePointMapAction, &QAction::triggered, this, &PreProcessorGridAttributeAbstractCellDataItem::generatePointMap);
	connect(m_editDifferenceAction, &QAction::triggered, this, &PreProcessorGridAttributeAbstractCellDataItem::editDifference);
	connect(m_editRatioAction, &QAction::triggered, this, &PreProcessorGridAttributeAbstractCellDataItem::editRatio);

	auto gItem = geoDataGroupDataItem();
	GeoDataCreator* creator = gItem->getPointMapCreator();
	if (creator == nullptr) {
		m_generatePointMapAction->setDisabled(true);
	}

	m_colorMapToolBarWidgetController = gridTypeDataItem()->createToolBarWidgetController(cond->name(), mainWindow());

	m_directionActor->GetProperty()->LightingOff();
	renderer()->AddActor(m_directionActor);
}

PreProcessorGridAttributeAbstractCellDataItem::~PreProcessorGridAttributeAbstractCellDataItem()
{
	renderer()->RemoveActor(m_directionActor);

	m_directionActor->Delete();
}

QDialog* PreProcessorGridAttributeAbstractCellDataItem::propertyDialog(QWidget* p)
{
	auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	if (compAtt != nullptr && compAtt->isGrouped() == false) {
		return nullptr;
	}

	if (m_condition->isDirection()) {
		auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, mainWindow());
		dialog->setWindowTitle(tr("Grid %1 Attribute Display Setting (%2)").arg(positionCaption()).arg(condition()->caption()));
		auto widget = new DirectionSettingEditWidget(dialog);
		widget->setSetting(&m_directionSetting);
		dialog->setWidget(widget);

		return dialog;
	} else {
		auto setting = colorMapSettingContainer();
		if (setting == nullptr) {return nullptr;}

		auto gItem = groupDataItem();
		auto dialog = new PropertyDialog(gItem, p);
		dialog->setWindowTitle(tr("Grid %1 Attribute Display Setting (%2)").arg(positionCaption()).arg(condition()->caption()));
		auto widget = m_condition->createColorMapSettingEditWidget(dialog);
		widget->setSetting(setting);
		dialog->setWidget(widget);

		dialog->setLineWidth(gItem->lineWidth());
		dialog->setOpacity(gItem->opacity());
		dialog->resize(900, 700);

		return dialog;
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCustomModified.load(node);

	auto dirNode = iRIC::getChildNode(node, "Direction");
	if (! dirNode.isNull()) {
		return m_directionSetting.load(dirNode);
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	v4InputGrid* g = gridDataItem()->grid();
	if (g != nullptr) {
		auto cont = g->attribute(m_condition->name());
		m_isCustomModified = cont->isCustomModified();
		m_isCustomModified.save(writer);
	}

	if (m_condition->isDirection()) {
		writer.writeStartElement("Direction");
		m_directionSetting.save(writer);
		writer.writeEndElement();
	}
}

int PreProcessorGridAttributeAbstractCellDataItem::loadFromCgnsFile()
{
	v4InputGrid* g = gridDataItem()->grid();
	if (g == nullptr) {return IRIC_NO_ERROR;}

	auto cont = g->attribute(m_condition->name());
	cont->setCustomModified(m_isCustomModified);

	return IRIC_NO_ERROR;
}

void PreProcessorGridAttributeAbstractCellDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);
	gridTypeDataItem()->updateColorBarVisibility(condition()->name());
}

void PreProcessorGridAttributeAbstractCellDataItem::updateActorSetting()
{
	m_directionActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	if (m_condition->isDirection()) {
		auto data = groupDataItem()->filteredData();
		data->GetCellData()->SetActiveScalars(m_condition->name().c_str());

		auto view = dataModel()->graphicsView();
		auto polyData = m_directionSetting.buildDirectionPolygonData(data, m_condition, view);
		auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->ScalarVisibilityOff();
		mapper->SetInputData(polyData);
		m_directionActor->SetMapper(mapper);
		polyData->Delete();

		m_directionActor->GetProperty()->SetColor(m_directionSetting.color);
		m_directionActor->GetProperty()->SetOpacity(m_directionSetting.opacity);
		m_directionActor->GetProperty()->SetLineWidth(m_directionSetting.lineWidth);

		m_actorCollection->AddItem(m_directionActor);
		updateVisibilityWithoutRendering();
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_definingBoundingBox) {
		selectedDataController()->handleMouseMoveEvent(event, v);
	} else {
		auto setting = colorMapSettingContainer();
		if (setting != nullptr && setting->legendSetting()->getVisible()) {
			auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
			imgCtrl->handleMouseMoveEvent(this, event, v);
			if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
				return;
			}
		}

		groupDataItem()->updateAttributeBrowser(event->pos(), v);
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
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
		selectedDataController()->handleMousePressEvent(event, v);
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr && setting->legendSetting()->getVisible()) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMouseReleaseEvent(this, event, v);
	}
	auto gItem = gridDataItem();

	static QMenu* menu = nullptr;
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			selectedDataController()->handleMouseReleaseEvent(event, v);
		}
		m_definingBoundingBox = false;
		groupDataItem()->fixAttributeBrowser(event->pos(), v);
		v->setCursor(gItem->normalCursor());
	} else if (event->button() == Qt::RightButton) {
		delete menu;
		menu = new QMenu(projectData()->mainWindow());
		bool cellSelected = selectedDataController()->selectedDataIds().size() > 0;
		menu->addAction(m_editValueAction);
		m_editValueAction->setEnabled(cellSelected);
		bool groupedComplex = false;
		auto ccond = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
		if (ccond != nullptr) {groupedComplex = ccond->isGrouped();}

		if (! m_condition->isOption() && ! groupedComplex) {
			menu->addAction(m_editDifferenceAction);
			m_editDifferenceAction->setEnabled(cellSelected);
			menu->addAction(m_editRatioAction);
			m_editRatioAction->setEnabled(cellSelected);
		}
		menu->addSeparator();
		menu->addAction(groupDataItem()->showAttributeBrowserAction());
		menu->move(event->globalPos());
		menu->show();
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	selectedDataController()->handleKeyPressEvent(event, v);
}

void PreProcessorGridAttributeAbstractCellDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	selectedDataController()->handleKeyReleaseEvent(event, v);
}

void PreProcessorGridAttributeAbstractCellDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_generatePointMapAction);
	menu->addSeparator();
	menu->addAction(m_exportAction);
	menu->addSeparator();

	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gridTypeDataItem())) {
		menu->addAction(groupDataItem()->showAttributeBrowserAction());
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::editValue()
{
	auto mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	auto gItem = gridDataItem();
	auto tItem = gridTypeDataItem();
	auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	if (compAtt != nullptr && compAtt->isGrouped() == false) {
		auto cItem = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*> (tItem->geoDataTop()->groupDataItem(m_condition->name()));
		if (selectedDataController()->selectedDataIds().size() > 1) {
			QMessageBox::warning(mainWindow(), tr("Warning"), tr("Please select only one cell."));
			return;
		}
		auto selectedV = selectedDataController()->selectedDataIds().at(0);
		GridComplexConditionGroupEditDialog dialog(mainWindow());
		dialog.setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
		dialog.setGroups(cItem->groups());
		dialog.setCurrentIndex(selectedV);
		dialog.exec();
	} else {
		auto dialog = m_condition->editDialog(mainWindow());
		dialog->setWindowTitle(QString(tr("Edit %1").arg(m_condition->caption())));
		dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid cells.")).arg(m_condition->caption()));
		auto i = tItem->geoDataTop()->groupDataItem(m_condition->name());
		i->setupEditWidget(dialog->widget());
		const auto& targets = selectedDataController()->selectedDataIds();
		v4InputGrid* g = gItem->grid();
		dialog->scanAndSetDefault(g->attribute(m_condition->name()), targets);

		if (QDialog::Accepted == dialog->exec()) {
			auto cellData = groupDataItem()->data()->data()->GetCellData();
			dialog->applyValue(g->attribute(m_condition->name()), targets, cellData, gItem);
		}
		delete dialog;
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::editDifference()
{
	editVariation(GridAttributeVariationEditWidget::Difference, tr("difference"));
}

void PreProcessorGridAttributeAbstractCellDataItem::editRatio()
{
	editVariation(GridAttributeVariationEditWidget::Ratio, tr("ratio"));
}

void PreProcessorGridAttributeAbstractCellDataItem::showDisplaySettingDialog()
{
	showPropertyDialog();
}

void PreProcessorGridAttributeAbstractCellDataItem::exportToFile()
{
	iRICMainWindowI* mw = dataModel()->iricMainWindow();
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

		vtkPointSet* vtkGrid = groupDataItem()->data()->data();
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
		iricMainWindow()->statusBar()->showMessage(tr("Grid condition successfully exported to %1.").arg(QDir::toNativeSeparators(fname)), iRICMainWindowI::STATUSBAR_DISPLAYTIME);
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
	} catch (ErrorMessage& message) {
		QMessageBox::critical(iricMainWindow(), tr("Error"), message);
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::generatePointMap()
{
	auto gItem = geoDataGroupDataItem();
	GeoDataCreator* creator = gItem->getPointMapCreator();
	if (creator == nullptr) {return;}

	auto grid = groupDataItem()->data()->data();
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

void PreProcessorGridAttributeAbstractCellDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void PreProcessorGridAttributeAbstractCellDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void PreProcessorGridAttributeAbstractCellDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_directionActor->SetPosition(0, 0, range.min());
}

void PreProcessorGridAttributeAbstractCellDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	selectedDataController()->setVisibility(true);
	groupDataItem()->initAttributeBrowser();
	GraphicsWindowDataItem::updateVisibility();
}

void PreProcessorGridAttributeAbstractCellDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	selectedDataController()->setVisibility(false);
	groupDataItem()->clearAttributeBrowser();
}

SolverDefinitionGridAttribute* PreProcessorGridAttributeAbstractCellDataItem::condition() const
{
	return m_condition;
}

void PreProcessorGridAttributeAbstractCellDataItem::informDataChange()
{
	groupDataItem()->informDataChange(m_condition->name());
}

PreProcessorGridTypeDataItem* PreProcessorGridAttributeAbstractCellDataItem::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent()->parent());
}

PreProcessorGeoDataGroupDataItemI* PreProcessorGridAttributeAbstractCellDataItem::geoDataGroupDataItem() const
{
	return gridTypeDataItem()->geoDataTop()->groupDataItem(m_condition->name());
}

PreProcessorGridAttributeAbstractCellGroupDataItem* PreProcessorGridAttributeAbstractCellDataItem::groupDataItem() const
{
	return dynamic_cast<PreProcessorGridAttributeAbstractCellGroupDataItem*> (parent());
}

PreProcessorGridDataItem* PreProcessorGridAttributeAbstractCellDataItem::gridDataItem() const
{
	return groupDataItem()->gridDataItem();
}

void PreProcessorGridAttributeAbstractCellDataItem::editVariation(GridAttributeVariationEditWidget::Mode mode, const QString& typeName)
{
	auto mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	auto dialog = m_condition->variationEditDialog(mainWindow());
	if (dialog == nullptr) {return;}
	dialog->setWindowTitle(QString(tr("Apply %1 to %2").arg(typeName).arg(m_condition->caption())));
	dialog->setLabel(QString(tr("Input the %1 of %2 at the selected grid cells.")).arg(typeName).arg(m_condition->caption()));
	dialog->widget()->setMode(mode);
	auto gItem = gridDataItem();
	const auto& targets = selectedDataController()->selectedDataIds();
	v4InputGrid* g = gItem->grid();

	if (QDialog::Accepted == dialog->exec()) {
		auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
		if (compAtt != nullptr && compAtt->isGrouped() == false) {
			auto tItem = gridTypeDataItem();
			auto gItem = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*> (tItem->geoDataTop()->groupDataItem(m_condition->name()));
			auto w = dynamic_cast<GridComplexConditionGroupRealEditWidget*> (dialog->widget());
			w->applyVariation(targets, gItem->groups());
			iRICUndoStack::instance().clear();
		} else {
			dialog->applyVariation(g->attribute(m_condition->name()), targets, groupDataItem()->data()->data()->GetCellData(), gItem);
		}
	}
	delete dialog;
}

bool PreProcessorGridAttributeAbstractCellDataItem::addToolBarButtons(QToolBar* toolBar)
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

	v4InputGrid* grid = groupDataItem()->gridDataItem()->grid();
	auto att = grid->attribute(condition()->name());
	const auto& selectWidgets = att->dimensions()->selectWidgets();
	if (selectWidgets.size() > 0) {
		toolBar->addSeparator();

		for (int i = 0; i < static_cast<int> (selectWidgets.size()); ++i) {
			auto w = selectWidgets.at(i);
			QAction* action = toolBar->addWidget(w);
			action->setVisible(true);
		}
	}

	return true;
}

ColorMapSettingContainerI* PreProcessorGridAttributeAbstractCellDataItem::colorMapSettingContainer() const
{
	return gridTypeDataItem()->colorMapSetting(m_condition->name());
}

ColorMapSettingToolBarWidgetController* PreProcessorGridAttributeAbstractCellDataItem::colorMapSettingToolBarWidgetController() const
{
	return m_colorMapToolBarWidgetController;
}
