#ifndef PRIMITIVEVIEW_H
#define PRIMITIVEVIEW_H

#include "misc_global.h"

#include <QTransform>
#include <QWidget>

namespace Ui
{
	class PrimitiveView;
}

class MISCDLL_EXPORT PrimitiveView : public QWidget
{
	Q_OBJECT
public:
	explicit PrimitiveView(QWidget *parent = 0);
	PrimitiveView::PrimitiveView(QWidget *parent, bool reverseY);
	~PrimitiveView();

	void updateView();

	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	void viewMouseMoveEvent(QMouseEvent* event);
	void viewMousePressEvent(QMouseEvent* event);
	void viewMouseReleaseEvent(QMouseEvent* event);
	void viewWheelEvent(QWheelEvent* event);

	void paintEvent(QPaintEvent*) override;
	void paintEvent(QWidget* widget, const QPixmap& pixmap, const QColor& backgroundColor, const QTransform& transform) const;

	QPointF conv(const QPointF& p) const;
	QPointF rconv(const QPointF& p) const;

	QRectF drawnArea() const;
	QRectF drawnArea(const QWidget& widget, const QTransform& transform, const QPointF& drawOffset) const;

signals:
	void drawnAreaChanged();

public slots:

protected:
	void setCenterPoint(const QPointF& centerPoint);

	double scale();
	void setScale(double scale); 

	void setRelativeScaleX(double scale);
	void setRelativeScaleY(double scale);

	double angleDegree() const;
	void setAngleDegree(double angle);

	const QTransform& transform();
	const QPoint& rightClickPos();
	void updateTransform();

private:
	virtual QPixmap drawModel();
	bool inViewOperation() const;

	QTransform m_transform;
	/// @name View related members
	//@{
	QPointF m_centerPoint;
	double m_scale;
	double m_relativeScaleX;
	double m_relativeScaleY;
	double m_angleDegree;
	double m_reverseYCoeff;
	bool m_zooming;
	bool m_translating;
	bool m_rotating;
	QPoint m_previousPos;
	QPointF m_drawOffset; // Currently not used (value naver changed).
	//@}

	/// Mouse position of the right click (buttun down) event
	QPoint m_rightClickPos;

	/// @name View operation related resources
	//@{
	QCursor m_zoomCursor;
	QCursor m_moveCursor;
	QCursor m_rotateCursor;
	//@}

	/// Cache related resources
	//@{
	QPixmap m_cachePixmap;
	QTransform m_cachedInvertedTransform;
	//@}

	Ui::PrimitiveView* ui;
};

#endif // PRIMITIVEVIEW_H
