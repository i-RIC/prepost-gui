#include "../../data/base/view.h"
#include "../../data/baseline/baseline.h"
#include "../../data/crosssection/crosssection.h"
#include "../../data/crosssections/crosssectionspreprocessorcontroller.h"
#include "../../data/elevationpoints/elevationpoints.h"
#include "../../data/project/project.h"
#include "../../data/riversurveydata/riversurveydata.h"
#include "../../data/riversurveydatadummy/riversurveydatadummy.h"
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
	deleteItem(s);
}

void PreProcessorModel::editCrossSectionCoordinates()
{
	auto s = selectedItem();
	auto cs = dynamic_cast<CrossSection*> (s);
	if (cs == nullptr) {
		QMessageBox::warning(view(), tr("Warning"), tr("To edit Cross Section coordinates, select it at Object Browser."));
		return;
	}

	CoordinatesEditDialog dialog(view());
	dialog.setOffset(impl->m_project->offset());
	dialog.setTarget(cs);
	dialog.exec();
}

void PreProcessorModel::editBaseLineCoordinates()
{
	auto& bl = impl->m_project->baseLine();
	if (bl.polyLine().size() < 2) {
		QMessageBox::warning(view(), tr("Warning"), tr("Base line is not defined yet."));
		return;
	}

	CoordinatesEditDialog dialog(view());
	dialog.setOffset(impl->m_project->offset());
	dialog.setTarget(&(impl->m_project->baseLine()));
	dialog.exec();
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

	if (proj->hasRiverSurveyData()) {
		model->appendRow(standardItem(proj->riverSurveyData()));
	} else {
		model->appendRow(standardItem(&(proj->riverSurveyDataDummy())));
	}
	objectBrowserView()->expandAll();
}

RootDataItem* PreProcessorModel::rootDataItem() const
{
	return impl->m_project->rootDataItem();
}
