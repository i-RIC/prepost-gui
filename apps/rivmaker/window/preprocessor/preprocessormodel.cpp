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
	m_project {nullptr},
	m_view {nullptr},
	m_standardItemModel {},
	m_elevationPointsItem {tr("Elevations")},
	m_waterSurfaceElevationPointsItem {tr("Water Surface Elevations")},
	m_crossSectionsItem {tr("Cross Sections")},
	m_baseLineItem {tr("Base Line")},
	m_riverSurveyDataItem {tr("River Survey Data")}
{
	setupStandatdItemModel();
}

PreProcessorModel::Impl::~Impl()
{}

void PreProcessorModel::Impl::setupStandatdItemModel()
{
	setupCheckBox(&m_elevationPointsItem);
	setupCheckBox(&m_waterSurfaceElevationPointsItem);
	setupCheckBox(&m_crossSectionsItem);
	setupCheckBox(&m_baseLineItem);
	setupCheckBox(&m_riverSurveyDataItem);

	m_standardItemModel.appendRow(&m_elevationPointsItem);
	m_standardItemModel.appendRow(&m_waterSurfaceElevationPointsItem);
	m_standardItemModel.appendRow(&m_crossSectionsItem);
	m_standardItemModel.appendRow(&m_baseLineItem);
	m_standardItemModel.appendRow(&m_riverSurveyDataItem);
}

// public interfaces

PreProcessorModel::PreProcessorModel(QObject* parent) :
	impl {new Impl {}}
{}

PreProcessorModel::~PreProcessorModel()
{
	delete impl;
}

void PreProcessorModel::setProject(Project* project)
{
	impl->m_project = project;
}

void PreProcessorModel::setView(PreProcessorView* view)
{
	impl->m_view = view;
}

QStandardItemModel* PreProcessorModel::standardItemmodel() const
{
	return &(impl->m_standardItemModel);
}
