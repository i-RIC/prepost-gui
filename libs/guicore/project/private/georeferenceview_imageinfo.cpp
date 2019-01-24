#include "georeferenceview_imageinfo.h"
#include "../backgroundimageinfo.h"
#include "../georeferenceview.h"
#include "../addiblegcptablemodel.h"
#include "../gcptablerow.h"
#include "../backgroundimageinfogeoreferencedialog.h"

#include <QMessageBox>
#include <QPainter>

// ITEM
GeoreferenceView::ImageInfo::ImageInfo(BackgroundImageInfo* info, AddibleGcpTableModel* gcpTableModel, GeoreferenceView* view) :
	m_view {view},
	m_backgroundImageInfo {info},
	m_gcpTableModel {gcpTableModel},
	m_dialog {nullptr},
	m_rightClickMenu {/*modelView()*/}
{
	m_deletePointsAction = new QAction(QIcon(""), tr("Delete Points"), this);

	addCustomMenuItems(&m_rightClickMenu);

	connect(m_deletePointsAction, SIGNAL(triggered()), this, SLOT(deletePoints()));
	connect(this, SIGNAL(gcpDefined()), backgroundImageInfo(), SLOT(startGcpSelect()));
	connect(backgroundImageInfo(), SIGNAL(gcpDefined(const QPointF&)), this, SLOT(startGcpSelect(const QPointF&)));
	// SLOT/SIGNAL macro version not worked.
 	// connect(this, SIGNAL(selectedIndexChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>&)), backgroundImageInfo(), SLOT(selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& selectedIndex)));
 	// connect(backgroundImageInfo(), SIGNAL(selectedIndexChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>&)), this, SLOT(selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& selectedIndex)));
	connect(this, &GeoreferenceView::ImageInfo::selectedIndexChanged, backgroundImageInfo(), &BackgroundImageInfo::selectPoints);
	connect(backgroundImageInfo(), &BackgroundImageInfo::selectedIndexChanged, this, &GeoreferenceView::ImageInfo::selectPoints);
	connect(backgroundImageInfo(), &BackgroundImageInfo::selectedIndexChanged, this, &GeoreferenceView::ImageInfo::updateTableSelection);
}

GeoreferenceView::ImageInfo::~ImageInfo()
{}

GeoreferenceView* GeoreferenceView::ImageInfo::modelView() const
{
	return m_view;
}

BackgroundImageInfo* GeoreferenceView::ImageInfo::backgroundImageInfo() const
{
	return m_backgroundImageInfo;
}

void GeoreferenceView::ImageInfo::showRightClickMenu(const QPoint& pos)
{
	QMenu* m = rightClickMenu();
	m->move(pos);
	m->show();
}

AddibleGcpTableModel* GeoreferenceView::ImageInfo::gcpTableModel()
{
	return m_gcpTableModel;
}

std::vector<GcpTableRow>* GeoreferenceView::ImageInfo::gcpTable()
{
	return m_gcpTableModel->gcpTable();
}

// CONTROLLER
void GeoreferenceView::ImageInfo::setDialog(BackgroundImageInfoGeoreferenceDialog* dialog)
{
	m_dialog = dialog;
	connect(m_dialog, &BackgroundImageInfoGeoreferenceDialog::selectionChanged, m_backgroundImageInfo, &BackgroundImageInfo::selectPoints);
}

void GeoreferenceView::ImageInfo::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_deletePointsAction);
}

void GeoreferenceView::ImageInfo::keyPressEvent(QKeyEvent* event, GeoreferenceView* view)
{
}

void GeoreferenceView::ImageInfo::mouseDoubleClickEvent(QMouseEvent* event, GeoreferenceView* view)
{
}

void GeoreferenceView::ImageInfo::mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view)
{
}

void GeoreferenceView::ImageInfo::mousePressEvent(QMouseEvent* event, GeoreferenceView* view)
{
}

void GeoreferenceView::ImageInfo::mouseReleaseEvent(QMouseEvent* event, GeoreferenceView* view)
{
}

void GeoreferenceView::ImageInfo::wheelEvent(QWheelEvent* event, GeoreferenceView* view)
{
}

QMenu* GeoreferenceView::ImageInfo::rightClickMenu()
{
	return &m_rightClickMenu;
}

void GeoreferenceView::ImageInfo::selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices)
{
}

void GeoreferenceView::ImageInfo::updateTableSelection(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices)
{
	m_dialog->clearSelection();
	m_dialog->setSelection(indices);
}

void GeoreferenceView::ImageInfo::deletePoints()
{
}

void GeoreferenceView::ImageInfo::startGcpSelect(const QPointF& point)
{
	std::vector<std::vector<GcpTableRow>::size_type> indices;
	indices.push_back(gcpTable()->size() - 1);
	std::vector<QPointF> points;
	points.push_back(QPointF {point.x(), point.y()});

	gcpTableModel()->replaceDestinationData(indices, points);

	QMessageBox::information(modelView(), tr("Information"), tr("Select a point on Georeference dialog."));
}

// VIEW
void GeoreferenceView::ImageInfo::paintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const
{
	doPaintView(painter, transform, rect);
	paintHelpers(painter, transform, rect);
}

std::vector<GeoreferenceViewHelper*>& GeoreferenceView::ImageInfo::viewHelpers()
{
	return m_viewHelpers;
}

void GeoreferenceView::ImageInfo::doPaintView(QPainter* painter, const QTransform&, const QRectF&) const
{
	QImage img {m_backgroundImageInfo->name()};

	painter->save();

	// translate
	QPointF topLeft = modelView()->conv(QPointF {});
	painter->translate(topLeft);

	// scale
	double s = modelView()->scale();
	painter->scale(s, s);

	// rotate
	// painter->rotate(- img->m_backgroundImageInfo->angle()); // Rotation disabled.

	painter->drawImage(0, 0, img);

	painter->restore();
}

void GeoreferenceView::ImageInfo::paintHelpers(QPainter* painter, const QTransform& transform, const QRectF& rect) const
{
}
