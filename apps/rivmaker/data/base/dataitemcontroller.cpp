#include "dataitemcontroller.h"
#include "model.h"
#include "view.h"

#include "private/dataitemcontroller_impl.h"

DataItemController::Impl::Impl(Model* model, DataItem* item) :
	m_model {model},
	m_item {item}
{}

DataItemController::DataItemController(Model* model, DataItem* item) :
	impl {new Impl {model, item}}
{}

DataItemController::~DataItemController()
{
	delete impl;
}

void DataItemController::keyPressEvent(QKeyEvent*, View*)
{}

void DataItemController::keyReleaseEvent(QKeyEvent*, View*)
{}

void DataItemController::mouseDoubleClickEvent(QMouseEvent*, View*)
{}

void DataItemController::mouseMoveEvent(QMouseEvent*, View*)
{}

void DataItemController::mousePressEvent(QMouseEvent*, View*)
{}

void DataItemController::mouseReleaseEvent(QMouseEvent*, View*)
{}

void DataItemController::wheelEvent(QWheelEvent*, View*)
{}

void DataItemController::restoreMouseCursorOnView(View*)
{}

QMenu& DataItemController::objectBrowserRightClickMenu()
{
	return impl->m_objectBrowserRightClickMenu;
}

QMenu& DataItemController::viewRightClickMenu()
{
	return impl->m_viewRightClickMenu;
}

DataItem* DataItemController::item()
{
	return impl->m_item;
}

Model* DataItemController::model()
{
	return impl->m_model;
}

void DataItemController::updateView()
{
	impl->m_model->view()->update();
}
