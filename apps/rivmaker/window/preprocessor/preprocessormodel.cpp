#include "preprocessormodel.h"
#include "private/preprocessormodel_impl.h"

PreProcessorModel::Impl::Impl() :
	m_project {nullptr},
	m_view {nullptr},
	m_standardItemModel {}
{}

PreProcessorModel::Impl::~Impl()
{}

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
