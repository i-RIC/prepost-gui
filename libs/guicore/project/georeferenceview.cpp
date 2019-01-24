#include "georeferenceview.h"
#include "backgroundimageinfo.h"
#include "addiblegcptablemodel.h"

#include <QMouseEvent>
#include <QPainter>

GeoreferenceView::GeoreferenceView(QWidget *parent) :
	PrimitiveView {parent}
{}

GeoreferenceView::~GeoreferenceView()
{
}

void GeoreferenceView::setInfo(BackgroundImageInfo* info, AddibleGcpTableModel* gcpTableModel)
{
	setupTransform();
}

void GeoreferenceView::setDialog(BackgroundImageInfoGeoreferenceDialog* dialog)
{
}

void GeoreferenceView::keyPressEvent(QKeyEvent* event)
{
}

void GeoreferenceView::mouseDoubleClickEvent(QMouseEvent* event)
{
}

void GeoreferenceView::mouseMoveEvent(QMouseEvent* event)
{
	PrimitiveView::mouseMoveEvent(event);
}

void GeoreferenceView::mousePressEvent(QMouseEvent* event)
{
	PrimitiveView::mousePressEvent(event);
}

void GeoreferenceView::mouseReleaseEvent(QMouseEvent* event)
{
	PrimitiveView::mouseReleaseEvent(event);
}

void GeoreferenceView::wheelEvent(QWheelEvent* event)
{
	PrimitiveView::wheelEvent(event);
}

void GeoreferenceView::resizeEvent(QResizeEvent*)
{
}

std::vector<GcpTableRow>* GeoreferenceView::gcpTable()
{
	return nullptr;
}

void GeoreferenceView::setupTransform()
{
}

QPixmap GeoreferenceView::drawModel()
{
	QPixmap pixmap(size());

	return pixmap;
}
