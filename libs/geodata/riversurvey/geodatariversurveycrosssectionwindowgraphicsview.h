#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_H

#include "geodatarivercrosssection.h"

#include <QAbstractItemView>

class QPainter;
class QMenu;
class QAction;
class GeoDataRiverPathPoint;
class GeoDataRiverSurveyCrosssectionWindow;
class QRubberBand;

class GeoDataRiverSurveyCrosssectionWindowGraphicsView : public QAbstractItemView
{
	Q_OBJECT

public:
	enum MouseEventMode {
		meNormal,
		meSelecting,
		meZooming,
		meTranslating,
		meMove,
		meMovePrepare
	};
	GeoDataRiverSurveyCrosssectionWindowGraphicsView(QWidget* w);

	void setParentWindow(GeoDataRiverSurveyCrosssectionWindow* w);
	void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void paintEvent(QPaintEvent* event) override;
	QRect visualRect(const QModelIndex&) const override;
	void scrollTo(const QModelIndex& index, ScrollHint hint) override;
	QModelIndex indexAt(const QPoint&) const override;
	QModelIndex moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers) override;
	int horizontalOffset() const override;
	int verticalOffset() const override;
	bool isIndexHidden(const QModelIndex&) const override;
	void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) override;
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
	QRegion visualRegionForSelection(const QItemSelection& selection) const override;

	QAction* activateAction() const;
	QAction* inactivateAction() const;
	QAction* moveAction() const;

	void cameraFit();
	void cameraMoveLeft();
	void cameraMoveRight();
	void cameraMoveUp();
	void cameraMoveDown();
	void cameraZoomIn();
	void cameraZoomOut();
	void cameraZoomInX();
	void cameraZoomOutX();
	void cameraZoomInY();
	void cameraZoomOutY();
	void toggleGridCreatingMode(bool gridMode);

private slots:
	void activateSelectedRows();
	void inactivateSelectedRows();
	void moveSelectedRows();

protected:
	int moveWidth();
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	void setupActions();
	void updateAltitudeList(GeoDataRiverCrosssection::AltitudeList& alist, const QPoint& start, const QPoint& end);
	double getGridCtrlPointOffset(const QPoint& start, const QPoint& end);
	void setupMenu();
	void updateActionStatus();
	void drawCircle(QPainter& painter);
	void drawSelectionCircle(QPainter& painter);
	void drawSquare(QPainter& painter);
	void drawSelectionSquare(QPainter& painter);
	void drawLine(GeoDataRiverPathPoint* point, const QColor& color, QPainter& painter);
	void drawWaterSurfaceElevations(QPainter& painter, const QMatrix& matrix);
	void drawWaterSurfaceElevation(int index, QPainter& painter, const QMatrix& matrix);
	void drawCrossPoint(const QPointF& origin, const QVector2D& direction, const QPointF& left, const QPointF& right, const QPointF& q1, const QPointF& q2, const QString& name, const QColor& color, std::vector<std::vector<QRectF> >* drawnRects, QPainter& painter);
	void drawPolyLineCrossPoints(QPainter& painter);
	void zoom(double scaleX, double scaleY);
	void selectPoints(const QPoint& from, const QPoint& to);
	void translate(int x, int y);
	void updateMouseCursor();
	void inspectLimits(bool* minlimit, double* min, bool* maxlimit, double* max);
	void inspectGridLimits(double* min, double* max);
	bool continuousSelection();
	bool continuousGridSelection();

	const static int iLeftMargin = 20;
	const static int iRightMargin = 20;
	const static int iTopMargin = 20;
	const static int iBottomMargin = 20;

	const static int ellipseR = 2;
	const static int selectedEllipseR = 4;
	const static int squareR = 2;
	const static int selectedSquareR = 4;
	const static int fontRectWidth = 250;
	const static int fontRectHeight = 14;
	const static int bankHOffset = 10;
	const static int bankVOffset = 30;

	double fLeftMargin;
	double fRightMargin;
	double fTopMargin;
	double fBottomMargin;

	QPixmap m_zoomPixmap;
	QPixmap m_movePixmap;

	QCursor m_zoomCursor;
	QCursor m_moveCursor;

	QPointF getPointF(double x, double y, const QRect& viewport);
	QRectF getRegion();
	QMatrix getMatrix(QRect& viewport);
	void drawScales(QPainter& painter, const QMatrix& matrix);

	QMenu* m_rightClickingMenu;
	QAction* m_activateAction;
	QAction* m_inactivateAction;
	QAction* m_moveAction;

	GeoDataRiverCrosssection::AltitudeList m_oldAltitudeList;
	struct {
		bool minSet;
		double min;
		bool maxSet;
		double max;
	} m_dragLimit;
	GeoDataRiverSurveyCrosssectionWindow* m_parentWindow;
	QRubberBand* m_rubberBand;
	QRectF m_drawnRegion;
	MouseEventMode m_mouseEventMode;
	QRectF m_shownRegion;
	QPoint m_oldPosition;
	QPoint m_rubberOrigin;
	QPoint m_dragStartPoint;
	QMatrix m_matrix;
	bool m_gridMode;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_H
