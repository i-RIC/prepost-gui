#include "gridcreatingconditionriversurveyi.h"
#include "geodatarivercrosssection.h"
#include "geodatarivercrosssectionaltitudemovedialog.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveycrosssectiondisplaysettingdialog.h"
#include "geodatariversurveycrosssectionslopepointeditdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowgraphicsview.h"
#include "private/geodatariversurvey_editcrosssectioncommand.h"
#include "private/geodatariversurvey_mouseeditcrosssectioncommand.h"
#include "private/geodatariversurveycrosssectionwindowgraphicsview_setdisplaysettingcommand.h"
#include "private/geodatariversurveycrosssectionwindow_impl.h"
#include "private/geodatariversurveycrosssectionwindow_pointadddialog.h"
#include "private/geodatariversurveycrosssectionwindow_jmkdataeditdialog.h"
#include "private/geodatariversurvey_editjmkdatabydragcommand.h"
#include "private/geodatariversurvey_editjmkdatacommand.h"
#include "private/geodatariversurvey_impl.h"

#include <geodata/polyline/geodatapolyline.h>
#include <geodata/polyline/geodatapolylineimplpolyline.h>
#include <geodata/polylinegroup/geodatapolylinegroup.h>
#include <geodata/polylinegroup/geodatapolylinegrouppolyline.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/base/preprocessorhydraulicdatagroupdataitemi.h>
#include <guicore/project/projectdataitem.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevation.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationitem.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/qundocommandhelper.h>

#include <QAction>
#include <QCheckBox>
#include <QFontMetricsF>
#include <QItemSelection>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndexList>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QSet>
#include <QSettings>
#include <QStandardItem>
#include <QTableView>
#include <QTextStream>
#include <QWheelEvent>
#include <QRect>

#include <geos/geom/LineString.h>

#include <cmath>

namespace {

const int bankHOffset = 10;
const int bankVOffset = 35;
const int ODN_VOFFSET = 10;
const int PREVIEW_LABEL_OFFSET = 3;
const int WSE_WIDTH = 120;

const int ASPECT_RATIO_RIGHT_MARGIN = 10;
const int ASPECT_RATIO_BOTTOM_MARGIN = 10;
const int MIN_SCALE_COUNT = 3;
const double DELTA = 1.0E-6;

int findRowToDraw(int rowToTry, const QRectF& rect, std::vector<std::vector<QRectF> >* drawnRects)
{
	if (rowToTry == drawnRects->size()) {
		std::vector<QRectF> newRowData;
		newRowData.push_back(rect);
		drawnRects->push_back(newRowData);
		return rowToTry;
	}
	auto& rowData = (*drawnRects)[rowToTry];
	for (const auto& r : rowData) {
		if (rect.intersects(r)) {
			return findRowToDraw(rowToTry + 1, rect, drawnRects);
		}
	}
	rowData.push_back(rect);
	return rowToTry;
}

int findRowToDraw(const QRectF& rect, std::vector<std::vector<QRectF> >* drawnRects)
{
	return findRowToDraw(0, rect, drawnRects);
}

void drawTextWithWhiteLines(QPainter* painter, const QPointF& point, const QString& str)
{
	painter->save();
	painter->setPen(Qt::white);

	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (i == 0 && j == 0) {continue;}

			auto p = point;
			p += QPointF(i, j);
			painter->drawText(p, str);
		}
	}
	painter->restore();

	painter->drawText(point, str);
}

void calcAutoScale(double width, double* scale, double* subScale)
{
	double w = width / MIN_SCALE_COUNT;
	int i = 0;
	while (w > 10) {
		w /= 10;
		++i;
	}
	while (w < 1) {
		w *= 10;
		--i;
	}
	double pow10 = 10;
	if (i < 0) {
		pow10 = 0.1;
		i = -i;
	}
	if (w > 5) {
		*scale = 5 * std::pow(pow10, i);
		*subScale = *scale * 0.2;
	} else if (w > 2) {
		*scale = 2 * std::pow(pow10, i);
		*subScale = *scale * 0.5;
	} else {
		*scale = std::pow(pow10, i);
		*subScale = *scale * 0.2;
	}
}

double round2(double value)
{
	return std::round(value * 100) / 100.0;
}

QPointF round2(QPointF point)
{
	double x = round2(point.x());
	double y = round2(point.y());
	return QPointF(x, y);
}

} // namespace

GeoDataRiverSurveyCrosssectionWindowGraphicsView::GeoDataRiverSurveyCrosssectionWindowGraphicsView(QWidget* w) :
	QAbstractItemView {w},
	fLeftMargin {0.2},
	fRightMargin {0.2},
	fTopMargin {0.2},
	fBottomMargin {0.2},
	m_rightClickingMenu {nullptr},
	m_rightClickingMenuForEditCrosssectionMode {nullptr},
	m_rubberBand {nullptr},
	m_mouseEventMode {meNormal},
	m_viewMouseEventMode {vmeNormal},
	m_modelessDialogIsOpen {false},
	m_oldLine (nullptr),
	m_slopePointEditMode {GeoDataRiverSurveyCrosssectionSlopePointEditDialog::Mode::LeftAdd},
	m_slopePointEditModeSlopePoint {},
	m_slopePointEditModeSlope {10},
	m_gridMode {false}
{
	// Set cursors for mouse view change events.
	m_zoomPixmap = QPixmap(":/libs/guibase/images/cursorZoom.png");
	m_movePixmap = QPixmap(":/libs/guibase/images/cursorMove.png");

	m_zoomCursor = QCursor(m_zoomPixmap);
	m_moveCursor = QCursor(m_movePixmap);
	setMouseTracking(true);
	setupActions();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setParentWindow(GeoDataRiverSurveyCrosssectionWindow* w)
{
	m_parentWindow = w;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setupActions()
{
	m_activateAction = new QAction(tr("&Activate"), this);
	connect(m_activateAction, &QAction::triggered, this, &GeoDataRiverSurveyCrosssectionWindowGraphicsView::activateSelectedRows);
	m_inactivateAction = new QAction(tr("&Inactivate"), this);
	connect(m_inactivateAction, &QAction::triggered, this, &GeoDataRiverSurveyCrosssectionWindowGraphicsView::inactivateSelectedRows);
	m_moveAction = new QAction(tr("&Move"), this);
	connect(m_moveAction, &QAction::triggered, this, &GeoDataRiverSurveyCrosssectionWindowGraphicsView::moveSelectedRows);
	m_activateAction->setEnabled(false);
	m_inactivateAction->setEnabled(false);
	m_moveAction->setEnabled(false);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setupMenu()
{
	if (m_rightClickingMenu == nullptr) {
		m_rightClickingMenu = new QMenu(this);
		m_rightClickingMenu->addAction(m_activateAction);
		m_rightClickingMenu->addAction(m_inactivateAction);

		m_rightClickingMenu->addSeparator();
		QMenu* submenu = m_rightClickingMenu->addMenu(tr("Inactivate using &water elevation"));
		submenu->addAction(m_parentWindow->inactivateByWEOnlyThisAction());
		submenu->addAction(m_parentWindow->inactivateByWEAllAction());

		m_rightClickingMenu->addSeparator();

		m_rightClickingMenu->addAction(m_parentWindow->editFromSelectedPointAction());

		auto startEditMenu = m_rightClickingMenu->addMenu(tr("Edit by specifying slope point"));
		startEditMenu->addAction(m_parentWindow->leftAddAction());
		startEditMenu->addAction(m_parentWindow->leftSubAction());
		startEditMenu->addAction(m_parentWindow->rightAddAction());
		startEditMenu->addAction(m_parentWindow->rightSubAction());

		auto odnMenu = m_rightClickingMenu->addMenu(tr("Set to ODN data point"));
		odnMenu->addAction(m_parentWindow->odnLeftStartAction());
		odnMenu->addAction(m_parentWindow->odnLeftMiddleAction());
		odnMenu->addAction(m_parentWindow->odnLeftLowAction());
		odnMenu->addAction(m_parentWindow->odnRightLowAction());
		odnMenu->addAction(m_parentWindow->odnRightMiddleAction());
		odnMenu->addAction(m_parentWindow->odnRightStartAction());

		m_rightClickingMenu->addAction(m_parentWindow->addPointAction());
		m_rightClickingMenu->addAction(m_moveAction);
		m_rightClickingMenu->addAction(m_parentWindow->deleteAction());
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_parentWindow->addVegetationAction());
		m_rightClickingMenu->addAction(m_parentWindow->editSelectedVegetationAction());
		m_rightClickingMenu->addAction(m_parentWindow->deleteSelectedVegetationAction());

		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_parentWindow->targetRiverSurvey()->impl->m_calcAreaAction);
	}
	if (m_rightClickingMenuForEditCrosssectionMode == nullptr) {
		m_rightClickingMenuForEditCrosssectionMode = new QMenu(this);
		m_rightClickingMenuForEditCrosssectionMode->addAction(m_parentWindow->editFromSelectedPointWithDialogAction());
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::paintEvent(QPaintEvent* /*e*/)
{
	QPainter painter(viewport());
	QRect vp = painter.viewport();
	QMatrix matrix = getMatrix(vp);
	m_matrix = matrix;
	painter.setRenderHint(QPainter::Antialiasing);
	// Draw scales.
	drawScales(painter, matrix);
	// draw water surface.
	drawWaterSurfaceElevations(painter, matrix);
	// draw poly line cross points.
	drawPolyLineCrossPoints(painter);

	if (! m_gridMode) {
		// draw lines.
		// black lines
		QPen pen = QPen(Qt::black, 1);
		painter.setPen(pen);

		auto refPoint = m_parentWindow->referenceRiverPathPoint();
		if (refPoint != nullptr) {
			QColor c = m_parentWindow->referenceRiverPathPointColor();
			drawLine(refPoint, c, painter);
		}

		drawJmkLine(painter);
		drawOdnNbPoints(painter);
		drawLine(&m_oldLine, Qt::gray, painter);
		for (int i = 0; i < m_parentWindow->riverPathPoints().count(); ++i) {
			GeoDataRiverPathPoint* p = m_parentWindow->riverPathPoints().at(i);
			if (p == nullptr) {continue;}
			bool enabled = m_parentWindow->riverSurveyEnables().at(i);
			if (! enabled) {continue;}
			QColor c = m_parentWindow->riverSurveyColors().at(i);
			drawLine(p, c, painter);
		}
		// draw circles.
		drawCircle(painter);
		// draw selected circles.
		drawSelectionCircle(painter);
		// draw edit preview
		drawEditPreview(painter);
		drawSlopePointEditPreview(painter);
	}
	else {
		// draw black lines.
		drawLine(m_parentWindow->gridCreatingConditionPoint(), Qt::black, painter);
		// draw yellow squares.
		drawSquare(painter);
		// draw selected yellow squares.
		drawSelectionSquare(painter);
	}
	drawAspectRatio(painter);
}

QRect GeoDataRiverSurveyCrosssectionWindowGraphicsView::visualRect(const QModelIndex&) const
{
	return QRect();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::scrollTo(const QModelIndex&, ScrollHint)
{}

QModelIndex GeoDataRiverSurveyCrosssectionWindowGraphicsView::indexAt(const QPoint&) const
{
	viewport()->update();
	return QModelIndex();
}

QModelIndex GeoDataRiverSurveyCrosssectionWindowGraphicsView::moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers)
{
	viewport()->update();
	return QModelIndex();
}

int GeoDataRiverSurveyCrosssectionWindowGraphicsView::horizontalOffset() const
{
	return 0;
}

int GeoDataRiverSurveyCrosssectionWindowGraphicsView::verticalOffset() const
{
	return 0;
}

bool GeoDataRiverSurveyCrosssectionWindowGraphicsView::isIndexHidden(const QModelIndex&) const
{
	return false;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setSelection(const QRect& /*rect*/, QItemSelectionModel::SelectionFlags /*command*/)
{
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::selectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	updateActionStatus();
	viewport()->update();
}

QRegion GeoDataRiverSurveyCrosssectionWindowGraphicsView::visualRegionForSelection(const QItemSelection& /*selection*/) const
{
	return QRegion();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setSlopePointEditModeSetting(const QPointF& point, double slope)
{
	m_slopePointEditModeSlopePoint = point;
	m_slopePointEditModeSlope = slope;
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::enterAddPointMode()
{
	auto p = m_parentWindow->target();
	auto rs = p->riverSurvey();
	auto dialog = new GeoDataRiverSurveyCrosssectionWindow::PointAddDialog(p, rs, m_parentWindow);
	connect(this, &GeoDataRiverSurveyCrosssectionWindowGraphicsView::positionClicked, dialog, &GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::setPoint);

	m_mouseEventMode = meAddPoint;
	dialog->show();

	connect(dialog, &QObject::destroyed, this, &GeoDataRiverSurveyCrosssectionWindowGraphicsView::restoreMouseEventMode);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::enterAddVegetationMode()
{
	m_mouseEventMode = MouseEventMode::meAddVegetation;
	InformationDialog::information(this, tr("Information"), tr("Drag the region where you want to add a new vegetation"), "geodatariversurvey_add_vegetation");

	updateMouseCursor();
}

QAction* GeoDataRiverSurveyCrosssectionWindowGraphicsView::activateAction() const
{
	return m_activateAction;
}

QAction* GeoDataRiverSurveyCrosssectionWindowGraphicsView::inactivateAction() const
{
	return m_inactivateAction;
}

QAction* GeoDataRiverSurveyCrosssectionWindowGraphicsView::moveAction() const
{
	return m_moveAction;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawLine(GeoDataRiverPathPoint* point, const QColor& color, QPainter& painter)
{
	if (point == nullptr) {return;}

	auto& cross = point->crosssection();
	double leftShift = cross.leftShift();
	bool first = true;
	QPointF oldpoint, newpoint;
	painter.setPen(color);
	for (const GeoDataRiverCrosssection::Altitude& alt : cross.AltitudeInfo()) {
		if (! alt.active()) {continue;}

		newpoint = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
		if (! first) {
			painter.drawLine(oldpoint, newpoint);
		}
		oldpoint = newpoint;
		first = false;
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawOdnNbPoints(QPainter& painter)
{
	auto target = m_parentWindow->target();
	if (target == nullptr) { return; }

	const auto& odn = target->odn();
	std::vector<std::vector<QRectF>> drawnRects;

	drawOdnNbPoint(odn.nb(0), tr("Left Start"), m_displaySetting.odnStartColor, painter, drawnRects);
	drawOdnNbPoint(odn.nb(5), tr("Right Start"), m_displaySetting.odnStartColor, painter, drawnRects);

	drawOdnNbPoint(odn.nb(1), tr("Left Middle"), m_displaySetting.odnMiddleColor, painter, drawnRects);
	drawOdnNbPoint(odn.nb(4), tr("Right Middle"), m_displaySetting.odnMiddleColor, painter, drawnRects);

	drawOdnNbPoint(odn.nb(2), tr("Left Low"), m_displaySetting.odnLowColor, painter, drawnRects);
	drawOdnNbPoint(odn.nb(3), tr("Right Low"), m_displaySetting.odnLowColor, painter, drawnRects);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawOdnNbPoint(int index, const QString& label, const QColor& color, QPainter& painter, std::vector<std::vector<QRectF>>& drawnRects)
{
	if (m_parentWindow->target() == nullptr) { return; }

	const auto& xsec = m_parentWindow->target()->crosssection();
	double leftShift = xsec.leftShift();
	const auto& alist = xsec.AltitudeInfo();
	if (index < 0 || index >= static_cast<int> (alist.size())) {return;}

	painter.save();
	const auto alt = alist.at(index);

	QPen pen(color, 1, Qt::SolidLine);
	QBrush brush(color, Qt::SolidPattern);

	painter.setPen(pen);
	painter.setBrush(brush);
	QPointF point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
	QRectF r(point.x() - odnEllipseR, point.y() - odnEllipseR, odnEllipseR * 2, odnEllipseR * 2);
	painter.drawEllipse(r);

	painter.setFont(m_displaySetting.odnNbFont);
	QFontMetricsF metrics(m_displaySetting.odnNbFont);

	auto rect = metrics.boundingRect(label);
	QRectF textRect(point.x() - rect.width(), point.y() + ODN_VOFFSET, rect.width() + 5, rect.height() + 5);
	int row = findRowToDraw(textRect, &drawnRects);

	qreal rowHeight = rect.height() + 3;
	QRectF fontRect = QRectF(point.x() - rect.width(), point.y() + ODN_VOFFSET + row * rowHeight, rect.width() + 5, rect.height() + 5);
	painter.drawText(fontRect, Qt::AlignHCenter | Qt::AlignTop, label);
	painter.restore();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawJmkLine(QPainter& painter)
{
	std::unordered_set<int> selected;
	for (const QModelIndex& index : m_parentWindow->vegetationSelectionModel()->selectedIndexes()) {
		selected.insert(index.row());
	}

	auto target = m_parentWindow->target();
	if (target == nullptr) {return;}

	const auto& xsec = target->crosssection();
	double leftShift = xsec.leftShift();
	auto& alist = xsec.AltitudeInfo();
	const auto& jmk = target->jmk();

	painter.save();

	for (int idx = 0; idx < static_cast<int> (jmk.items().size()); ++idx) {
		const auto& item = jmk.items().at(idx);
		QPen pen(Qt::darkGreen, 2);

		if (item.dense == 0) {
			pen.setStyle(Qt::PenStyle::DashLine);
		}

		if (item.dead == 0) {
			pen.setColor(Qt::darkGray);
		}

		auto it = selected.find(idx);
		if (it != selected.end()) {
			pen.setWidth(4);
		}

		painter.setPen(pen);

		auto lb = std::lower_bound(alist.begin(), alist.end(), GeoDataRiverCrosssection::Altitude(item.distance - xsec.leftShift(), 0));
		auto ub = std::lower_bound(alist.begin(), alist.end(), GeoDataRiverCrosssection::Altitude(item.distance - xsec.leftShift() + item.width, 0));
		if (lb == alist.end() || ub == alist.end()) {continue;}

		auto lb_idx = lb - alist.begin();
		auto ub_idx = ub - alist.begin();
		std::vector<QPointF> points;

		// start
		if (lb_idx == 0) {
			const auto& a = alist.at(0);
			points.push_back(m_matrix.map(QPointF(a.position() + leftShift, a.height())));
			points.push_back(m_matrix.map(QPointF(a.position() + leftShift, a.height() + item.height)));
		} else {
			const auto& a1 = alist.at(lb_idx - 1);
			const auto& a2 = alist.at(lb_idx);
			auto r = (item.distance - xsec.leftShift() - a1.position()) / (a2.position() - a1.position());
			points.push_back(m_matrix.map(QPointF(item.distance, (1 - r) * a1.height() + r * a2.height())));
			points.push_back(m_matrix.map(QPointF(item.distance, (1 - r) * a1.height() + r * a2.height() + item.height)));
		}
		for (auto it = lb; it != ub; ++it) {
			points.push_back(m_matrix.map(QPointF(it->position() + leftShift, it->height() + item.height)));
		}
		// end
		{
			const auto& a1 = alist.at(ub_idx - 1);
			const auto& a2 = alist.at(ub_idx);
			auto r = (item.distance - xsec.leftShift() + item.width - a1.position()) / (a2.position() - a1.position());
			points.push_back(m_matrix.map(QPointF(item.distance + item.width, (1 - r) * a1.height() + r * a2.height() + item.height)));
			points.push_back(m_matrix.map(QPointF(item.distance + item.width, (1 - r) * a1.height() + r * a2.height())));
		}

		for (int i = 0; i < static_cast<int> (points.size()) - 1; ++i) {
			painter.drawLine(QLineF(points.at(i), points.at(i + 1)));
		}
	}

	painter.restore();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawCircle(QPainter& painter)
{
	if (m_parentWindow->target() == nullptr) {return;}

	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);
	QBrush inactiveBrush(Qt::green, Qt::SolidPattern);
	QBrush fixBrush(Qt::gray, Qt::SolidPattern);

	const auto& cross = m_parentWindow->target()->crosssection();
	double leftShift = cross.leftShift();

	painter.setPen(pen);
	const GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	for (const GeoDataRiverCrosssection::Altitude& alt : alist) {
		if (alt.active()) {
			painter.setBrush(activeBrush);
		} else {
			painter.setBrush(inactiveBrush);
		}
		QPointF point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
		QRectF r(point.x() - ellipseR, point.y() - ellipseR, ellipseR * 2, ellipseR * 2);
		painter.drawEllipse(r);
	}
	painter.setBrush(fixBrush);

	// draw fixed points.
	if (cross.fixedPointLSet()) {
		int lindex = cross.fixedPointLIndex();
		GeoDataRiverCrosssection::Altitude alt = alist.at(lindex);
		QPointF point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
		QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
		painter.drawEllipse(r);
	}
	if (cross.fixedPointRSet()) {
		int lindex = cross.fixedPointRIndex();
		GeoDataRiverCrosssection::Altitude alt = alist.at(lindex);
		QPointF point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
		QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
		painter.drawEllipse(r);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawSelectionCircle(QPainter& painter)
{
	if (m_parentWindow->target() == nullptr) {return;}

	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);
	QBrush inactiveBrush(Qt::green, Qt::SolidPattern);

	auto& cross = m_parentWindow->target()->crosssection();
	double leftShift = cross.leftShift();
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	painter.setPen(pen);
	QSet<int> drawnRows;
	for (const QModelIndex& index : selectionModel()->selectedIndexes()) {
		if (drawnRows.contains(index.row())) {continue;}
		const GeoDataRiverCrosssection::Altitude& alt = alist.at(index.row());
		if (alt.active()) {
			painter.setBrush(activeBrush);
		} else {
			painter.setBrush(inactiveBrush);
		}
		QPointF point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
		QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
		painter.drawEllipse(r);
		drawnRows.insert(index.row());
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawSquare(QPainter& painter)
{
	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::yellow, Qt::SolidPattern);
	QBrush inactiveBrush(Qt::blue, Qt::SolidPattern);

	GeoDataRiverPathPoint* blackpoint = m_parentWindow->gridCreatingConditionPoint();
	if (blackpoint == nullptr) {return;}
	auto& cross = blackpoint->crosssection();
	double leftShift = cross.leftShift();
	painter.setPen(pen);
	painter.setBrush(inactiveBrush);

	// left bank
	GeoDataRiverCrosssection::Altitude leftbank = cross.leftBank(true);
	QPointF point = m_matrix.map(QPointF(leftbank.position() + leftShift, leftbank.height()));
	QRectF r(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
	painter.drawRect(r);

	// right bank
	GeoDataRiverCrosssection::Altitude rightbank = cross.rightBank(true);
	point = m_matrix.map(QPointF(rightbank.position() + leftShift, rightbank.height()));
	r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
	painter.drawRect(r);

	// river center
	double height = blackpoint->lXSec()->interpolate(0).height();
	point = m_matrix.map(QPointF(leftShift, height));
	r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
	painter.drawRect(r);

	painter.setBrush(activeBrush);
	// control points between rivercenter and left bank
	for (double v : blackpoint->CenterToLeftCtrlPoints) {
		GeoDataRiverCrosssection::Altitude alt = blackpoint->lXSec()->interpolate(v);
		point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
		r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
		painter.drawRect(r);
	}
	// control points between rivercenter and right bank
	for (double v : blackpoint->CenterToRightCtrlPoints) {
		GeoDataRiverCrosssection::Altitude alt = blackpoint->rXSec()->interpolate(v);
		point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
		r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
		painter.drawRect(r);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawSelectionSquare(QPainter& painter)
{
	GeoDataRiverPathPoint* blackpoint = m_parentWindow->gridCreatingConditionPoint();
	if (blackpoint == nullptr) {return;}
	std::list<CtrlPointSelectionInfo>& sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();

	for (const CtrlPointSelectionInfo& info : sel) {
		if (info.Point == blackpoint) {
			double leftShift = blackpoint->crosssection().leftShift();
			if (info.Position == GeoDataRiverPathPoint::pposCenterToLeft) {
				double v = blackpoint->CenterToLeftCtrlPoints[info.Index];
				GeoDataRiverCrosssection::Altitude alt = blackpoint->lXSec()->interpolate(v);
				QPointF point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
				QRectF r(point.x() - selectedSquareR, point.y() - selectedSquareR, selectedSquareR * 2, selectedSquareR * 2);
				painter.drawRect(r);
			} else if (info.Position == GeoDataRiverPathPoint::pposCenterToRight) {
				double v = blackpoint->CenterToRightCtrlPoints[info.Index];
				GeoDataRiverCrosssection::Altitude alt = blackpoint->rXSec()->interpolate(v);
				QPointF point = m_matrix.map(QPointF(alt.position() + leftShift, alt.height()));
				QRectF r(point.x() - selectedSquareR, point.y() - selectedSquareR, selectedSquareR * 2, selectedSquareR * 2);
				painter.drawRect(r);
			}
		}
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawScales(QPainter& painter, const QMatrix& matrix)
{
	QWidget* w = viewport();
	QMatrix invMatrix = matrix.inverted();
	QPointF mins, maxs;
	mins = invMatrix.map(QPointF(0, w->height()));
	maxs = invMatrix.map(QPointF(w->width(), 0));

	// set pen.
	QPen scalePen(QColor(160, 160, 50), 1, Qt::SolidLine, Qt::RoundCap);
	painter.setPen(scalePen);

	double xoffset = 5;
	double yoffset = 5;
	double fontoffset = 4;
	double mainruler = 5;
	double subruler = 3;

	double xScale = m_displaySetting.bgHScaleInterval;
	double xSubScale = m_displaySetting.bgHSubScaleInterval;
	double yScale = m_displaySetting.bgVScaleInterval;
	double ySubScale = m_displaySetting.bgVSubScaleInterval;

	if (m_displaySetting.bgHScaleAuto) {
		calcAutoScale(maxs.x() - mins.x(), &xScale, &xSubScale);
	}
	if (m_displaySetting.bgVScaleAuto) {
		calcAutoScale(maxs.y() - mins.y(), &yScale, &ySubScale);
	}

	if (m_parentWindow->impl->m_gridDisplayCheckBox->isChecked()) {
		painter.save();

		if (m_displaySetting.bgGridType == GeoDataRiverSurveyCrossSectionDisplaySetting::BackgroundGridType::Lines) {
			// lines
			// scales
			QPen pen(m_displaySetting.bgGridColor);
			pen.setStyle(Qt::SolidLine);
			painter.setPen(pen);

			double x = std::floor(mins.x() / xScale) * xScale;
			while (x < maxs.x()) {
				auto from = matrix.map(QPointF(x, maxs.y()));
				auto to = from;
				to.setY(w->height());
				painter.drawLine(from, to);
				x += xScale;
			}
			double y = std::floor(mins.y() / yScale) * yScale;
			while (y < maxs.y()) {
				auto from = matrix.map(QPointF(mins.x(), y));
				auto to = from;
				to.setX(w->width());
				painter.drawLine(from, to);
				y += yScale;
			}
			// subscales
			pen.setStyle(Qt::DashLine);
			painter.setPen(pen);
			x = std::floor(mins.x() / xSubScale) * xSubScale;
			while (x < maxs.x()) {
				auto from = matrix.map(QPointF(x, maxs.y()));
				auto to = from;
				to.setY(w->height());
				painter.drawLine(from, to);
				x += xSubScale;
			}
			y = std::floor(mins.y() / ySubScale) * ySubScale;
			while (y < maxs.y()) {
				auto from = matrix.map(QPointF(mins.x(), y));
				auto to = from;
				to.setX(w->width());
				painter.drawLine(from, to);
				y += ySubScale;
			}
		} else {
			// dots
			QPen pen(m_displaySetting.bgGridColor);
			pen.setStyle(Qt::SolidLine);
			painter.setPen(pen);
			QBrush brush(m_displaySetting.bgGridColor);
			painter.setBrush(brush);

			double x = std::floor(mins.x() / xScale) * xScale;
			while (x < maxs.x()) {
				double y = std::floor(mins.y() / yScale) * yScale;
				while (y < maxs.y()) {
					auto from = matrix.map(QPointF(x, y));
					from.setX(from.x() - 1);
					from.setY(from.y() - 1);
					QPointF to(from.x() + 2, from.y() + 2);
					painter.drawRect(QRectF(from, to));
					y += yScale;
				}
				x += xScale;
			}
			x = std::floor(mins.x() / xSubScale) * xSubScale;
			while (x < maxs.x()) {
				double y = std::floor(mins.y() / ySubScale) * ySubScale;
				while (y < maxs.y()) {
					auto p = matrix.map(QPointF(x, y));
					painter.drawPoint(p);
					y += ySubScale;
				}
				x += xSubScale;
			}
		}
		painter.restore();
	}

	if (m_parentWindow->impl->m_scaleDisplayCheckBox->isChecked()) {
		painter.save();

		painter.setPen(m_displaySetting.distanceMarkersColor);
		painter.setFont(m_displaySetting.distanceMarkersFont);
		QFontMetrics metrics(m_displaySetting.distanceMarkersFont);

		// Horizontal line
		QPointF from, to;
		from = QPointF(0, yoffset);
		to = QPointF(w->width(), yoffset);
		painter.drawLine(from, to);

		// draw X scales
		double x = std::floor(mins.x() / xScale) * xScale;
		while (x < maxs.x()) {
			from = matrix.map(QPointF(x, maxs.y()));
			from.setY(yoffset);
			to = from;
			to.setY(yoffset + mainruler);
			painter.drawLine(from, to);

			QPointF fontPos = to;
			fontPos.setY(yoffset + mainruler + fontoffset);
			QString str = QString("%1").arg(x);
			auto rect = metrics.boundingRect(str);
			QRectF fontRect(fontPos.x() - rect.width() / 2, fontPos.y(), rect.width() + 5, rect.height() + 5);
			painter.drawText(fontRect, Qt::AlignHCenter | Qt::AlignTop, str);
			x += xScale;
		}
		// draw X sub scales.
		x = std::floor(mins.x() / xSubScale) * xSubScale;
		while (x < maxs.x()) {
			from = matrix.map(QPointF(x, maxs.y()));
			from.setY(from.y() + yoffset);
			to = from;
			to.setY(to.y() + yoffset + subruler);
			painter.drawLine(from, to);
			x += xSubScale;
		}

		// Vertical line
		from = QPointF(xoffset, 0);
		to = QPointF(xoffset, w->height());
		painter.drawLine(from, to);

		// draw Y scales
		double y = std::floor(mins.y() / yScale) * yScale;
		while (y < maxs.y()) {
			from = matrix.map(QPointF(mins.x(), y));
			from.setX(xoffset);
			to.setX(xoffset + mainruler);
			to.setY(from.y());
			painter.drawLine(from, to);

			QPointF fontPos = to;
			fontPos.setX(xoffset + mainruler + fontoffset);
			QString str = QString("%1").arg(y);
			auto rect = metrics.boundingRect(str);
			QRectF fontRect(fontPos.x(), fontPos.y() - rect.height() / 2, rect.width() + 5, rect.height() + 5);
			painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignVCenter, str);
			y += yScale;
		}
		// draw Y sub scales.
		y = std::floor(mins.y() / ySubScale) * ySubScale;
		while (y < maxs.y()) {
			from = matrix.map(QPointF(mins.x(), y));
			from.setX(xoffset);
			to.setX(xoffset + subruler);
			to.setY(from.y());
			painter.drawLine(from, to);
			y += ySubScale;
		}
		painter.restore();
	}

	if (m_parentWindow->impl->m_markersDisplayCheckBox->isChecked()) {
		painter.save();

		// line at x = 0;
		auto from = matrix.map(QPointF(0, 0));
		from.setY(0);
		auto to = from;
		to.setY(w->height());

		QPen pen(m_displaySetting.lbBankMarkersColor);
		pen.setStyle(Qt::DotLine);
		painter.setPen(pen);
		painter.drawLine(from, to);

		painter.setFont(m_displaySetting.lbBankMarkersFont);
		QFontMetricsF metrics(m_displaySetting.lbBankMarkersFont);

		// left bank
		QString label = tr("Left Bank Side");
		auto rect = metrics.boundingRect(label);
		QRectF fontRect = QRectF(from.x() - bankHOffset - rect.width(), bankVOffset, rect.width() + 5, rect.height() + 5);
		painter.drawText(fontRect, Qt::AlignRight | Qt::AlignTop, label);

		// right bank side
		label = tr("Right Bank Side");
		rect = metrics.boundingRect(label);
		fontRect = QRectF(from.x() + bankHOffset, bankVOffset, rect.width() + 5, rect.height() + 5);
		painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, label);

		painter.restore();
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawWaterSurfaceElevations(QPainter& painter, const QMatrix& matrix)
{
	auto weGroup = m_parentWindow->waterElevationGroup();
	if (weGroup == nullptr) {return;}

	for (int i = 0; i < weGroup->hydraulicDatas().size(); ++i) {
		drawWaterSurfaceElevation(i, painter, matrix);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawWaterSurfaceElevation(int index, QPainter& painter, const QMatrix& matrix)
{
	auto weGroup = m_parentWindow->waterElevationGroup();
	if (weGroup == nullptr) {return;}

	auto name = m_parentWindow->crosssectionName();
	auto we = m_parentWindow->waterElevation(index);

	bool specified = false;
	double val = 0;

	auto weItem = we->getItem(name);
	if (weItem != nullptr) {
		specified = weItem->isSpecified();
		val = weItem->value();
	}

	if (! specified) {return;}

	QWidget* w = viewport();
	QMatrix invMatrix = matrix.inverted();
	QPointF mins, maxs;
	mins = invMatrix.map(QPointF(0, w->height()));
	maxs = invMatrix.map(QPointF(w->width(), 0));

	QPointF from = matrix.map(QPointF(0, val));
	from.setX(0);
	QPointF to(w->width(), from.y());

	QPen oldPen = painter.pen();
	QPen pen(we->color());
	painter.setPen(pen);
	painter.drawLine(from, to);

	QPointF points[6];

	int wse_offset = w->width() - WSE_WIDTH;

	points[0] = QPointF(wse_offset, from.y() - 5);
	points[1] = QPointF(wse_offset + 5, from.y() - 15);
	points[2] = points[1];
	points[3] = QPointF(wse_offset - 5, from.y() - 15);
	points[4] = points[3];
	points[5] = points[0];

	painter.drawLines(points, 3);

	QRectF fontRect;
	fontRect = QRectF(wse_offset + 10, from.y() - 20, WSE_WIDTH, 20);
	int precision = 2;
	double limit = 1;
	for (int i = 0; i < 5; ++i) {
		if (val <= -limit || limit <= val) {++ precision;}
		limit *= 10;
	}
	QString caption = QString("%1: %2").arg(we->caption()).arg(QString::number(val, 'g', precision));
	painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignVCenter, caption);

	painter.setPen(oldPen);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawCrossPoint(const QPointF& origin, const QPointF& direction, const QPointF& left, const QPointF& right, const QPointF& q1, const QPointF& q2, double leftShift, const QString& name, const QColor& color, std::vector<std::vector<QRectF> >* drawnRects, QPainter& painter)
{
	int topMargin = 80;
	int lineHeight = 15;
	int lineMargin = 10;
	int fontMargin = 5;
	int fontHeight = 18;

	int rowHeight = 35;

	QPointF crossPoint;
	double r, s;
	bool crosses = iRIC::intersectionPoint(left, right, q1, q2, &crossPoint, &r, &s);
	if (! crosses) {return;}
	if (r < 0 || r > 1) {return;}
	if (s < 0 || s > 1) {return;}

	QPointF diff(crossPoint.x() - origin.x(), crossPoint.y() - origin.y());
	double pos = QPointF::dotProduct(direction, diff);

	QPointF mappedPos = m_matrix.map(QPointF(pos + leftShift, 0));

	painter.save();
	painter.setPen(color);

	QFontMetricsF metrics(painter.font());
	QRectF rect = metrics.boundingRect(name);
	QRectF textRect(mappedPos.x(), 0, rect.width() + lineMargin, fontHeight);

	int row = findRowToDraw(textRect, drawnRects);

	painter.drawLine(QPointF(mappedPos.x(), topMargin + row * rowHeight), QPointF(mappedPos.x(), topMargin + lineHeight + row * rowHeight));
	painter.drawLine(QPointF(mappedPos.x(), topMargin + row * rowHeight), QPointF(mappedPos.x() + rect.width() + lineMargin, topMargin + row * rowHeight));
	QRectF fontRect(mappedPos.x() + fontMargin, topMargin - fontHeight + row * rowHeight, rect.width(), fontHeight);

	painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, name);
	painter.restore();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawAspectRatio(QPainter& painter)
{
	painter.save();

	painter.setPen(m_displaySetting.aspectRatioColor);
	painter.setFont(m_displaySetting.aspectRatioFont);

	if (! m_parentWindow->impl->m_aspectRatioDisplayCheckBox->isChecked()) {return;}

	QSize windowSize = size();

	auto aspectRatioStr = tr("Aspect ratio (V/H): 1 / %1").arg(aspectRatio());
	QFontMetricsF metrics(painter.font());
	auto rect = metrics.boundingRect(aspectRatioStr);

	auto left = windowSize.width() - ASPECT_RATIO_RIGHT_MARGIN - rect.width();
	auto top = windowSize.height() - ASPECT_RATIO_BOTTOM_MARGIN - rect.height();
	QRectF fontRect(left, top, rect.width() + 1, rect.height() + 1);
	painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, aspectRatioStr);

	painter.restore();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawPolyLineCrossPoints(QPainter& painter)
{
	std::vector<std::vector<QRectF> > drawnRects;

	double marginRate = 3;

	auto gridTypeDataItem = dynamic_cast<PreProcessorGeoDataTopDataItemI*>
			(m_parentWindow->targetRiverSurvey()->geoDataDataItem()->parent()->parent());
	auto refGroupDataItem = gridTypeDataItem->groupDataItem("_referenceinformation");

	auto targetPoint = m_parentWindow->target();
	if (targetPoint == nullptr) {return;}

	QPointF origin = targetPoint->position();
	QPointF left, right;
	if (targetPoint->nextPoint() == nullptr) {
		auto prevPoint = targetPoint->previousPoint();
		left = prevPoint->leftBank()->interpolate(1);
		right = prevPoint->rightBank()->interpolate(1);
	} else {
		left = targetPoint->leftBank()->interpolate(0);
		right = targetPoint->rightBank()->interpolate(0);
	}
	QPointF marginedLeft = origin + marginRate * (left - origin);
	QPointF marginedRight = origin + marginRate * (right - origin);

	double xmin = qMin(marginedLeft.x(), marginedRight.x());
	double xmax = qMax(marginedLeft.x(), marginedRight.x());
	double ymin = qMin(marginedLeft.y(), marginedRight.y());
	double ymax = qMax(marginedLeft.y(), marginedRight.y());

	for (auto child : refGroupDataItem->childItems()) {
		auto geoDataItem = dynamic_cast<PreProcessorGeoDataDataItemI*> (child);
		if (geoDataItem->standardItem()->checkState() != Qt::Checked) {continue;}

		auto polyLine = dynamic_cast<GeoDataPolyLine*> (geoDataItem->geoData());
		if (polyLine != nullptr) {
			auto line = polyLine->polyLine()->polyLine();
			for (int i = 0; i < static_cast<int>(line.size()) - 1; ++i) {
				QPointF p1 = line.at(i);
				QPointF p2 = line.at(i + 1);

				drawCrossPoint(origin, targetPoint->crosssectionDirection(), marginedLeft, marginedRight, p1, p2, targetPoint->crosssection().leftShift(), polyLine->caption(), polyLine->color(), &drawnRects, painter);
			}
		}
		auto polyLineGroup = dynamic_cast<GeoDataPolyLineGroup*> (geoDataItem->geoData());
		if (polyLineGroup != nullptr) {
			auto lines = polyLineGroup->polyLinesInBoundingBox(xmin, xmax, ymin, ymax);
			for (GeoDataPolyLineGroupPolyLine* l : lines) {
				geos::geom::LineString* ls = l->getGeosLineString();
				for (int i = 0; i < ls->getNumPoints() - 1; ++i) {
					const auto& c1 = ls->getCoordinateN(i);
					const auto& c2 = ls->getCoordinateN(i + 1);
					QPointF p1(c1.x, c1.y);
					QPointF p2(c2.x, c2.y);

					drawCrossPoint(origin, targetPoint->crosssectionDirection(), marginedLeft, marginedRight, p1, p2, targetPoint->crosssection().leftShift(), l->name() , polyLineGroup->color(), &drawnRects, painter);
				}
			}
		}
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawEditPreview(QPainter& painter)
{
	if (m_mouseEventMode != meEditCrosssection) {return;}

	const auto index = selectionModel()->selectedIndexes().at(0);
	auto& xsec = m_parentWindow->target()->crosssection();
	double leftShift = xsec.leftShift();
	const auto& alist = xsec.AltitudeInfo();
	const auto& selectedAlt = alist.at(index.row());

	auto startP = m_matrix.map(QPointF(selectedAlt.position() + leftShift, selectedAlt.height()));
	auto endP = m_matrix.map(QPointF(m_editAltitudePreview.position() + leftShift, m_editAltitudePreview.height()));

	painter.save();
	painter.setPen(Qt::black);
	painter.drawLine(QLineF(startP, endP));

	// draw ratio
	QPointF p = (startP + endP) / 2.0;
	drawTextWithWhiteLines(&painter, p, m_editRatio);

	painter.setPen(Qt::blue);

	QPointF endPH = m_matrix.map(QPointF(m_editAltitudePreview.position() + leftShift, selectedAlt.height()));
	painter.drawLine(QLineF(startP, endPH));
	painter.drawLine(QLineF(endP, endPH));

	// draw horizontal distance
	double distH = std::fabs(selectedAlt.position() - m_editAltitudePreview.position());
	QPointF p2(p.x(), startP.y() - PREVIEW_LABEL_OFFSET);
	drawTextWithWhiteLines(&painter, p2, QString::number(distH, 'f', 2));

	// draw vertical distance
	double distV = std::fabs(selectedAlt.height() - m_editAltitudePreview.height());
	QPointF p3(endP.x(), p.y());
	if (distV != 0) {
		QFontMetricsF metrics(painter.font());
		auto distVStr = QString::number(distV, 'f', 2);
		auto rect = metrics.boundingRect(distVStr);

		p2 = QPointF(p3.x() + PREVIEW_LABEL_OFFSET, p3.y());
		if ((m_editAltitudePreview.position() - selectedAlt.position()) < 0) {
			p2 = QPointF(p3.x() - PREVIEW_LABEL_OFFSET - rect.width(), p3.y());
		}
		drawTextWithWhiteLines(&painter, p2, distVStr);
	}

	painter.restore();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawSlopePointEditPreview(QPainter& painter)
{
	if (m_mouseEventMode != MouseEventMode::meEditWithSlopePoint) {return;}

	painter.save();
	QPointF point = m_matrix.map(m_slopePointEditModeSlopePoint);
	QPen pen(QColor(150, 150, 150), 1, Qt::PenStyle::DashLine);
	painter.setPen(pen);

	QPointF left, right;
	GeoDataRiverSurveyCrosssectionSlopePointEditDialog::calculateLeftAndRightPoints(m_parentWindow->target()->crosssection().AltitudeInfo(), m_parentWindow->target()->crosssection().leftShift(), m_slopePointEditMode, m_slopePointEditModeSlopePoint, m_slopePointEditModeSlope, &left, &right);

	painter.drawLine(QLineF(m_matrix.map(left), point));
	painter.drawLine(QLineF(m_matrix.map(right), point));

	painter.restore();
}

QRectF GeoDataRiverSurveyCrosssectionWindowGraphicsView::getRegion()
{
	QRectF ret(0., 0., 0., 0.);
	bool first = true;
	for (int i = 0; i < m_parentWindow->riverPathPoints().count(); ++i) {
		GeoDataRiverPathPoint* p = m_parentWindow->riverPathPoints()[i];
		if (p == nullptr) {continue;}

		double leftShift = p->crosssection().leftShift();
		GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		for (int j = 0; j < alist.size(); ++j) {
			GeoDataRiverCrosssection::Altitude alt = alist.at(j);
			double pos = alt.position() + leftShift;

			if (first || pos < ret.left()) {ret.setLeft(pos);}
			if (first || pos > ret.right()) {ret.setRight(pos);}
			if (first || alt.height() < ret.top()) {ret.setTop(alt.height());}
			if (first || alt.height() > ret.bottom()) {ret.setBottom(alt.height());}
			first = false;
		}
	}

	if (std::abs(ret.left() - ret.right()) < DELTA) {
		double center = ret.left();
		ret.setLeft(center - 0.5);
		ret.setRight(center + 0.5);
	}
	if (std::abs(ret.top() - ret.bottom()) < DELTA) {
		double center = ret.top();
		ret.setTop(center - 0.5);
		ret.setBottom(center + 0.5);
	}
	return ret;
}

QMatrix GeoDataRiverSurveyCrosssectionWindowGraphicsView::getMatrix(const QRect& viewport)
{
	QMatrix translate1, scale, translate2;

	translate1 = QMatrix(1, 0, 0, 1, - m_center.x(), - m_center.y());
	scale = QMatrix(m_scaleX, 0, 0, - m_scaleY, 0, 0);
	translate2 = QMatrix(1, 0, 0, 1, viewport.width() * 0.5, viewport.height() * 0.5);

	return translate1 * scale * translate2;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraFit()
{
	auto r = getRegion();
	if (! m_parentWindow->isRegionFixed()) {
		m_center.setX((r.left() + r.right()) * 0.5);
		m_center.setY((r.top() + r.bottom()) * 0.5);
	}

	if (! m_parentWindow->isRegionFixed() && ! m_parentWindow->isAspectRatioFixed()) {
		m_scaleX = (viewport()->width() - iLeftMargin - iRightMargin) / (r.width() * (1 + fLeftMargin + fRightMargin));
		m_scaleY = (viewport()->height() - iTopMargin - iBottomMargin) / (r.height() * (1 + fTopMargin + fBottomMargin));
	}

	viewport()->update();
	emit drawnRegionChanged();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveLeft()
{
	translate(- moveWidth(), 0);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveRight()
{
	translate(moveWidth(), 0);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveUp()
{
	translate(0, - moveWidth());
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveDown()
{
	translate(0, moveWidth());
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomIn()
{
	zoom(1.2, 1.2);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomOut()
{
	zoom(1. / 1.2, 1. / 1.2);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomInX()
{
	zoom(1.2, 1);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomOutX()
{
	zoom(1. / 1.2, 1);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomInY()
{
	zoom(1, 1.2);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomOutY()
{
	zoom(1, 1. / 1.2);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	int diffx = event->x() - m_oldPosition.x();
	int diffy = event->y() - m_oldPosition.y();

	if (m_viewMouseEventMode == vmeTranslating) {
		translate(diffx, diffy);
	} else if (m_viewMouseEventMode == vmeZooming) {
		double scaleX = 1 + diffx * 0.02;
		double scaleY = 1 - diffy * 0.02;
		if (scaleX < 0.5) {scaleX = 0.5;}
		if (scaleY < 0.5) {scaleY = 0.5;}
		if (scaleX > 2) {scaleX = 2;}
		if (scaleY > 2) {scaleY = 2;}
		if (m_parentWindow->isAspectRatioFixed()) {
			zoom(scaleY, scaleY);
		} else {
			zoom(scaleX, scaleY);
		}
	} else if ((m_mouseEventMode == meNormal || m_mouseEventMode == meMovePrepare || m_mouseEventMode == meDragVegetationPrepare) && ! m_modelessDialogIsOpen) {
		m_mouseEventMode = meNormal;
		if (m_gridMode) {
			// find selected points near the mouse cursor.
			std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
			QPointF mins(event->x() - 5, event->y() + 5);
			QPointF maxs(event->x() + 5, event->y() - 5);
			QMatrix invMatrix = m_matrix.inverted();
			QPointF mappedMins = invMatrix.map(mins);
			QPointF mappedMaxs = invMatrix.map(maxs);
			if (continuousGridSelection()) {
				CtrlPointSelectionInfo info = sel.front();
				if (info.Point == m_parentWindow->gridCreatingConditionPoint()) {
					if (info.Position == GeoDataRiverPathPoint::pposCenterToLeft) {
						for (auto it = sel.begin(); it != sel.end(); ++it) {
							info = *it;
							GeoDataRiverCrosssection::Altitude alt = info.Point->lXSec()->interpolate(info.Point->CtrlPoints(info.Position).at(info.Index));
							if (alt.position() >= mappedMins.x() &&
									alt.position() <= mappedMaxs.x() &&
									alt.height() >= mappedMins.y() &&
									alt.height() <= mappedMaxs.y()) {
								// one of the selected points is near enough to the mouse pointer.
								m_mouseEventMode = meMovePrepare;
							}
						}
					} else if (info.Position == GeoDataRiverPathPoint::pposCenterToRight) {
						for (auto it = sel.begin(); it != sel.end(); ++it) {
							info = *it;
							GeoDataRiverCrosssection::Altitude alt = info.Point->rXSec()->interpolate(info.Point->CtrlPoints(info.Position).at(info.Index));
							if (alt.position() >= mappedMins.x() &&
									alt.position() <= mappedMaxs.x() &&
									alt.height() >= mappedMins.y() &&
									alt.height() <= mappedMaxs.y()) {
								// one of the selected points is near enough to the mouse pointer.
								m_mouseEventMode = meMovePrepare;
							}
						}
					}
				}
			}
		} else {
			QPointF mins(event->x() - 5, event->y() + 5);
			QPointF maxs(event->x() + 5, event->y() - 5);
			QMatrix invMatrix = m_matrix.inverted();
			QPointF mappedMins = invMatrix.map(mins);
			QPointF mappedMaxs = invMatrix.map(maxs);

			// find jmk data near the mouse cursor;
			const auto& jmkItems = m_parentWindow->target()->jmk().items();
			for (int i = 0; i < static_cast<int>(jmkItems.size()); ++i) {
				const auto& jmkItem = jmkItems.at(i);
				double left = jmkItem.distance;
				double right = left + jmkItem.width;

				if (left >= mappedMins.x() && left <= mappedMaxs.x()) {
					m_mouseEventMode = meDragVegetationPrepare;
					m_dragJmkItemIndex = i;
					m_dragJmkRight = false;
				}
				if (right >= mappedMins.x() && right <= mappedMaxs.x()) {
					m_mouseEventMode = meDragVegetationPrepare;
					m_dragJmkItemIndex = i;
					m_dragJmkRight = true;
				}
			}

			// find selected points near the mouse cursor.
			QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
			GeoDataRiverPathPoint* p = m_parentWindow->target();
			double leftShift = p->crosssection().leftShift();
			GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
			if (continuousSelection()) {
				for (auto it = selectedPoints.begin(); it != selectedPoints.end(); ++it) {
					int index = it->row();
					GeoDataRiverCrosssection::Altitude& alt = alist[index];
					if (alt.position() + leftShift >= mappedMins.x() &&
							alt.position() + leftShift <= mappedMaxs.x() &&
							alt.height() >= mappedMins.y() &&
							alt.height() <= mappedMaxs.y()) {
						// one of the selected points is near enough to the mouse pointer.
						m_mouseEventMode = meMovePrepare;
					}
				}
			}
		}
		updateMouseCursor();
	} else if (m_mouseEventMode == meSelecting) {
		QPoint topLeft(qMin(m_rubberOrigin.x(), event->x()), qMin(m_rubberOrigin.y(), event->y()));
		QSize size(qAbs(m_rubberOrigin.x() - event->x()), qAbs(m_rubberOrigin.y() - event->y()));
		QRect rect(topLeft, size);
		m_rubberBand->setGeometry(rect);
		viewport()->update();
	} else if (m_mouseEventMode == meMove) {
		if (m_gridMode) {
			std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
			double offset = getGridCtrlPointOffset(m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand(true, offset, m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()));
		} else {
			GeoDataRiverCrosssection::AltitudeList newAltitudeList = m_oldAltitudeList;
			updateAltitudeList(newAltitudeList, m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GeoDataRiverSurvey::MouseEditCrosssectionCommand(m_parentWindow->target(), newAltitudeList, m_oldAltitudeList, m_parentWindow, m_parentWindow->targetRiverSurvey(), true));
		}
	} else if (m_mouseEventMode == meEditCrosssection) {
		bool ctrlPressed = ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier);
		m_editAltitudePreview = createAltitude(event->pos(), &m_editRatio, ctrlPressed);
		viewport()->update();
	} else if (m_mouseEventMode == meEditWithSlopePoint) {
		auto invMatrix = m_matrix.inverted();
		m_slopePointEditModeSlopePoint = invMatrix.map(QPointF(event->pos()));
		viewport()->update();
	} else if (m_mouseEventMode == meAddVegetation) {
		QPoint topLeft(qMin(m_rubberOrigin.x(), event->x()), qMin(m_rubberOrigin.y(), event->y()));
		QSize size(qAbs(m_rubberOrigin.x() - event->x()), qAbs(m_rubberOrigin.y() - event->y()));
		QRect rect(topLeft, size);
		if (m_rubberBand != nullptr) {
			m_rubberBand->setGeometry(rect);
		}
		viewport()->update();
	} else if (m_mouseEventMode == meDragVegetation) {
		auto invMatrix = m_matrix.inverted();
		auto oldP = invMatrix.map(QPointF(m_oldPosition));
		auto newP = invMatrix.map(QPointF(event->pos()));
		auto dx = round2(newP.x() - oldP.x());

		auto target = m_parentWindow->target();
		auto oldJmk = target->jmk();
		auto newJmk = oldJmk;
		auto& editTargetItem = newJmk.items().at(m_dragJmkItemIndex);
		if (m_dragJmkRight) {
			editTargetItem.width += dx;
			if (editTargetItem.width < 0) {
				editTargetItem.width = 0.01;
			}
		} else {
			editTargetItem.width -= dx;
			if (editTargetItem.width < 0) {
				dx -= (0.01 - editTargetItem.width);
				editTargetItem.width = 0.01;
			}
			editTargetItem.distance += dx;
		}

		iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditJmkDataByDragCommand(true, m_dragJmkItemIndex, m_dragJmkRight, newJmk, oldJmk, target, m_parentWindow));
	}

	// Emit position change signal for status bar
	QMatrix invMatrix = m_matrix.inverted();
	QPointF dataCoordinate = invMatrix.map(QPointF(event->pos()));
	emit positionChangedForStatusBar(dataCoordinate);

	m_oldPosition = event->pos();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::mousePressEvent(QMouseEvent* event)
{
	if (event->modifiers() == Qt::ControlModifier) {
		switch (event->button()) {
		case Qt::LeftButton:
			// translate
			if (! m_parentWindow->isRegionFixed()) {
				m_viewMouseEventMode = vmeTranslating;
			}
			break;
		case Qt::MidButton:
			// zoom.
			if (! m_parentWindow->isRegionFixed()) {
				m_viewMouseEventMode = vmeZooming;
			}
			break;
		default:
			break;
		}
		m_oldPosition = event->pos();
		updateMouseCursor();
		return;
	}

	switch (m_mouseEventMode) {
	case meNormal:
		if (! m_modelessDialogIsOpen) {
			if (event->button() == Qt::LeftButton) {
				// start selecting.
				m_mouseEventMode = meSelecting;
				if (m_rubberBand == nullptr) {
					m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
				}
				m_rubberOrigin = event->pos();
				m_rubberBand->setGeometry(m_rubberOrigin.x(), m_rubberOrigin.y(), 0, 0);
				m_rubberBand->show();
			} else if (event->button() == Qt::RightButton) {
				m_dragStartPoint = event->pos();
			}
		}
		break;
	case meEditCrosssection:
		if (event->button() == Qt::RightButton) {
			m_dragStartPoint = event->pos();
		}
		break;
	case meEditWithSlopePoint:
		if (event->button() == Qt::LeftButton) {
			QMatrix invMatrix = m_matrix.inverted();
			QPointF point = invMatrix.map(QPointF(event->pos()));

			emit positionClicked(point);
		}
		break;

	case meMovePrepare:
		if (event->button() == Qt::LeftButton) {
			// start dragging points.
			if (m_gridMode) {
				// start dragging points.
				m_dragStartPoint = event->pos();
				inspectGridLimits(&m_dragLimit.min, &m_dragLimit.max);
			} else {
				m_dragStartPoint = event->pos();
				m_oldAltitudeList = m_parentWindow->target()->crosssection().AltitudeInfo();
				inspectLimits(&m_dragLimit.minSet, &m_dragLimit.min, &m_dragLimit.maxSet, &m_dragLimit.max);
			}
			m_mouseEventMode = meMove;
			updateMouseCursor();
		} else if (event->button() == Qt::RightButton) {
			m_dragStartPoint = event->pos();
		}
		break;

	case meAddVegetation:
		if (event->button() == Qt::LeftButton) {
			// start selecting.
			if (m_rubberBand == nullptr) {
				m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
			}
			m_rubberOrigin = event->pos();
			m_rubberBand->setGeometry(m_rubberOrigin.x(), m_rubberOrigin.y(), 0, 0);
			m_rubberBand->show();
		}
		break;
	case meDragVegetationPrepare:
		if (event->button() == Qt::LeftButton) {
			m_mouseEventMode = meDragVegetation;
		}
	case meAddPoint:
		if (event->button() == Qt::LeftButton) {
			auto invMatrix = m_matrix.inverted();
			auto newPoint = invMatrix.map(QPointF(event->pos()));
			emit positionClicked(newPoint);
		}

	default:
		break;
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_viewMouseEventMode != vmeNormal) {
		m_viewMouseEventMode = vmeNormal;
		updateMouseCursor();
		return;
	}

	switch (m_mouseEventMode) {
	case meNormal:
	case meMovePrepare:
	case meEditCrosssection:
		if (event->button() == Qt::RightButton && ! m_modelessDialogIsOpen) {
			if (iRIC::isNear(m_dragStartPoint, event->pos())) {
				// show right-clicking menu.
				setupMenu();
				QMenu* m = m_rightClickingMenu;
				if (m_mouseEventMode == meEditCrosssection) {
					m = m_rightClickingMenuForEditCrosssectionMode;
				}
				m->move(event->globalPos());
				m->show();
			}
		} else if (m_mouseEventMode == meEditCrosssection && event->button() == Qt::LeftButton) {
			QString ratio;
			bool ctrlPressed = ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier);
			auto newAltitude = createAltitude(event->pos(), &ratio, ctrlPressed);
			editCrossSection(newAltitude);
		}
		break;
	case meSelecting:
		// finish selecting.
		m_rubberBand->hide();
		if (iRIC::isNear(m_rubberOrigin, event->pos())) {
			// press point and release point are too near.
			QPoint p1(event->pos().x() - 3, event->pos().y() - 3);
			QPoint p2(event->pos().x() + 3, event->pos().y() + 3);
			selectPoints(p1, p2);
		} else {
			// select the point inside the rubberband geometry.
			selectPoints(m_rubberOrigin, event->pos());
		}
		m_mouseEventMode = meNormal;
		// go back to normal mode.
		updateMouseCursor();
		break;
	case meMove:
		if (m_gridMode) {
			// implement this!
			std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
			double offset = getGridCtrlPointOffset(m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand(false, offset, m_parentWindow->targetRiverSurvey()->gridCreatingCondition()));
			m_mouseEventMode = meNormal;
		} else {
			GeoDataRiverCrosssection::AltitudeList newAltitudeList = m_oldAltitudeList;
			updateAltitudeList(newAltitudeList, m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GeoDataRiverSurvey::MouseEditCrosssectionCommand(m_parentWindow->target(), newAltitudeList, m_oldAltitudeList, m_parentWindow, m_parentWindow->targetRiverSurvey(), false));
			m_mouseEventMode = meNormal;
		}
		updateMouseCursor();
		break;

	case meAddVegetation:
		{
			// finish selecting.
			m_rubberBand->hide();
			m_mouseEventMode = meNormal;

			QMatrix invMatrix = m_matrix.inverted();
			QPointF mappedLeft = round2(invMatrix.map(QPointF(qMin(event->x(), m_rubberOrigin.x()), 0)));
			QPointF mappedRight = round2(invMatrix.map(QPointF(qMax(event->x(), m_rubberOrigin.x()), 0)));

			GeoDataRiverPathPointJmkData::Item item;
			auto target = m_parentWindow->target();
			item.distance = mappedLeft.x();
			item.width = mappedRight.x() - mappedLeft.x();
			item.height = 10;

			GeoDataRiverSurveyCrosssectionWindow::JmkDataEditDialog dialog(this);
			dialog.setItem(item);
			int ret = dialog.exec();
			if (ret == QDialog::Rejected) {return;}

			item = dialog.item();
			auto oldJmk = target->jmk();
			auto newJmk = oldJmk;

			newJmk.items().push_back(item);
			std::sort(newJmk.items().begin(), newJmk.items().end());

			iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditJmkDataCommand(newJmk, oldJmk, target, m_parentWindow));
		}
		break;

	case meDragVegetation:
		{
			auto target = m_parentWindow->target();
			auto oldJmk = target->jmk();
			auto newJmk = oldJmk;
			iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditJmkDataByDragCommand(false, m_dragJmkItemIndex, m_dragJmkRight, newJmk, oldJmk, target, m_parentWindow));
			m_mouseEventMode = meDragVegetationPrepare;
		}
		break;
	}
}


void GeoDataRiverSurveyCrosssectionWindowGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (m_mouseEventMode == meEditCrosssection && event->button() == Qt::LeftButton) {
		m_mouseEventMode = meNormal;
		m_oldLine.crosssection().AltitudeInfo().clear();
		updateMouseCursor();
		viewport()->update();
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::keyReleaseEvent(QKeyEvent* event)
{
	if (m_mouseEventMode == meEditCrosssection && (iRIC::isEnterKey(event->key()) || event->key() == Qt::Key_Escape)) {
		m_mouseEventMode = meNormal;
		m_oldLine.crosssection().AltitudeInfo().clear();
		updateMouseCursor();
		viewport()->update();
	}
	if (m_mouseEventMode == meAddVegetation && event->key() == Qt::Key_Escape) {
		m_mouseEventMode = meNormal;
		updateMouseCursor();
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::wheelEvent(QWheelEvent* event)
{
	if (event->orientation() == Qt::Horizontal) {return;}
	if (m_parentWindow->isRegionFixed()) {return;}

	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	if (numSteps > 0) {
		cameraZoomIn();
	} else {
		cameraZoomOut();
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::resizeEvent(QResizeEvent*)
{
	emit drawnRegionChanged();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::zoom(double scaleX, double scaleY)
{
	m_scaleX *= scaleX;
	m_scaleY *= scaleY;
	viewport()->update();

	emit drawnRegionChanged();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::translate(int x, int y)
{
	m_center.setX(m_center.x() - x / m_scaleX);
	m_center.setY(m_center.y() + y / m_scaleY);
	viewport()->update();

	emit drawnRegionChanged();
}

int GeoDataRiverSurveyCrosssectionWindowGraphicsView::moveWidth()
{
	QWidget* w = viewport();
	int stdW = qMax(w->width(), w->height());
	return stdW / 3;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::updateMouseCursor()
{
	if (m_viewMouseEventMode == vmeZooming) {
		setCursor(m_zoomCursor);
	} else if (m_viewMouseEventMode == vmeTranslating) {
		setCursor(m_moveCursor);
	} else if (m_mouseEventMode == meMove) {
		setCursor(Qt::ClosedHandCursor);
	} else if (m_mouseEventMode == meMovePrepare) {
		setCursor(Qt::OpenHandCursor);
	} else if (m_mouseEventMode == meEditCrosssection) {
		setCursor(Qt::CrossCursor);
	} else if (m_mouseEventMode == meAddVegetation) {
		setCursor(Qt::CrossCursor);
	} else if (m_mouseEventMode == meDragVegetationPrepare || m_mouseEventMode == meDragVegetation) {
		setCursor(Qt::SizeHorCursor);
	} else {
		setCursor(Qt::ArrowCursor);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::selectPoints(const QPoint& from, const QPoint& to)
{
	QPointF mins(qMin(from.x(), to.x()), qMax(from.y(), to.y()));
	QPointF maxs(qMax(from.x(), to.x()), qMin(from.y(), to.y()));

	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedMins = invMatrix.map(mins);
	QPointF mappedMaxs = invMatrix.map(maxs);

	if (m_gridMode) {
		GeoDataRiverPathPoint* blackpoint = m_parentWindow->gridCreatingConditionPoint();
		double leftShift = blackpoint->crosssection().leftShift();
		std::list<CtrlPointSelectionInfo>& sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
		sel.clear();
		CtrlPointSelectionInfo selInfo;
		selInfo.Point = blackpoint;

		// left side
		for (int i = 0; i < blackpoint->CenterToLeftCtrlPoints.count(); ++i) {
			double v = blackpoint->CenterToLeftCtrlPoints.at(i);
			GeoDataRiverCrosssection::Altitude alt = blackpoint->lXSec()->interpolate(v);
			if (alt.position() + leftShift >= mappedMins.x() &&
					alt.position() + leftShift <= mappedMaxs.x() &&
					alt.height() >= mappedMins.y() &&
					alt.height() <= mappedMaxs.y()) {
				selInfo.Position = GeoDataRiverPathPoint::pposCenterToLeft;
				selInfo.Index = i;
				sel.push_back(selInfo);
			}
		}
		// right side
		for (int i = 0; i < blackpoint->CenterToRightCtrlPoints.count(); ++i) {
			double v = blackpoint->CenterToRightCtrlPoints.at(i);
			GeoDataRiverCrosssection::Altitude alt = blackpoint->rXSec()->interpolate(v);
			if (alt.position() + leftShift >= mappedMins.x() &&
					alt.position() + leftShift <= mappedMaxs.x() &&
					alt.height() >= mappedMins.y() &&
					alt.height() <= mappedMaxs.y()) {
				selInfo.Position = GeoDataRiverPathPoint::pposCenterToRight;
				selInfo.Index = i;
				sel.push_back(selInfo);
			}
		}
		m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->updateShapeData();
		m_parentWindow->gridCreatingConditionRiverSurvey()->renderGraphicsView();
	} else {
		if (m_parentWindow->target() == nullptr) {return;}
		QItemSelection selection;

		GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
		double leftShift = cross.leftShift();
		GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
		int row = 0;
		QModelIndex firstIndex;
		bool firstset = false;
		for (auto it = alist.begin(); it != alist.end(); ++it) {
			GeoDataRiverCrosssection::Altitude alt = *it;
			if (alt.position() + leftShift >= mappedMins.x() &&
					alt.position() + leftShift <= mappedMaxs.x() &&
					alt.height() >= mappedMins.y() &&
					alt.height() <= mappedMaxs.y()) {
				selection.merge(QItemSelection(model()->index(row, 0), model()->index(row, 2)), QItemSelectionModel::Select);
				if (! firstset) {
					firstIndex = model()->index(row, 0);
					firstset = true;
				}
			}
			++row;
		}
		selectionModel()->clearSelection();
		selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
		if (firstset) {
			m_parentWindow->tableView()->scrollTo(firstIndex);
		}
	}
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::updateActionStatus()
{
	QModelIndexList rows = selectionModel()->selectedRows();
	m_activateAction->setEnabled(rows.count() > 0);
	m_inactivateAction->setEnabled(rows.count() > 0);
	if (rows.count() == 0) {
		m_moveAction->setDisabled(true);
	} else {
		bool continuous = true;
		auto it = rows.begin();
		auto it2 = it;
		++it2;
		while (it2 != rows.end()) {
			continuous = continuous && (it2->row() == it->row() + 1);
			++it; ++it2;
		}
		m_moveAction->setEnabled(continuous);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::informModelessDialogOpen()
{
	m_mouseEventMode = meNormal;
	m_modelessDialogIsOpen = true;
	updateMouseCursor();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::informModelessDialogClose()
{
	m_modelessDialogIsOpen = false;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::restoreMouseEventMode()
{
	m_mouseEventMode = MouseEventMode::meNormal;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::activateSelectedRows()
{
	if (m_parentWindow->target() == nullptr) {return;}
	QModelIndexList rows = selectionModel()->selectedRows();
	GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	before = alist;
	for (auto it = rows.begin(); it != rows.end(); ++it) {
		QModelIndex index = *it;
		alist[index.row()].setActive(true);
	}
	after = alist;
	auto odn = m_parentWindow->target()->odn();
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points"), m_parentWindow->target(), after, GeoDataRiverSurvey::EditCrosssectionCommand::NO_SEL, before, GeoDataRiverSurvey::EditCrosssectionCommand::NO_SEL, odn, m_parentWindow, m_parentWindow->targetRiverSurvey()));
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::inactivateSelectedRows()
{
	if (m_parentWindow->target() == nullptr) {return;}
	QModelIndexList rows = selectionModel()->selectedRows();
	GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	before = alist;
	std::vector<int> indices;
	for (auto it = rows.begin(); it != rows.end(); ++it) {
		QModelIndex index = *it;
		indices.push_back(index.row());
	}
	if (! m_parentWindow->canInactivateSelectedRows(cross, indices)) {
		alist = before;
		return;
	}
	after = alist;
	auto odn = m_parentWindow->target()->odn();
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points"), m_parentWindow->target(), after, GeoDataRiverSurvey::EditCrosssectionCommand::NO_SEL, before, GeoDataRiverSurvey::EditCrosssectionCommand::NO_SEL, odn, m_parentWindow, m_parentWindow->targetRiverSurvey()));
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::moveSelectedRows()
{
	if (m_parentWindow->target() == nullptr) {return;}
	QModelIndexList rows = selectionModel()->selectedRows();
	int from = rows.front().row();
	int to = rows.back().row();
	GeoDataRiverCrosssectionAltitudeMoveDialog dialog(m_parentWindow->target(), from, to, m_parentWindow->targetRiverSurvey(), m_parentWindow, this);
	dialog.exec();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::updateAltitudeList(GeoDataRiverCrosssection::AltitudeList& alist, const QPoint& start, const QPoint& end)
{
	QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
	QPointF startF(start);
	QPointF endF(end);
	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedStartF = invMatrix.map(startF);
	QPointF mappedEndF = invMatrix.map(endF);
	double xoffset = mappedEndF.x() - mappedStartF.x();
	if (m_dragLimit.minSet && xoffset < m_dragLimit.min) {
		xoffset = m_dragLimit.min;
	}
	if (m_dragLimit.maxSet && xoffset > m_dragLimit.max) {
		xoffset = m_dragLimit.max;
	}
	double yoffset = mappedEndF.y() - mappedStartF.y();
	for (auto it = selectedPoints.begin(); it != selectedPoints.end(); ++it) {
		int index = it->row();
		GeoDataRiverCrosssection::Altitude& alt = alist[index];
		alt.setPosition(alt.position() + xoffset);
		alt.setHeight(alt.height() + yoffset);
	}
}

double GeoDataRiverSurveyCrosssectionWindowGraphicsView::getGridCtrlPointOffset(const QPoint& start, const QPoint& end)
{
	QPointF startF(start);
	QPointF endF(end);
	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedStartF = invMatrix.map(startF);
	QPointF mappedEndF = invMatrix.map(endF);
	double xoffset = mappedEndF.x() - mappedStartF.x();
	std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
	CtrlPointSelectionInfo info = sel.front();
	if (info.Position == GeoDataRiverPathPoint::pposCenterToLeft) {
		xoffset /= (info.Point->lXSec()->interpolate(1).position());
	} else if (info.Position == GeoDataRiverPathPoint::pposCenterToRight) {
		xoffset /= (info.Point->rXSec()->interpolate(1).position());
	}
	if (xoffset < m_dragLimit.min) {
		xoffset = m_dragLimit.min;
	}
	if (xoffset > m_dragLimit.max) {
		xoffset = m_dragLimit.max;
	}
	return xoffset;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::inspectLimits(bool* minlimit, double* min, bool* maxlimit, double* max)
{
	QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
	GeoDataRiverCrosssection::AltitudeList& alist = m_parentWindow->target()->crosssection().AltitudeInfo();

	// First, specify that both min and max limit is set.
	*minlimit = true;
	*maxlimit = true;

	int left = selectedPoints.first().row() - 1;
	int right = selectedPoints.last().row() + 1;

	if (left == - 1) {
		left = 0;
		*minlimit = false;
	}
	if (right == static_cast<int>(alist.size())) {
		right = static_cast<int>(alist.size()) - 1;
		*maxlimit = false;
	}
	double min1, min2, max1, max2;
	min1 = alist[left].position() - alist[selectedPoints.first().row()].position();
	max1 = alist[right].position() - alist[selectedPoints.last().row()].position();
	min2 = 0 - alist.back().position();
	max2 = 0 - alist.front().position();
	if (*minlimit) {
		*max = max1;
		if (*maxlimit) {
			*min = min1;
		} else {
			*min = std::max(min1, min2);
		}
	} else {
		*min = min2;
		if (*maxlimit) {
			*max = std::min(max1, max2);
		} else {
			*max = max2;

			*minlimit = true;
			*maxlimit = true;
		}
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::inspectGridLimits(double* min, double* max)
{
	std::list<CtrlPointSelectionInfo>& list = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
	std::list<CtrlPointSelectionInfo> tmplist;
	for (auto it = list.begin(); it != list.end(); ++it) {
		if (it->Index < it->Point->CtrlPoints(it->Position).size()) {
			// valid point.
			tmplist.push_back(*it);
		}
	}
	list = tmplist;
	if (continuousGridSelection()) {
		CtrlPointSelectionInfo& infof = list.front();
		m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->GCPOffsetInfo().points = infof.Point->CtrlPoints(infof.Position);
		if (infof.Index == 0) {
			*min = 0 - infof.Point->CtrlPoints(infof.Position)[0];
		} else {
			*min = infof.Point->CtrlPoints(infof.Position)[infof.Index - 1]
						 - infof.Point->CtrlPoints(infof.Position)[infof.Index];
		}
		CtrlPointSelectionInfo& infob = list.back();
		if (infob.Index == infob.Point->CtrlPoints(infob.Position).size() - 1) {
			*max = 1 - infob.Point->CtrlPoints(infob.Position)[infob.Index];
		} else {
			*max = infob.Point->CtrlPoints(infob.Position)[infob.Index + 1]
						 - infob.Point->CtrlPoints(infob.Position)[infob.Index];
		}
	}
}

bool GeoDataRiverSurveyCrosssectionWindowGraphicsView::continuousSelection()
{
	QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
	int current = 1;
	while (current < selectedPoints.count()) {
		if ((selectedPoints[current].row() - selectedPoints[current - 1].row()) != 1) {
			return false;
		}
		++current;
	}
	return true;
}

bool GeoDataRiverSurveyCrosssectionWindowGraphicsView::continuousGridSelection()
{
	std::list<CtrlPointSelectionInfo>& list = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
	if (list.size() == 0) {return false;}
	else if (list.size() == 1) {return true;}
	else {
		bool ok = true;
		auto it = list.begin();
		CtrlPointSelectionInfo tmpinfo = *it++;
		while (it != list.end()) {
			ok = ok && (tmpinfo.Point == it->Point && tmpinfo.Index + 1 == it->Index);
			tmpinfo = *it++;
		}
		return ok;
	}
}

std::vector<double> GeoDataRiverSurveyCrosssectionWindowGraphicsView::loadSlopeRatios()
{
	QSettings settings;
	auto ratios = settings.value("riversurvey/slopeangles", QString("0.5,1,2,3,4,5,10,20")).toString().split(",");
	std::vector<double> ret;
	for (auto r : ratios) {
		ret.push_back(r.toDouble());
	}
	return ret;
}

struct CreateAltitudeInfo
{
	GeoDataRiverCrosssection::Altitude altitude;
	QString ratio;
};

GeoDataRiverCrosssection::Altitude GeoDataRiverSurveyCrosssectionWindowGraphicsView::createAltitude(const QPoint& pos, QString* ratio, bool freeSlope)
{
	const auto index = selectionModel()->selectedIndexes().at(0);
	auto& xsec = m_parentWindow->target()->crosssection();
	const auto& alist = xsec.AltitudeInfo();
	const auto& selectedAlt = alist.at(index.row());

	QMatrix invMatrix = m_matrix.inverted();
	QPointF posF(pos.x(), pos.y());
	auto mappedPos = invMatrix.map(posF);
	mappedPos.setX(mappedPos.x() - xsec.leftShift());

	double distX = mappedPos.x() - selectedAlt.position();
	double distY = mappedPos.y() - selectedAlt.height();
	double sign = 1;
	if (distX * distY < 0) {sign = -1;}

	if (freeSlope) {
		double r = std::abs((mappedPos.y() - selectedAlt.height()) / (mappedPos.x() - selectedAlt.position()));
		if (r == 0) {
			*ratio = "";
		} else {
			*ratio = QString("1:%1").arg(QString::number(1.0 / r, 'f', 2));
		}
		return GeoDataRiverCrosssection::Altitude(mappedPos.x(), mappedPos.y());
	} else {
		std::map<double, CreateAltitudeInfo> diffs;
		GeoDataRiverCrosssection::Altitude a(mappedPos.x(), selectedAlt.height());
		CreateAltitudeInfo ainfo{a, ""};
		diffs.insert({std::abs(distY), ainfo});
		for (double r : loadSlopeRatios()) {
			double dy = distX * sign / r;
			GeoDataRiverCrosssection::Altitude a(mappedPos.x(), selectedAlt.height() + dy);
			CreateAltitudeInfo ainfo{a, QString::number(r)};
			double diff = mappedPos.y() - (a.height());
			diffs.insert({std::abs(diff), ainfo});
		}
		// get the CreateAltitudeInfo with the smallest diff
		auto pair = diffs.begin();
		if  (ratio != nullptr) {
			if (pair->second.ratio == "") {
				*ratio = "";
			} else {
				*ratio = QString("1:%1").arg(pair->second.ratio);
			}
		}
		return pair->second.altitude;
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::editCrossSection(GeoDataRiverCrosssection::Altitude& alt)
{
	auto before = m_parentWindow->target()->crosssection().AltitudeInfo();
	auto after = before;
	const auto index = selectionModel()->selectedIndexes().at(0);
	const auto& selectedAlt = after.at(index.row());
	auto eraseBegin = after.begin();
	auto eraseEnd = eraseBegin;
	int newIndex = index.row();
	if (alt.position() > selectedAlt.position()) {
		eraseBegin = after.begin() + index.row() + 1;
		eraseEnd = eraseBegin;
		while (eraseEnd != after.end() && eraseEnd->position() <= alt.position()) {
			++ eraseEnd;
		}
		newIndex = newIndex + 1;
	} else {
		eraseEnd = after.begin() + index.row();
		eraseBegin = eraseEnd;
		while (eraseBegin != after.begin() && eraseBegin->position() > alt.position()) {
			-- eraseBegin;
		}
		newIndex = newIndex - (eraseEnd - eraseBegin);
	}
	after.erase(eraseBegin, eraseEnd);
	after.push_back(alt);
	std::sort(after.begin(), after.end());
	auto odn = m_parentWindow->target()->odn();

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Edit Cross Section"), m_parentWindow->target(), after, newIndex, before, index.row(), odn, m_parentWindow, m_parentWindow->targetRiverSurvey()));
	QItemSelection sel(model()->index(newIndex, 0), model()->index(newIndex, 2));
	auto selModel = selectionModel();
	selModel->clearSelection();
	selModel->select(sel, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

double GeoDataRiverSurveyCrosssectionWindowGraphicsView::aspectRatio() const
{
	return m_scaleY / m_scaleX;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setAspectRatio(double ratio)
{
	if (ratio == 0) {ratio = 1;}

	double currentRatio = aspectRatio();
	double rate = currentRatio / ratio;
	if (rate > 0.999 && rate < 1.001) {return;}

	zoom(1.0, ratio / currentRatio);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::toggleGridCreatingMode(bool gridMode)
{
	m_gridMode = gridMode;
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::enterEditCrosssectionMode()
{
	InformationDialog::information(this, tr("Information"), tr("Edit the cross section by mouse-clicking. Finish editing by double clicking, or pressing return key.\nYou can precisely edit the cross section by inputting values from dialog. Please enter dialog edit mode from \"Edit from Dialog\" in the right-clicking menu."), "riversurveyedit");
	m_mouseEventMode = meEditCrosssection;

	const auto index = selectionModel()->selectedIndexes().at(0);
	auto& xsec = m_parentWindow->target()->crosssection();
	const auto& alist = xsec.AltitudeInfo();
	const auto& selectedAlt = alist.at(index.row());
	m_editAltitudePreview = selectedAlt;
	m_oldLine.crosssection().AltitudeInfo() = alist;
	m_oldLine.crosssection().setLeftShift(xsec.leftShift());

	updateMouseCursor();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::enterSlopePointEditMode(GeoDataRiverSurveyCrosssectionSlopePointEditDialog::Mode mode)
{
	auto dialog = new GeoDataRiverSurveyCrosssectionSlopePointEditDialog(m_parentWindow);
	dialog->setMode(mode);
	dialog->setSlope(m_slopePointEditModeSlope);
	dialog->show();

	m_mouseEventMode = MouseEventMode::meEditWithSlopePoint;
	m_slopePointEditMode = mode;

	auto mainWindow = m_parentWindow->groupDataItem()->iricMainWindow();
	mainWindow->enterModelessDialogMode();

	connect(this, &GeoDataRiverSurveyCrosssectionWindowGraphicsView::positionClicked, dialog, &GeoDataRiverSurveyCrosssectionSlopePointEditDialog::setPoint);
	connect(dialog, &QDialog::destroyed, this, &GeoDataRiverSurveyCrosssectionWindowGraphicsView::restoreMouseEventMode);
	connect(dialog, &QDialog::destroyed, mainWindow, &iRICMainWindowI::exitModelessDialogMode);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::editDisplaySetting()
{
	QWidget* w = viewport();
	auto matrix = getMatrix(w->rect());
	QMatrix invMatrix = matrix.inverted();
	QPointF mins, maxs;
	mins = invMatrix.map(QPointF(0, w->height()));
	maxs = invMatrix.map(QPointF(w->width(), 0));

	if (m_displaySetting.bgHScaleAuto) {
		calcAutoScale(maxs.x() - mins.x(), &m_displaySetting.bgHScaleInterval, &m_displaySetting.bgHSubScaleInterval);
	}
	if (m_displaySetting.bgVScaleAuto) {
		calcAutoScale(maxs.y() - mins.y(), &m_displaySetting.bgVScaleInterval, &m_displaySetting.bgVSubScaleInterval);
	}
	GeoDataRiverSurveyCrossSectionDisplaySettingDialog dialog(this);
	dialog.setSetting(m_displaySetting);

	int ret = dialog.exec();
	if (ret != QDialog::Accepted) {return;}

	iRICUndoStack::instance().push(new SetDisplaySettingCommand(dialog.setting(), this));
}
