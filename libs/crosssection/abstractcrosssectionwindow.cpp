#include "abstractcrosssectionwindow.h"
#include "ui_abstractcrosssectionwindow.h"
#include "private/abstractcrosssectionwindow_csvexportcontroller.h"
#include "private/abstractcrosssectionwindow_displaysettingdialog.h"
#include "private/abstractcrosssectionwindow_displaysettingtablecontroller.h"
#include "private/abstractcrosssectionwindow_edittablecontroller.h"
#include "private/abstractcrosssectionwindow_impl.h"
#include "private/abstractcrosssectionwindow_snapshotsavecontroller.h"
#include "private/abstractcrosssectionwindow_updatecommand.h"
#include "private/abstractcrosssectionwindow_updategraphicsviewcommand.h"
#include "public/abstractcrosssectionwindow_controller.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/windowgeometrycontainer.h>
#include <misc/xmlsupport.h>

AbstractCrosssectionWindow::AbstractCrosssectionWindow(QWidget *parent) :
	QWidget {parent},
	impl {new Impl {this, parent}},
	ui(new Ui::AbstractCrosssectionWindow)
{
	ui->setupUi(this);
	ui->graphicsView->setImpl(impl);

	impl->setupSplitterSizes();
	impl->setupToolBars();
	impl->setupController(parent);
	impl->setupDisplaySettingTableController();
	impl->setupEditTableController();
}

AbstractCrosssectionWindow::~AbstractCrosssectionWindow()
{
	// impl->m_gridDataItem->removeCrossSectionWindow(this);

	delete ui;
	delete impl;
}

void AbstractCrosssectionWindow::loadFromProjectMainFile(const QDomNode& node)
{
	WindowGeometryContainer geometry;
	geometry.setWidget(dynamic_cast<QWidget*> (parent()));
	geometry.load(node);

	impl->m_tmpDirection = static_cast<Direction> (iRIC::getIntAttribute(node, "direction"));
	impl->m_tmpIndex = iRIC::getIntAttribute(node, "positionIndex");
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
			auto it = impl->m_displaySettingMap.find(attName);
			if (it != impl->m_displaySettingMap.end()) {
				it->second->load(e);
			}
		}
	}

	applyTmpTargetSetting();
}

void AbstractCrosssectionWindow::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	WindowGeometryContainer geometry;
	geometry.setWidget(dynamic_cast<QWidget*> (parent()));
	geometry.save(writer);

	iRIC::setIntAttribute(writer, "direction", static_cast<int>(impl->m_controller->targetDirection()));
	iRIC::setIntAttribute(writer, "positionIndex", impl->m_controller->targetIndex());
	iRIC::setIntAttribute(writer, "cellside", static_cast<int>(impl->m_controller->cellSide()));

	impl->graphicsView()->saveToProjectMainFile(writer);

	writer.writeStartElement("DisplaySetting");
	impl->m_displaySetting.applyFromWidgets(impl);
	impl->m_displaySetting.save(writer);
	writer.writeEndElement();

	writer.writeStartElement("GridAttributeDisplaySettings");
	for (const auto& s : impl->m_displaySettings) {
		writer.writeStartElement("Setting");
		writer.writeAttribute("attribute", s.attributeName().c_str());
		s.save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void AbstractCrosssectionWindow::setupDisplaySettings()
{
	impl->setupDisplaySettings();
	impl->m_displaySettingTableController->applyToTable();
}

void AbstractCrosssectionWindow::setTarget(Direction dir, int index)
{
	impl->setTargetDirection(dir);
	impl->m_controller->setTarget(dir, index);
	cameraFit();
}

AbstractCrosssectionWindow::Direction AbstractCrosssectionWindow::targetDirection() const
{
	return impl->m_controller->targetDirection();
}

int AbstractCrosssectionWindow::targetIndex() const
{
	return impl->m_controller->targetIndex();
}

void AbstractCrosssectionWindow::applyTmpTargetSetting()
{
	impl->m_controller->setCellSide(impl->m_tmpCellSide);
	impl->m_controller->setTarget(impl->m_tmpDirection, impl->m_tmpIndex);
}

void AbstractCrosssectionWindow::update()
{
	impl->updateColorMapValueRanges();
	impl->m_displaySettingTableController->updateVisible();
	impl->m_editTableController->applyToTable();

	updateGraphicsView();
}

void AbstractCrosssectionWindow::handleSettingChange()
{
	update();

	if (impl->m_displaySetting.enableAutoRescale && ! impl->m_displaySetting.fixRegion) {
		cameraFit();
	}
}

void AbstractCrosssectionWindow::applyColorMapSetting(const std::string& name)
{
	auto it = impl->m_displaySettingMap.find(name);
	if (it == impl->m_displaySettingMap.end()) {return;}

	auto cs = preColorMapSetting(name);
	if (cs == nullptr) {return;}

	it->second->colorMapSetting->setAutoValueRange(cs->autoMinValue, cs->autoMaxValue);
	updateGraphicsView();
}

QPixmap AbstractCrosssectionWindow::snapshot() const
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

QWidget* AbstractCrosssectionWindow::graphicsView() const
{
	return impl->graphicsView();
}

bool AbstractCrosssectionWindow::saveCsvFile(const QString& fileName)
{
	return impl->m_editTableController->saveCsvFile(fileName);
}

QToolBar* AbstractCrosssectionWindow::viewToolBar() const
{
	return impl->m_viewToolBar;
}

QToolBar* AbstractCrosssectionWindow::displayToolBar() const
{
	return impl->m_displayToolBar;
}

v4Structured2dGrid* AbstractCrosssectionWindow::additionalGrid()
{
	return nullptr;
}

QString AbstractCrosssectionWindow::additionalGridPrefix()
{
	return "";
}

AbstractCrosssectionWindow::Controller* AbstractCrosssectionWindow::controller() const
{
	return impl->m_controller;
}

void AbstractCrosssectionWindow::updateGraphicsView()
{
	ui->graphicsView->viewport()->update();
}

void AbstractCrosssectionWindow::cameraFit()
{
	ui->graphicsView->cameraFit();
}

void AbstractCrosssectionWindow::openDisplaySettingDialog()
{
	auto dialog = new DisplaySettingDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->show();
}

void AbstractCrosssectionWindow::saveSnapshots()
{
	static SnapshotSaveController controller(this);

	controller.save();
}

void AbstractCrosssectionWindow::exportCsvs()
{
	static CsvExportController controller(this);

	controller.save();
}

void AbstractCrosssectionWindow::pushUpdateCommand(QUndoCommand* command)
{
	iRICUndoStack::instance().push(new UpdateCommand(command, this));
}

void AbstractCrosssectionWindow::pushUpdateGraphicsViewCommand(QUndoCommand* command)
{
	iRICUndoStack::instance().push(new UpdateGraphicsViewCommand(command, this));
}

ColorMapSettingContainerI* AbstractCrosssectionWindow::preColorMapSetting(const std::string& name) const
{
	return nullptr;
}
