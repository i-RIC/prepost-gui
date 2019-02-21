#include "georeferenceview.h"
#include "backgroundimageinfo.h"
#include "addiblegcptablemodel.h"

#include "private/georeferenceview_imageinfo.h"

#include <QMouseEvent>
#include <QPainter>

namespace {
// Borrowed from apps/rivmaker/data/base/view.cpp
const static int CLICK_LIMIT = 3;
bool isClick(const QPoint& pressP, const QPoint& releaseP)
{
	if (qAbs(pressP.x() - releaseP.x()) > CLICK_LIMIT) {return false;}
	if (qAbs(pressP.y() - releaseP.y()) > CLICK_LIMIT) {return false;}

	return true;
}
}

GeoreferenceView::GeoreferenceView(QWidget *parent) :
	PrimitiveView {parent},
	m_info {nullptr}
{}

GeoreferenceView::~GeoreferenceView()
{
	delete m_info;
}

void GeoreferenceView::setInfo(BackgroundImageInfo* info, AddibleGcpTableModel* gcpTableModel)
{
	auto s = info->filename().toStdString();
	m_info = new GeoreferenceView::ImageInfo {info, gcpTableModel, this};
	setupTransform();
}

void GeoreferenceView::setDialog(BackgroundImageInfoGeoreferenceDialog* dialog)
{
	m_info->setDialog(dialog);

	connect(dialog, SIGNAL(selectionChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>&)), m_info, SLOT(selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>&)));
}

void GeoreferenceView::keyPressEvent(QKeyEvent* event)
{
	if (m_info == nullptr) {return;}
	m_info->keyPressEvent(event, this);
}

void GeoreferenceView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (m_info == nullptr) {return;}
	m_info->mouseDoubleClickEvent(event, this);
}

void GeoreferenceView::mouseMoveEvent(QMouseEvent* event)
{
	PrimitiveView::mouseMoveEvent(event);

	if (m_info == nullptr) {return;}
	m_info->mouseMoveEvent(event, this);
}

void GeoreferenceView::mousePressEvent(QMouseEvent* event)
{
	PrimitiveView::mousePressEvent(event);

	if (m_info == nullptr) {return;}
	m_info->mousePressEvent(event, this);
}

void GeoreferenceView::mouseReleaseEvent(QMouseEvent* event)
{
	PrimitiveView::mouseReleaseEvent(event);

	if (m_info == nullptr) {return;}
	m_info->mouseReleaseEvent(event, this);

	if (event->button() == Qt::RightButton) {
		if (! isClick(rightClickPos(), event->pos())) {return;}
		m_info->showRightClickMenu(event->globalPos());
	}
}

void GeoreferenceView::wheelEvent(QWheelEvent* event)
{
	PrimitiveView::wheelEvent(event);

	if (m_info == nullptr) {return;}
	m_info->wheelEvent(event, this);
}

void GeoreferenceView::resizeEvent(QResizeEvent*)
{
	setupTransform();
}

std::vector<GcpTableRow>* GeoreferenceView::gcpTable()
{
	return m_info->gcpTable();
}

void GeoreferenceView::setupTransform()
{
	QImage img {m_info->backgroundImageInfo()->name()};
	auto bbox = img.rect();

	auto center = bbox.center();
	center.setY(-center.ry());

	QSize s = size();
	double scaleX = static_cast<double> (s.width()) / bbox.width();
	double scaleY = static_cast<double> (s.height()) / bbox.height();

	auto angleDegree = m_info->backgroundImageInfo()->angle();

	setCenterPoint(center);
	setScale(std::min(scaleX, scaleY));
	setAngleDegree(angleDegree);

	updateTransform();
}

QPixmap GeoreferenceView::drawModel()
{
	QPixmap pixmap(size());

	QPainter painter(&pixmap);
	painter.fillRect(rect(), Qt::darkGray);

	QRectF r = drawnArea();

	if (m_info != nullptr) {
		m_info->paintView(&painter, transform(), r);
	}

	return pixmap;
}
