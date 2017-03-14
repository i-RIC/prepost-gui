#include "dataitemview.h"
#include "dataitemviewhelperi.h"
#include "model.h"

#include "private/dataitemview_impl.h"

#include <QRectF>

#include <algorithm>

DataItemView::Impl::Impl(Model *model, DataItem *item) :
	m_model {model},
	m_item {item}
{}

DataItemView::Impl::~Impl()
{
	for (auto helper : m_viewHelpers) {
		delete helper;
	}
}

DataItemView::DataItemView(Model* model, DataItem* item) :
	impl {new Impl {model, item}}
{}

DataItemView::~DataItemView()
{
	delete impl;
}

void DataItemView::discardDrawCache()
{
	doDiscardDrawCache();
	for (auto child : impl->m_childItems) {
		child->discardDrawCache();
	}
}

bool DataItemView::prepareDraw()
{
	bool ok = doPrepareDraw();
	if (! ok) {return false;}

	for (auto it = impl->m_childItems.rbegin(); it != impl->m_childItems.rend(); ++it) {
		auto child = *it;
		ok = child->prepareDraw();
		if (! ok) {return false;}
	}
	return true;
}

void DataItemView::draw(QPainter* painter) const
{
	if (! model()->shouldDraw(item())) {return;}

	doDraw(painter);

	for (auto it = impl->m_childItems.rbegin(); it != impl->m_childItems.rend(); ++ it) {
		auto child = *it;
		child->draw(painter);
	}
}

void DataItemView::addChildItem(DataItemView* v)
{
	auto it = std::find(impl->m_childItems.begin(), impl->m_childItems.end(), v);
	if (it != impl->m_childItems.end()) {return;}

	impl->m_childItems.push_back(v);
}

void DataItemView::removeChildItem(DataItemView* v)
{
	auto it = std::find(impl->m_childItems.begin(), impl->m_childItems.end(), v);
	if (it == impl->m_childItems.end()) {return;}

	impl->m_childItems.erase(it);
}

QRectF DataItemView::boundingBox() const
{
	QRectF ret = doBoundingBox();

	for (auto child : impl->m_childItems) {
		QRectF bb = child->boundingBox();
		if (bb.isNull()) {continue;}

		ret = ret.united(bb);
	}
	return ret;
}


void DataItemView::doDraw(QPainter *painter) const
{
	for (auto h : impl->m_viewHelpers) {
		h->draw(painter);
	}
}

DataItem* DataItemView::item() const
{
	return impl->m_item;
}

View* DataItemView::view() const
{
	return model()->view();
}

Model *DataItemView::model() const
{
	return impl->m_model;
}

const std::vector<DataItemViewHelperI*>& DataItemView::viewHelpers() const
{
	return impl->m_viewHelpers;
}

std::vector<DataItemViewHelperI*>& DataItemView::viewHelpers()
{
	return impl->m_viewHelpers;
}

void DataItemView::doDiscardDrawCache()
{}

bool DataItemView::doPrepareDraw()
{
	return true;
}

QRectF DataItemView::doBoundingBox() const
{
	return QRectF();
}
