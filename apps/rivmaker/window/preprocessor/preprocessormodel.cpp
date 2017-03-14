#include "../../data/project/project.h"
#include "../../data/riversurveydata/riversurveydata.h"
#include "../../data/riversurveydatadummy/riversurveydatadummy.h"
#include "preprocessordataitemi.h"
#include "preprocessormodel.h"
#include "private/preprocessormodel_impl.h"

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
}

void PreProcessorModel::setupStandatdItemModel()
{
	clearStandardItems();

	auto proj = impl->m_project;
	auto model = standardItemModel();

	if (proj == nullptr) {return;}

	buildStandardItems<PreProcessorDataItemI>(proj->rootDataItem(), &(PreProcessorDataItemI::buildPreProcessorStandardItem));

	model->appendRow(standardItem(&(proj->elevationPoints())));
	model->appendRow(standardItem(&(proj->waterSurfaceElevationPoints())));
	model->appendRow(standardItem(&(proj->baseLine())));
	model->appendRow(standardItem(&(proj->crossSections())));

	if (proj->hasRiverSurveyData()) {
		model->appendRow(standardItem(proj->riverSurveyData()));
	} else {
		model->appendRow(standardItem(&(proj->riverSurveyDataDummy())));
	}
}

