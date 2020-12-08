#ifndef CHARTGRAPHICSVIEW_H
#define CHARTGRAPHICSVIEW_H

#include "chartgraphicsviewdisplaysetting.h"

#include <QWidget>

class ChartWindow;

class QPainter;

class ChartGraphicsView : public QWidget
{
	Q_OBJECT

public:
	enum ViewMouseEventMode {
		vmeNormal,
		vmeZooming,
		vmeTranslating
	};

	ChartGraphicsView(QWidget* w);

	void paintEvent(QPaintEvent* event) override;
	QSize sizeHint() const override;

	double aspectRatio() const;
	void setAspectRatio(double ratio);

	const ChartGraphicsViewDisplaySetting& chartDisplaySetting() const;
	void setChartDisplaySetting(const ChartGraphicsViewDisplaySetting& setting);

signals:
	void positionChangedForStatusBar(const QPointF& point);

public slots:
	void cameraFit();
	void cameraZoomIn();
	void cameraZoomOut();
	void cameraZoomInX();
	void cameraZoomOutX();
	void cameraZoomInY();
	void cameraZoomOutY();

protected:
	void mouseMoveEventViewChangeHandler(QMouseEvent* event);
	void mousePressEventViewChangeHandler(QMouseEvent* event);
	void mouseReleaseEventViewChangeHandler(QMouseEvent* event);
	void wheelEventViewChangeHandler(QWheelEvent* event);

	void emitPositionChanged(QMouseEvent* event);

	void paintBackground(QPainter* painter);
	void drawScales(QPainter* painter, const QMatrix& matrix);
	void drawAspectRatio(QPainter* painter);

	ChartWindow* chartWindow() const;
	QMatrix getMatrix(const QRect& viewport);
	ViewMouseEventMode viewMouseEventMode() const;

private:
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	void zoom(double scaleX, double scaleY);
	void translate(int x, int y);

	void updateMouseCursorForViewChange();

	virtual QRectF getRegion();

	QPixmap m_zoomPixmap;
	QPixmap m_movePixmap;

	QCursor m_zoomCursor;
	QCursor m_moveCursor;

	QPointF m_center;
	double m_scaleX;
	double m_scaleY;

	ViewMouseEventMode m_viewMouseEventMode;

	QPoint m_oldPosition;
	QPoint m_dragStartPoint;
	QMatrix m_matrix;

	ChartGraphicsViewDisplaySetting m_chartDisplaySetting;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_H
