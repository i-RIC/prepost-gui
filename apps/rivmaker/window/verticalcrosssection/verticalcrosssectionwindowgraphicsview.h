#ifndef VERTICALCROSSSECTIONWINDOWGRAPHICSVIEW_H
#define VERTICALCROSSSECTIONWINDOWGRAPHICSVIEW_H

#include "../../widgets/chartgraphicsview.h"

class VerticalCrossSectionWindow;

class VerticalCrossSectionWindowGraphicsView : public ChartGraphicsView
{
public:
	enum MouseEventMode {
		meNormal,
		meMovePrepare,
		meMove
	};

	VerticalCrossSectionWindowGraphicsView(QWidget* parent);

	void paintEvent(QPaintEvent *event) override;
	void draw(QPainter* painter);
	void updateView();

private:
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void drawArbitrary(QPainter* painter, const QMatrix& matrix);
	void drawBenchmark(QPainter* painter, const QMatrix& matrix);
	void drawReferenceMark(QPainter* painter, const QMatrix& matrix);
	void drawHub(QPainter* painter, const QMatrix& matrix);
	void drawLeftHWMs(QPainter* painter, const QMatrix& matrix);
	void drawRightHWMs(QPainter* painter, const QMatrix& matrix);
	void drawStreamGage(QPainter* painter, const QMatrix& matrix);
	void drawLeftHWMLine(QPainter* painter, const QMatrix& matrix);
	void drawRightHWMLine(QPainter* painter, const QMatrix& matrix);
	void drawCrossSectionLines(QPainter* painter, const QMatrix& matrix);
	void drawElevationLine(QPainter* painter, const QMatrix& matrix);

	VerticalCrossSectionWindow* chartWindow() const;
	QRectF getRegion() override;

	void updateMouseEventMode(QMouseEvent* event);
	void updateMouseCursor();

	MouseEventMode m_mouseEventMode;
	int m_editTargetCrossSection;

	bool m_drawn;
};

#endif // VERTICALCROSSSECTIONWINDOWGRAPHICSVIEW_H
