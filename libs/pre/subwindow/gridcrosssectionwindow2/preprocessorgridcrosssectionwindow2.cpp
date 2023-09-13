#include "preprocessorgridcrosssectionwindow2.h"
#include "ui_preprocessorgridcrosssectionwindow2.h"
#include "private/preprocessorgridcrosssectionwindow2_controller.h"
#include "private/preprocessorgridcrosssectionwindow2_csvexportcontroller.h"
#include "private/preprocessorgridcrosssectionwindow2_displaysettingdialog.h"
#include "private/preprocessorgridcrosssectionwindow2_displaysettingtablecontroller.h"
#include "private/preprocessorgridcrosssectionwindow2_edittablecontroller.h"
#include "private/preprocessorgridcrosssectionwindow2_impl.h"
#include "private/preprocessorgridcrosssectionwindow2_snapshotsavecontroller.h"
#include "private/preprocessorgridcrosssectionwindow2_updatecommand.h"
#include "private/preprocessorgridcrosssectionwindow2_updategraphicsviewcommand.h"
#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridtypedataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/windowgeometrycontainer.h>
#include <misc/xmlsupport.h>

PreProcessorGridCrosssectionWindow2::PreProcessorGridCrosssectionWindow2(PreProcessorGridDataItem* item, QWidget *parent) :
	QMainWindowWithSnapshot {parent},
	impl {new Impl {item, this}},
	ui(new Ui::PreProcessorGridCrosssectionWindow2)
{
	ui->setupUi(this);
	ui->graphicsView->setImpl(impl);

	impl->setupSplitterSizes();
	impl->setupToolBars();
	impl->setupController();
	impl->setupDisplaySettings();
	impl->setupDisplaySettingTableController();
	impl->setupEditTableController();
}

PreProcessorGridCrosssectionWindow2::~PreProcessorGridCrosssectionWindow2()
{
	impl->m_gridDataItem->removeCrossSectionWindow(this);

	delete ui;
	delete impl;
}

void PreProcessorGridCrosssectionWindow2::loadFromProjectMainFile(const QDomNode& node)
{
	WindowGeometryContainer geometry;
	geometry.setWidget(dynamic_cast<QWidget*> (parent()));
	geometry.load(node);

	impl->m_tmpDirection = static_cast<Direction> (iRIC::getIntAttribute(node, "direction"));
	impl->m_tmpIndex = iRIC::getIntAttribute(node, "index");
	impl->m_tmpCellSide = static_cast<Controller::CellSide> (iRIC::getIntAttribute(node, "cellside"));

	impl->graphicsView()->loadFromProjectMainFile(node);

	auto dsNode = iRIC::getChildNode(node, "DisplaySetting");
	if (! dsNode.isNull()) {
		impl->m_displaySetting.load(dsNode);
		impl->m_displaySetting.applyToWidgets(impl);
	}
	auto gaNode = iRIC::getChildNode(node, "GridAttributeDisplaySettings");
	if (! gaNode.isNull()) {
		const auto& list = gaNode.childNodes();
		for (int i = 0; i < list.size(); ++i) {
			const auto& e = list.at(i).toElement();
			auto attName = iRIC::toStr(e.attribute("attribute"));
			auto s = impl->m_displaySettingMap.at(attName);
			s->load(e);
		}
	}
}

void PreProcessorGridCrosssectionWindow2::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	WindowGeometryContainer geometry;
	geometry.setWidget(dynamic_cast<QWidget*> (parent()));
	geometry.save(writer);

	iRIC::setIntAttribute(writer, "direction", static_cast<int>(impl->m_controller->targetDirection()));
	iRIC::setIntAttribute(writer, "index", impl->m_controller->targetIndex());
	iRIC::setIntAttribute(writer, "cellside", static_cast<int>(impl->m_controller->cellSide()));

	impl->graphicsView()->saveToProjectMainFile(writer);

	writer.writeStartElement("DisplaySetting");
	impl->m_displaySetting.applyFromWidgets(impl);
	impl->m_displaySetting.save(writer);
	writer.writeEndElement();

	writer.writeStartElement("GridAttributeDisplaySettings");
	for (const auto& s : impl->m_displaySettings) {
		writer.writeStartElement("Setting");
		writer.writeAttribute("attribute", s.attribute()->name().c_str());
		s.save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void PreProcessorGridCrosssectionWindow2::setTarget(Direction dir, int index)
{
	impl->setTargetDirection(dir);
	impl->m_controller->setTarget(dir, index);
	cameraFit();
}

PreProcessorGridCrosssectionWindow2::Direction PreProcessorGridCrosssectionWindow2::targetDirection() const
{
	return impl->m_controller->targetDirection();
}

int PreProcessorGridCrosssectionWindow2::targetIndex() const
{
	return impl->m_controller->targetIndex();
}

void PreProcessorGridCrosssectionWindow2::applyTmpTargetSetting()
{
	impl->m_controller->setCellSide(impl->m_tmpCellSide);
	impl->m_controller->setTarget(impl->m_tmpDirection, impl->m_tmpIndex);
}

void PreProcessorGridCrosssectionWindow2::update()
{
	impl->m_displaySettingTableController->updateVisible();
	impl->m_editTableController->applyToTable();
	updateGraphicsView();
}

void PreProcessorGridCrosssectionWindow2::applyColorMapSetting(const std::string& name)
{
	auto it = impl->m_displaySettingMap.find(name);
	if (it == impl->m_displaySettingMap.end()) {return;}

	auto cs = impl->preColorMapSetting(name);
	if (cs == nullptr) {return;}

	it->second->colorMapSetting->setAutoValueRange(cs->autoMinValue, cs->autoMaxValue);
	updateGraphicsView();
}

QPixmap PreProcessorGridCrosssectionWindow2::snapshot() const
{
	auto gv = impl->graphicsView();

	QPixmap pixmap(gv->size() * devicePixelRatioF());
	pixmap.setDevicePixelRatio(devicePixelRatioF());

	QBrush brush = QBrush(Qt::white);
	QPainter painter;
	painter.begin(&pixmap);
	QRect rect = pixmap.rect();
	painter.fillRect(rect, brush);
	QRegion region(0, 0, pixmap.width(), pixmap.height());
	gv->render(&painter, QPoint(), region, QWidget::DrawChildren);
	painter.end();
	return pixmap;
}

QWidget* PreProcessorGridCrosssectionWindow2::snapshotArea() const
{
	return impl->graphicsView();
}

bool PreProcessorGridCrosssectionWindow2::saveCsvFile(const QString& fileName)
{
	return impl->m_editTableController->saveCsvFile(fileName);
}

void PreProcessorGridCrosssectionWindow2::updateGraphicsView()
{
	ui->graphicsView->viewport()->update();
}

void PreProcessorGridCrosssectionWindow2::cameraFit()
{
	ui->graphicsView->cameraFit();
}

void PreProcessorGridCrosssectionWindow2::openDisplaySettingDialog()
{
	auto dialog = new DisplaySettingDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->show();
}

void PreProcessorGridCrosssectionWindow2::saveSnapshots()
{
	static SnapshotSaveController controller(this);

	controller.save();
}

void PreProcessorGridCrosssectionWindow2::exportCsvs()
{
	static CsvExportController controller(this);

	controller.save();
}

void PreProcessorGridCrosssectionWindow2::pushUpdateCommand(QUndoCommand* command)
{
	iRICUndoStack::instance().push(new UpdateCommand(command, this));
}

void PreProcessorGridCrosssectionWindow2::pushUpdateGraphicsViewCommand(QUndoCommand* command)
{
	iRICUndoStack::instance().push(new UpdateGraphicsViewCommand(command, this));
}
