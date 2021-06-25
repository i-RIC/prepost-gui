#include "../../data/arbitraryhwm/arbitraryhwm.h"
#include "../../data/base/view.h"
#include "../../data/baseline/baseline.h"
#include "../../data/baseline/baselinepreprocessorcontroller.h"
#include "../../data/crosssection/crosssection.h"
#include "../../data/crosssection/crosssectionpreprocessorcontroller.h"
#include "../../data/crosssections/crosssectionspreprocessorcontroller.h"
#include "../../data/elevationpoints/elevationpoints.h"
#include "../../data/points/pointspreprocessorcontroller.h"
#include "../../data/project/project.h"
#include "../../data/watersurfaceelevationpoints/watersurfaceelevationpoints.h"
#include "../../dialogs/coordinateseditdialog.h"

#include "preprocessordataitemi.h"
#include "preprocessormodel.h"
#include "private/preprocessormodel_impl.h"
#include "objectbrowser/objectbrowserview.h"

#include <QMessageBox>

namespace {

void setupCheckBox(QStandardItem* item)
{
	item->setCheckable(true);
	item->setCheckState(Qt::Checked);
}

} // namespace

PreProcessorModel::Impl::Impl() :
	m_project {nullptr}
{}

PreProcessorModel::Impl::~Impl()
{}

// public interfaces

PreProcessorModel::PreProcessorModel(QObject* parent) :
	Model {parent},
	impl {new Impl {}}
{
	setupStandatdItemModel();
}

PreProcessorModel::~PreProcessorModel()
{
	delete impl;
}

void PreProcessorModel::setProject(Project* project)
{
	impl->m_project = project;
	setupStandatdItemModel();
	view()->fit();
}

void PreProcessorModel::setCrossSections(const std::vector<CrossSection*>& crossSections)
{
	for (auto cs : impl->m_project->crossSections().crossSectionVector()) {
		deleteItem(cs);
	}

	auto cs_c = dynamic_cast<CrossSectionsPreProcessorController*> (dataItemController(&(impl->m_project->crossSections())));
	cs_c->setCrossSections(crossSections);
}

void PreProcessorModel::importElevation()
{
	auto e_c = dynamic_cast<PointsPreProcessorController*> (dataItemController(&(impl->m_project->elevationPoints())));
	e_c->importData();
}

void PreProcessorModel::importWaterSurfaceElevation()
{
	auto e_c = dynamic_cast<PointsPreProcessorController*> (dataItemController(&(impl->m_project->waterSurfaceElevationPoints().arbitraryHWM())));
	e_c->importData();
}

void PreProcessorModel::importBaseLine()
{
	auto bl_c = dynamic_cast<BaseLinePreProcessorController*> (dataItemController(&(impl->m_project->baseLine())));
	bl_c->importData();
}

void PreProcessorModel::exportBaseLine()
{
	auto bl_c = dynamic_cast<BaseLinePreProcessorController*> (dataItemController(&(impl->m_project->baseLine())));
	bl_c->exportData();
}

void PreProcessorModel::importCrossSections()
{
	auto css_c = dynamic_cast<CrossSectionsPreProcessorController*> (dataItemController(&(impl->m_project->crossSections())));
	css_c->importData();
}

void PreProcessorModel::exportCrossSections()
{
	auto css_c = dynamic_cast<CrossSectionsPreProcessorController*> (dataItemController(&(impl->m_project->crossSections())));
	css_c->exportData();
}

void PreProcessorModel::addCrossSection()
{
	auto ctrl = dynamic_cast<CrossSectionsPreProcessorController*>
			(dataItemController(& impl->m_project->crossSections()));
	if (ctrl == nullptr) {return;}

	ctrl->addCrossSection();

	objectBrowserView()->expandAll();

	view()->update();
}

void PreProcessorModel::deleteCrossSection()
{
	auto s = selectedItem();
	if (dynamic_cast<CrossSection*> (s) == nullptr) {
		QMessageBox::warning(view(), tr("Warning"), tr("To delete a Cross Section, select it at Object Browser."));
		return;
	}
	s->project()->setModified();
	deleteItem(s);

	updateCrossSections();
}

void PreProcessorModel::editCrossSectionCoordinates()
{
	auto cs = dynamic_cast<CrossSection*> (selectedItem());
	if (cs == nullptr) {
		QMessageBox::warning(view(), tr("Warning"), tr("To edit Cross Section coordinates, select it at Object Browser."));
		return;
	}
	auto cs_c = dynamic_cast<CrossSectionPreProcessorController*> (dataItemController(cs));
	cs_c->editCoordinates();
}

void PreProcessorModel::addBaseLineVertex()
{
	auto& bl = impl->m_project->baseLine();
	auto bl_c = dynamic_cast<BaseLinePreProcessorController*> (dataItemController(&bl));
	bl_c->addVertex();

	select(&bl);
}

void PreProcessorModel::removeBaseLineVertex()
{
	auto& bl = impl->m_project->baseLine();
	auto bl_c = dynamic_cast<BaseLinePreProcessorController*> (dataItemController(&bl));
	bl_c->removeVertex();

	select(&bl);
}

void PreProcessorModel::editBaseLineCoordinates()
{
	auto& bl = impl->m_project->baseLine();
	auto bl_c = dynamic_cast<BaseLinePreProcessorController*> (dataItemController(&bl));
	bl_c->editCoordinates();

	select(&bl);
}

void PreProcessorModel::reverseBaseLineDirection()
{
	auto& bl = impl->m_project->baseLine();
	auto bl_c = dynamic_cast<BaseLinePreProcessorController*> (dataItemController(&bl));
	bl_c->reverseDirection();

	select(&bl);
}

void PreProcessorModel::deleteBaseLine()
{
	auto& bl = impl->m_project->baseLine();
	auto bl_c = dynamic_cast<BaseLinePreProcessorController*> (dataItemController(&bl));
	bl_c->clear();

	select(&bl);
}

void PreProcessorModel::setupStandardItemAndViewAndController(PreProcessorDataItemI* newItem, DataItem* parent)
{
	auto dItem = dynamic_cast<DataItem*> (newItem);
	auto sItem = newItem->buildPreProcessorStandardItem();
	if (sItem != nullptr) {
		addStandardItem(dItem, sItem);
		standardItem(parent)->appendRow(sItem);
	}

	auto view = newItem->buildPreProcessorDataItemView(this);
	if (view != nullptr) {
		addDataItemView(dItem, view);
		dataItemView(parent)->addChildItem(view);
	}

	auto ctrl = newItem->buildPreProcessorDataItemController(this);
	if (ctrl != nullptr) {
		addDataItemController(dItem, ctrl);
	}
}

void PreProcessorModel::updateCrossSections()
{
	bool all_internal = true;
	impl->m_project->calcCrossSectionElevations(&all_internal);
	if (! all_internal) {
		QMessageBox::warning(view(), tr("Warning"),
			tr("Base line should be long enough to cover extents of HWMs"));
	}

	impl->m_project->mapPointsToCrossSections();
	bool sorted = impl->m_project->sortCrossSectionsIfPossible();
	if (! sorted && impl->m_project->baseLine().polyLine().size() >= 2) {
		QMessageBox::warning(view(), tr("Warning"), tr("Not all Cross Sections cross Base line"));
	}
	if (! sorted) {
		impl->m_project->emitUpdated();
		return;
	}

	auto cs = dynamic_cast<CrossSectionsPreProcessorController*> (dataItemController(&(impl->m_project->crossSections())));
	cs->rebuildStandardItemsAndViews();

	impl->m_project->emitUpdated();
}

void PreProcessorModel::emitValueChange(double val)
{
	emit valueChanged(val);
}

void PreProcessorModel::emitValueClear()
{
	emit valueCleared();
}

void PreProcessorModel::deleteSelectedItem()
{
	auto s = selectedItem();
	if (dynamic_cast<CrossSection*> (s) != nullptr) {
		deleteCrossSection();
	}
}

void PreProcessorModel::setupStandatdItemModel()
{
	clearStandardItems();
	clearDataItemViews();
	clearDataItemControllers();

	auto proj = impl->m_project;
	auto model = standardItemModel();

	if (proj == nullptr) {return;}

	buildStandardItems<PreProcessorDataItemI>(proj->rootDataItem(), &(PreProcessorDataItemI::buildPreProcessorStandardItem));
	buildDataItemViews<PreProcessorDataItemI>(proj->rootDataItem(), &(PreProcessorDataItemI::buildPreProcessorDataItemView));
	buildDataItemControllers<PreProcessorDataItemI>(proj->rootDataItem(), &(PreProcessorDataItemI::buildPreProcessorDataItemController));

	model->appendRow(standardItem(&(proj->elevationPoints())));
	model->appendRow(standardItem(&(proj->waterSurfaceElevationPoints())));
	model->appendRow(standardItem(&(proj->crossSections())));
	model->appendRow(standardItem(&(proj->baseLine())));

	objectBrowserView()->expandAll();
}

RootDataItem* PreProcessorModel::rootDataItem() const
{
	if (impl->m_project == nullptr) {
		return nullptr;
	}

	return impl->m_project->rootDataItem();
}
