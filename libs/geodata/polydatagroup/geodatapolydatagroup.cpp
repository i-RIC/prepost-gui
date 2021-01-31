#include "geodatapolydatagroup.h"
#include "geodatapolydatagroupattributebrowser.h"
#include "geodatapolydatagroupcreator.h"
#include "geodatapolydatagroupcopysettingdialog.h"
#include "geodatapolydatagroupmergesettingdialog.h"
#include "geodatapolydatagrouppolydata.h"
#include "private/geodatapolydatagroup_impl.h"
#include "private/geodatapolydatagroup_editnameandvaluecommand.h"
#include "private/geodatapolydatagroup_setcolorsettingcommand.h"
#include "private/geodatapolydatagroup_sortcommand.h"
#include "private/geodatapolydatagroup_sortedittargetdatacommand.h"

#include <geodata/polydata/geodatapolydata.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditnameandvaluedialog.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>

#include <vtkActorCollection.h>
#include <vtkRenderWindow.h>

#include <geos/geom/Envelope.h>
#include <geos/index/quadtree/Quadtree.h>

namespace {

int indexOfFirstSelected(const std::vector<GeoDataPolyDataGroupPolyData*>& data, const std::unordered_set<GeoDataPolyDataGroupPolyData*>& selectedData)
{
	for (auto it = data.begin(); it != data.end(); ++it) {
		if (selectedData.find(*it) != selectedData.end()) {
			return (it - data.begin());
		}
	}
	return -1;
}

std::vector<GeoDataPolyDataGroupPolyData*> getSelectedDataVector(const std::vector<GeoDataPolyDataGroupPolyData*>& data, const std::unordered_set<GeoDataPolyDataGroupPolyData*>& selectedData)
{
	std::vector<GeoDataPolyDataGroupPolyData*> ret;
	for (auto d : data) {
		if (selectedData.find(d) == selectedData.end()) {continue;}

		ret.push_back(d);
	}
	return ret;
}

std::vector<GeoDataPolyDataGroupPolyData*> getNotSelectedDataVector(const std::vector<GeoDataPolyDataGroupPolyData*>& data, const std::unordered_set<GeoDataPolyDataGroupPolyData*>& selectedData)
{
	std::vector<GeoDataPolyDataGroupPolyData*> ret;
	for (auto d : data) {
		if (selectedData.find(d) != selectedData.end()) {continue;}

		ret.push_back(d);
	}
	return ret;
}

} // namespace

GeoDataPolyDataGroup::GeoDataPolyDataGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData(d, creator, condition),
	impl {new Impl {this}}
{
	impl->m_attributeBrowser->setPolyDataGroup(this);
	preProcessorWindow()->addDockWidget(Qt::LeftDockWidgetArea, impl->m_attributeBrowser);
	impl->m_attributeBrowser->hide();

	auto att = gridAttribute();
	if (att && att->isReferenceInformation()) {
		impl->m_colorSetting.mapping = GeoDataPolyDataGroupColorSettingDialog::Arbitrary;
	}

	makeConnections();
}

GeoDataPolyDataGroup::~GeoDataPolyDataGroup()
{
	delete impl;
}

void GeoDataPolyDataGroup::updateIndex()
{
	delete impl->m_qTree;
	impl->m_qTree = new geos::index::quadtree::Quadtree();

	for (auto d : impl->m_data) {
		double xmin, xmax, ymin, ymax;
		d->getBoundingRect(&xmin, &xmax, &ymin, &ymax);
		auto env = new geos::geom::Envelope(xmin, xmax, ymin, ymax);
		impl->m_qTree->insert(env, d);
	}
}

void GeoDataPolyDataGroup::updateOrder()
{
	for (int i = 0; i < impl->m_data.size(); ++i) {
		auto d = impl->m_data.at(i);
		d->setOrder(i);
	}
}

bool GeoDataPolyDataGroup::getValueRange(double* min, double* max)
{
	if (impl->m_data.size() == 0 && impl->m_editTargetData == nullptr) {return false;}

	bool first = true;
	for (auto p : impl->m_data) {
		double v = p->value().toDouble();
		if (first || v < *min) {*min = v;}
		if (first || v > *max) {*max = v;}
		first = false;
	}

	if (impl->m_editTargetData != nullptr) {
		double v = impl->m_editTargetData->variantValue().toDouble();
		if (first || v < *min) {*min = v;}
		if (first || v > *max) {*max = v;}
	}
	return true;
}

void GeoDataPolyDataGroup::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataPolyDataGroup::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_addAction);
	menu->addSeparator();
	menu->addAction(m_editNameAction);
	menu->addSeparator();
	menu->addAction(impl->m_copyAction);
	menu->addSeparator();
}

bool GeoDataPolyDataGroup::addToolBarButtons(QToolBar* /*parent*/)
{
	return false;
}

void GeoDataPolyDataGroup::showInitialDialog()
{
	addData();
}

QDialog* GeoDataPolyDataGroup::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPolyDataGroupColorSettingDialog(parent);
	dialog->setSetting(impl->m_colorSetting);
	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		dialog->setIsReferenceInformation(gridAtt->isReferenceInformation());
	}
	return dialog;
}

void GeoDataPolyDataGroup::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<GeoDataPolyDataGroupColorSettingDialog*> (d);
	pushRenderCommand(new SetColorSettingCommand(dialog->setting(), this));
}

void GeoDataPolyDataGroup::viewOperationEnded(PreProcessorGraphicsViewInterface* /*v*/)
{}

void GeoDataPolyDataGroup::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode != Mode::EditingData) {return;}

	impl->m_editTargetData->keyPressEvent(event, v);
}

void GeoDataPolyDataGroup::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode != Mode::EditingData) {return;}

	impl->m_editTargetData->keyReleaseEvent(event, v);
}

void GeoDataPolyDataGroup::mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode != Mode::EditingData) {return;}

	impl->m_editTargetData->mouseDoubleClickEvent(event, v);
}

void GeoDataPolyDataGroup::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Mode::EditingData) {
		impl->m_editTargetData->mouseMoveEvent(event, v);
	} else {
		auto box = dataModel()->mouseBoundingBox();
		box->setEndPoint(event->x(), event->y());

		renderGraphicsView();
	}
}

void GeoDataPolyDataGroup::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		if (impl->m_mode == Mode::EditingData) {
			auto target = editTargetData();
			target->mousePressEvent(event, v);
			if (target->inNormalMode()) {
				mergeEditTargetData();
			}
		}

		if (impl->m_mode == Mode::Normal) {
			auto box = dataModel()->mouseBoundingBox();
			box->setStartPoint(event->x(), event->y());
			box->enable();

			v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
		}
	} else if (event->button() == Qt::RightButton) {
		impl->m_dragStartPoint = event->pos();
	}
}

void GeoDataPolyDataGroup::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	addAction()->setText(tr("Add New %1").arg(creator()->shapeNameCamelCase()));

	if (impl->m_mode == Mode::EditingData) {
		if (event->button() == Qt::LeftButton) {
			editTargetData()->mouseReleaseEvent(event, v);
		} else if (event->button() == Qt::RightButton) {
			if (! iRIC::isNear(impl->m_dragStartPoint, event->pos())) {return;}

			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	} else {
		if (event->button() == Qt::LeftButton) {
			auto box = dataModel()->mouseBoundingBox();
			box->setEndPoint(event->x(), event->y());
			box->disable();

			if (iRIC::isNear(box->startPoint(), box->endPoint())) {
				double x = box->endPoint().x();
				double y = box->endPoint().y();
				v->viewportToWorld(x, y);
				selectDataWithPoint(QPointF(x, y));
			} else {
				auto vtkPol = box->getPolygon();
				auto points = vtkPol->GetPoints();
				QPolygonF qpol;
				for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
					double p[3];
					points->GetPoint(i, p);
					qpol.push_back(QPointF(p[0], p[1]));
				}
				qpol.push_back(qpol.at(0));
				selectDataWithPolygon(qpol);
			}
			v->restoreUpdateRate();
			if (selectedData().size() == 1) {
				setupEditTargetDataFromSelectedData();
			} else {
				updateVtkObjects();
				updateSelectedDataVtkObjects();
				updateMenu();
			}
			updateAttributeBrowser();
			renderGraphicsView();
			impl->updateActionStatus();
		}
		else {
			if (! iRIC::isNear(impl->m_dragStartPoint, event->pos())) {return;}

			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}

}

void GeoDataPolyDataGroup::informSelection(PreProcessorGraphicsViewInterface* /*v*/)
{
	auto actors = actorsToShowWhenSelected();

	auto col = actorCollection();
	for (auto a : actors) {
		col->AddItem(a);
	}

	if (impl->m_attributeBrowserIsShown) {
		impl->m_attributeBrowser->setWindowTitle(tr("%1 Group Attribute Browser").arg(creator()->shapeNameCamelCase()));
		impl->m_attributeBrowser->show();
	}

	if (selectedData().size() == 1) {
		setupEditTargetDataFromSelectedData();
	}

	updateVisibilityWithoutRendering();
}

void GeoDataPolyDataGroup::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	mergeEditTargetData();

	auto actors = actorsToShowWhenSelected();
	auto col = actorCollection();

	for (auto a : actors) {
		col->RemoveItem(a);
		a->VisibilityOff();
	}

	impl->m_attributeBrowser->blockSignals(true);
	impl->m_attributeBrowser->hide();
	impl->m_attributeBrowser->blockSignals(false);

	v->unsetCursor();
}

void GeoDataPolyDataGroup::addData()
{
	if (impl->m_editTargetData != nullptr) {
		if (! impl->m_editTargetData->isDefined()) {return;}
		mergeEditTargetData();
	}
	setupNewEditTargetData();

	renderGraphicsView();
	updateMenu();
}

void GeoDataPolyDataGroup::editName()
{
	bool ok;
	QString newName = QInputDialog::getText(preProcessorWindow(), tr("Edit name"), tr("Name:"), QLineEdit::Normal, "", &ok);
	if (! ok) {return;}
	if (newName.isEmpty()) {return;}

	std::vector<QString> newNames;
	std::vector<QVariant> newValues;
	std::vector<GeoDataPolyDataGroupPolyData*> data;

	for (auto d : impl->m_selectedData) {
		newNames.push_back(newName);
		newValues.push_back(d->value());
		data.push_back(d);
	}
	pushCommand(new EditNameAndValueCommand(newNames, newValues, data, this));
}

void GeoDataPolyDataGroup::editNameAndValue()
{
	auto dialog = m_gridAttribute->editNameAndValueDialog(preProcessorWindow());
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	i->setupEditWidget(dialog->widget());
	dialog->setName("");
	dialog->clearValue();
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {
		delete dialog;
		return;
	}
	QString newName = dialog->name();
	bool valSelected = dialog->isValueSelected();
	QVariant newValue = dialog->variantValue();

	if (newName.isEmpty() && ! valSelected) {return;}

	std::vector<QString> newNames;
	std::vector<QVariant> newValues;
	std::vector<GeoDataPolyDataGroupPolyData*> data;

	for (auto p : impl->m_selectedData) {
		if (! newName.isEmpty()) {
			newNames.push_back(newName);
		} else {
			newNames.push_back(p->name());
		}

		if (valSelected) {
			newValues.push_back(newValue);
		} else {
			newValues.push_back(p->value());
		}

		data.push_back(p);
	}

	delete dialog;

	pushCommand(new EditNameAndValueCommand(newNames, newValues, data, this));
}

void GeoDataPolyDataGroup::deleteSelectedData()
{
	if (QMessageBox::No == QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to delete selected %1s?").arg(creator()->shapeName()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
		return;
	}
	std::vector<GeoDataPolyDataGroupPolyData*> newData;

	auto& selData = impl->m_selectedData;
	for (auto p : impl->m_data) {
		if (selData.find(p) != selData.end()) {
			delete p;
		} else {
			newData.push_back(p);
		}
	}
	impl->m_data = newData;
	selData.clear();

	if (impl->m_editTargetData != nullptr) {
		delete impl->m_editTargetData;
		delete impl->m_editTargetDataBackup;

		impl->m_editTargetData = nullptr;
		impl->m_editTargetDataIndex = 0;
		impl->m_editTargetDataBackup = nullptr;
		impl->m_mode = Mode::Normal;
	}

	// this operation is not undoable.
	iRICUndoStack::instance().clear();

	updateVtkObjects();
	updateSelectedDataVtkObjects();
	updateIndex();
	updateMenu();

	impl->updateAttributeBrowser();
	impl->updateActionStatus();

	auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (parent());
	p->informValueRangeChange();
	p->informDataChange();
}

void GeoDataPolyDataGroup::editColorSetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPolyDataGroup::showAttributeBrowser()
{
	impl->m_attributeBrowser->show();
	impl->updateAttributeBrowser();
}

void GeoDataPolyDataGroup::mergePolyDataAndPolyDataGroups()
{
	mergeEditTargetData();

	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (parent()->parent());
	std::vector<PreProcessorGeoDataDataItemInterface*> dataToMerge;
	std::map<PreProcessorGeoDataDataItemInterface*, int> dataIndex;
	int thisIndex = 0;

	int index = 0;
	for (auto item : gItem->childItems()) {
		auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (item);
		auto geoData = p->geoData();
		if (geoData == this) {
			thisIndex = index;
			continue;
		}
		if (isMergablePolyData(geoData) || isMergablePolyDataGroup(geoData)) {
			dataToMerge.push_back(p);
			dataIndex.insert({p, index});
		}
		++ index;
	}
	if (dataToMerge.size() == 0) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("There is no other data to merge in \"%1\" group.").arg(gItem->condition()->caption()));
		return;
	}

	GeoDataPolyDataGroupMergeSettingDialog dialog(preProcessorWindow());
	dialog.setItems(dataToMerge);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	dataToMerge = dialog.selectedItems();
	int thisCount = impl->m_data.size();
	int position = 0;
	for (auto item : dataToMerge) {
		auto geoData = item->geoData();
		int index = dataIndex.at(item);
		if (index >= thisIndex) {
			position += thisCount;
			thisCount = 0;
		}
		if (isMergablePolyData(geoData)) {
			auto polyData = dynamic_cast<GeoDataPolyData*> (geoData);
			mergePolyData(polyData, position);
			++ position;
		} else if (isMergablePolyDataGroup(geoData)) {
			auto group = dynamic_cast<GeoDataPolyDataGroup*> (geoData);
			mergePolyDataGroup(group, position);
			position += group->allData().size();
		}
		item->setDeleteSilently(true);
		delete item;
	}
	updateIndex();
	updateVtkObjects();
	updateSelectedDataVtkObjects();
	updateAttributeBrowser();
}

void GeoDataPolyDataGroup::handleAttributeBrowserVisibilityChange(bool visible)
{
	impl->m_attributeBrowserIsShown = visible;
}

void GeoDataPolyDataGroup::moveSelectedDataToTop()
{
	if (impl->m_editTargetData != nullptr) {
		if (impl->m_editTargetDataIndex == 0) {return;}
		pushCommand(new SortEditTargetDataCommand(0, this));
	} else {
		auto selected = getSelectedDataVector(impl->m_data, impl->m_selectedData);
		auto notSelected = getNotSelectedDataVector(impl->m_data, impl->m_selectedData);
		notSelected.insert(notSelected.begin(), selected.begin(), selected.end());

		pushCommand(new SortCommand(notSelected, this));
	}
}

void GeoDataPolyDataGroup::moveSelectedDataToBottom()
{
	if (impl->m_editTargetData != nullptr) {
		if (impl->m_editTargetDataIndex == impl->m_data.size()) {return;}
		pushCommand(new SortEditTargetDataCommand(static_cast<int>(impl->m_data.size()), this));
	} else {
		auto selected = getSelectedDataVector(impl->m_data, impl->m_selectedData);
		auto notSelected = getNotSelectedDataVector(impl->m_data, impl->m_selectedData);
		notSelected.insert(notSelected.end(), selected.begin(), selected.end());

		pushCommand(new SortCommand(notSelected, this));
	}
}

void GeoDataPolyDataGroup::moveSelectedDataUp()
{
	if (impl->m_editTargetData != nullptr) {
		if (impl->m_editTargetDataIndex == 0) {return;}
		pushCommand(new SortEditTargetDataCommand(impl->m_editTargetDataIndex - 1, this));
	} else {
		auto index = indexOfFirstSelected(impl->m_data, impl->m_selectedData);
		if (index == 0) {return;}

		auto selected = getSelectedDataVector(impl->m_data, impl->m_selectedData);
		auto notSelected = getNotSelectedDataVector(impl->m_data, impl->m_selectedData);
		notSelected.insert(notSelected.begin() + index - 1, selected.begin(), selected.end());

		pushCommand(new SortCommand(notSelected, this));
	}
}

void GeoDataPolyDataGroup::moveSelectedDataDown()
{
	if (impl->m_editTargetData != nullptr) {
		if (impl->m_editTargetDataIndex == impl->m_data.size()) {return;}
		pushCommand(new SortEditTargetDataCommand(impl->m_editTargetDataIndex + 1, this));
	} else {
		auto index = indexOfFirstSelected(impl->m_data, impl->m_selectedData);
		if (index == impl->m_data.size() - impl->m_selectedData.size()) {return;}

		auto selected = getSelectedDataVector(impl->m_data, impl->m_selectedData);
		auto notSelected = getNotSelectedDataVector(impl->m_data, impl->m_selectedData);
		notSelected.insert(notSelected.begin() + index + 1, selected.begin(), selected.end());

		pushCommand(new SortCommand(notSelected, this));
	}
}

void GeoDataPolyDataGroup::copy()
{
	mergeEditTargetData();

	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	auto tItem = dynamic_cast<PreProcessorGeoDataTopDataItemInterface*>(gItem->parent());

	std::vector<PreProcessorGeoDataGroupDataItemInterface*> targetGroups;
	QStringList targetNames;

	for (PreProcessorGeoDataGroupDataItemInterface* item : tItem->groupDataItems()) {
		if (item == gItem) {continue;}

		targetGroups.push_back(item);
		targetNames.push_back(item->standardItem()->text());
	}

	bool ok;
	QString item = QInputDialog::getItem(preProcessorWindow(), tr("Select Geographic Data"), tr("Please select which geographic data to copy this %1.").arg(creator()->shapeName()), targetNames, 0, false, &ok);
	if (! ok) {return;} // canceled

	PreProcessorGeoDataGroupDataItemInterface* targetGroup = targetGroups[targetNames.indexOf(item)];

	auto geoDataItem = targetGroup->buildGeoDataDataItem();

	auto copyGroup = createInstanceForCopy(geoDataItem);
	for (GeoDataPolyDataGroupPolyData* data : impl->m_data) {
		copyGroup->impl->m_data.push_back(data->copy(copyGroup));
	}

	creator()->setNameAndDefaultCaption(targetGroup->childItems(), copyGroup);
	if (! copyGroup->gridAttribute()->isReferenceInformation()) {
		GeoDataPolyDataGroupCopySettingDialog dialog(this, copyGroup, preProcessorWindow());
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {
			delete copyGroup;
			delete geoDataItem;
			return;
		}
	}

	copyGroup->updateVtkObjects();
	copyGroup->updateIndex();
	copyGroup->updateMenu();
	copyGroup->impl->updateAttributeBrowser(true);

	geoDataItem->setGeoData(copyGroup);
	targetGroup->addGeoData(geoDataItem);
}

const std::vector<GeoDataPolyDataGroupPolyData*>& GeoDataPolyDataGroup::data() const
{
	return impl->m_data;
}

std::vector<GeoDataPolyDataGroupPolyData*>& GeoDataPolyDataGroup::data()
{
	return impl->m_data;
}

const std::unordered_set<GeoDataPolyDataGroupPolyData*>& GeoDataPolyDataGroup::selectedData() const
{
	return impl->m_selectedData;
}

std::unordered_set<GeoDataPolyDataGroupPolyData*>& GeoDataPolyDataGroup::selectedData()
{
	return impl->m_selectedData;
}

void GeoDataPolyDataGroup::addData(GeoDataPolyDataGroupPolyData* data)
{
	impl->m_data.push_back(data);
}

GeoDataPolyData* GeoDataPolyDataGroup::editTargetData() const
{
	return impl->m_editTargetData;
}

GeoDataPolyDataGroupPolyData* GeoDataPolyDataGroup::editTargetDataBackup() const
{
	return impl->m_editTargetDataBackup;
}

QMenu* GeoDataPolyDataGroup::rightClickingMenu() const
{
	return impl->m_rightClickingMenu;
}

GeoDataPolyDataGroup::Mode GeoDataPolyDataGroup::mode() const
{
	return impl->m_mode;
}

void GeoDataPolyDataGroup::setMode(Mode mode)
{
	impl->m_mode = mode;
}

geos::index::quadtree::Quadtree* GeoDataPolyDataGroup::qTree() const
{
	return impl->m_qTree;
}

const ZDepthRange& GeoDataPolyDataGroup::depthRange() const
{
	return impl->m_depthRange;
}

GeoDataPolyDataGroupColorSettingDialog::Setting GeoDataPolyDataGroup::colorSetting() const
{
	return impl->m_colorSetting;
}

QAction* GeoDataPolyDataGroup::addAction() const
{
	return impl->m_addAction;
}

QAction* GeoDataPolyDataGroup::editNameAction() const
{
	return impl->m_editNameAction;
}

QAction* GeoDataPolyDataGroup::editNameAndValueAction() const
{
	return impl->m_editNameAndValueAction;
}

QAction* GeoDataPolyDataGroup::deleteAction() const
{
	return impl->m_deleteAction;
}

QAction* GeoDataPolyDataGroup::mergeAction() const
{
	return impl->m_mergeAction;
}

QAction* GeoDataPolyDataGroup::editColorSettingAction() const
{
	return impl->m_editColorSettingAction;
}

QAction* GeoDataPolyDataGroup::attributeBrowserAction() const
{
	return impl->m_attributeBrowserAction;
}

QAction* GeoDataPolyDataGroup::moveToTopAction() const
{
	return impl->m_moveToTopAction;
}

QAction* GeoDataPolyDataGroup::moveToBottomAction() const
{
	return impl->m_moveToBottomAction;
}

QAction* GeoDataPolyDataGroup::moveUpAction() const
{
	return impl->m_moveUpAction;
}

QAction* GeoDataPolyDataGroup::moveDownAction() const
{
	return impl->m_moveDownAction;
}

QAction* GeoDataPolyDataGroup::copyAction() const
{
	return impl->m_copyAction;
}

void GeoDataPolyDataGroup::setColorSetting(const GeoDataPolyDataGroupColorSettingDialog::Setting& setting)
{
	impl->m_colorSetting = setting;
	updateActorSetting();
}

void GeoDataPolyDataGroup::updateActorSettingForEditTargetPolyData()
{
	auto targetData = editTargetData();
	if (targetData == nullptr) {return;}

	const auto& cs = impl->m_colorSetting;
	targetData->setColor(cs.color);
	targetData->setOpacity(cs.opacity);
	if (cs.mapping == GeoDataPolyDataGroupColorSettingDialog::Arbitrary) {
		targetData->setMapping(GeoDataPolyDataColorSettingDialog::Arbitrary);
	} else {
		targetData->setMapping(GeoDataPolyDataColorSettingDialog::Value);
	}
}

void GeoDataPolyDataGroup::updateAttributeBrowser(bool force)
{
	impl->updateAttributeBrowser(force);
}

void GeoDataPolyDataGroup::makeConnections()
{
	connect(impl->m_addAction, SIGNAL(triggered()), this, SLOT(addData()));
	connect(impl->m_mergeAction, SIGNAL(triggered()), this, SLOT(mergePolyDataAndPolyDataGroups()));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedData()));
	connect(impl->m_editNameAction, SIGNAL(triggered()), this, SLOT(editName()));
	connect(impl->m_editNameAndValueAction, SIGNAL(triggered()), this, SLOT(editNameAndValue()));
	connect(impl->m_editColorSettingAction, SIGNAL(triggered()), this, SLOT(editColorSetting()));
	connect(impl->m_attributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));
	connect(impl->m_moveToTopAction, SIGNAL(triggered()), this, SLOT(moveSelectedDataToTop()));
	connect(impl->m_moveToBottomAction, SIGNAL(triggered()), this, SLOT(moveSelectedDataToBottom()));
	connect(impl->m_moveUpAction, SIGNAL(triggered()), this, SLOT(moveSelectedDataUp()));
	connect(impl->m_moveDownAction, SIGNAL(triggered()), this, SLOT(moveSelectedDataDown()));
	connect(impl->m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
	connect(impl->m_attributeBrowser, SIGNAL(visibilityChanged(bool)), this, SLOT(handleAttributeBrowserVisibilityChange(bool)));
}

void GeoDataPolyDataGroup::mergeEditTargetData(bool noUpdate)
{
	if (editTargetData() == nullptr) {return;}

	auto p = impl->m_editTargetData;
	auto pb = impl->m_editTargetDataBackup;

	if (p->isDefined()) {
		pb->setName(p->caption());
		pb->setValue(p->variantValue());
		pb->copyShapeFrom(p);

		impl->m_data.insert(impl->m_data.begin() + impl->m_editTargetDataIndex, pb);
		impl->m_selectedData.insert(pb);
	} else {
		delete impl->m_editTargetDataBackup;
	}

	delete impl->m_editTargetData;
	impl->m_editTargetData = nullptr;
	impl->m_editTargetDataIndex = 0;
	impl->m_editTargetDataBackup = nullptr;
	impl->m_mode = Mode::Normal;

	iRICUndoStack::instance().clear();

	if (noUpdate) {return;}

	updateVtkObjects();
	updateSelectedDataVtkObjects();
	updateIndex();
	updateMenu();
	impl->updateAttributeBrowser();
}

GeoDataPolyDataGroupCreator* GeoDataPolyDataGroup::creator() const
{
	return dynamic_cast<GeoDataPolyDataGroupCreator*> (GeoData::creator());
}

void GeoDataPolyDataGroup::setupNewEditTargetData()
{
	GeoDataPolyData* d = createEditTargetData();
	impl->m_editTargetData = d;
	d->setCaption(captionForData(impl->m_data.size() + 1));
	d->showInitialDialog();
	d->informSelection(graphicsView());

	impl->m_editTargetDataIndex = 0;
	impl->m_editTargetDataBackup = createNewData();
	impl->m_editTargetDataBackup->setName(d->caption());
	connect(d, SIGNAL(modified()), impl->m_editTargetDataBackup, SLOT(applyShape()));
	impl->m_mode = Mode::EditingData;

	selectedData().clear();
	updateSelectedDataVtkObjects();
	updateActorSettingForEditTargetPolyData();
	updateAttributeBrowser();
	impl->updateActionStatus();
}

void GeoDataPolyDataGroup::setupEditTargetDataFromSelectedData()
{
	auto sel = *(selectedData().begin());
	auto it = std::find(impl->m_data.begin(), impl->m_data.end(), sel);
	if (it == impl->m_data.end()) {return;}

	impl->m_editTargetDataIndex = static_cast<int>(it - impl->m_data.begin());
	impl->m_data.erase(it);

	impl->m_editTargetDataBackup = sel;

	GeoDataPolyData* d = createEditTargetData();
	impl->m_editTargetData = d;
	d->setCaption(sel->name());
	d->setVariantValue(sel->value());
	sel->copyShapeTo(d);
	connect(d, SIGNAL(modified()), sel, SLOT(applyShape()));
	d->informSelection(graphicsView());

	selectedData().clear();;
	impl->m_mode = Mode::EditingData;

	updateVtkObjects();
	updateSelectedDataVtkObjects();
	updateIndex();
	updateMenu();
	updateActorSettingForEditTargetPolyData();
	impl->updateAttributeBrowserSelection();

	iRICUndoStack::instance().clear();
}

void GeoDataPolyDataGroup::mergePolyData(GeoDataPolyData* data, int position)
{
	auto d = createNewData();
	d->setName(data->caption());
	d->setValue(data->variantValue());
	// @todo
	// d->setId()
	d->copyShapeFrom(data);
	if (position == -1) {
		impl->m_data.push_back(d);
	} else {
		impl->m_data.insert(impl->m_data.begin() + position, d);
	}
}

void GeoDataPolyDataGroup::mergePolyDataGroup(GeoDataPolyDataGroup* group, int position)
{
	auto& data = group->data();
	if (position == -1) {
		impl->m_data.insert(impl->m_data.end(), data.begin(), data.end());
	} else {
		impl->m_data.insert(impl->m_data.begin() + position, data.begin(), data.end());
	}
	data.clear();
}

void GeoDataPolyDataGroup::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_colorSetting.load(node);
}

void GeoDataPolyDataGroup::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_colorSetting.save(writer);
}

void GeoDataPolyDataGroup::loadExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);

	int numData;
	s >> numData;

	for (int i = 0; i < numData; ++i) {
		auto data = createNewData();
		data->loadExternalData(&s);
		impl->m_data.push_back(data);
	}
	f.close();

	updateVtkObjects();
	updateSelectedDataVtkObjects();;
	updateIndex();
	updateActorSetting();

	impl->m_attributeBrowser->update();
	impl->updateActionStatus();
}

void GeoDataPolyDataGroup::saveExternalData(const QString& filename)
{
	mergeEditTargetData();

	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);

	int numData = static_cast<int> (impl->m_data.size());
	s << numData;
	for (int i = 0; i < numData; ++i) {
		auto d = impl->m_data.at(i);
		d->saveExternalData(&s);
	}
	f.close();
}

void GeoDataPolyDataGroup::assignActorZValues(const ZDepthRange& range)
{
	impl->m_depthRange = range;
}

void GeoDataPolyDataGroup::doApplyOffset(double x, double y)
{
	for (auto d : impl->m_data) {
		d->applyOffset(x, y);
	}
	if (impl->m_editTargetData != nullptr) {
		impl->m_editTargetData->applyOffset(x, y);
	}

	updateVtkObjects();
	updateSelectedDataVtkObjects();
	updateIndex();
}

int GeoDataPolyDataGroup::editTargetDataIndex() const
{
	return impl->m_editTargetDataIndex;
}

bool GeoDataPolyDataGroup::isSelected(GeoDataPolyDataGroupPolyData* data)
{
	const auto& selData = impl->m_selectedData;
	return (selData.find(data) != selData.end());
}

std::vector<GeoDataPolyDataGroupPolyData*> GeoDataPolyDataGroup::allData() const
{
	auto ret = impl->m_data;
	if (impl->m_editTargetData != nullptr) {
		ret.insert(ret.begin() + impl->m_editTargetDataIndex, impl->m_editTargetDataBackup);
	}
	return ret;
}

void GeoDataPolyDataGroup::panTo(int row)
{
	double xmin, xmax, ymin, ymax;
	if (row < impl->m_editTargetDataIndex) {
		data().at(row)->getBoundingRect(&xmin, &xmax, &ymin, &ymax);
	} else if (row == editTargetDataIndex() && editTargetData() != nullptr) {
		editTargetData()->getBoundingRect(&xmin, &xmax, &ymin, &ymax);
	} else {
		int offset = 0;
		if (editTargetData() != nullptr) {offset = -1;}
		data().at(row + offset)->getBoundingRect(&xmin, &xmax, &ymin, &ymax);
	}
	auto v = graphicsView();
	v->panTo((xmin + xmax) * 0.5, (ymin + ymax) * 0.5);
}

QColor GeoDataPolyDataGroup::color() const
{
	return colorSetting().color;
}
