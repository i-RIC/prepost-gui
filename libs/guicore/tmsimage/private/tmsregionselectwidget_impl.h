#ifndef TMSREGIONSELECTWIDGET_IMPL_H
#define TMSREGIONSELECTWIDGET_IMPL_H

#include "../tmsregionselectwidget.h"
#include <tmsloader/tmsloader.h>

#include <QImage>
#include <QPoint>
#include <QPointF>
#include <QTimer>

#include <string>

class QRectF;

class TmsRegionSelectWidget::Impl
{
public:
	enum class ViewOperationState {
		None,
		Zooming,
		Translating
	};

	Impl(TmsRegionSelectWidget* w);
	~Impl();

	QRectF imageRect() const;
	QRectF regionRect() const;
	void posToXY(const QPoint& pos, const QSize& size, double* x, double* y) const;
	void XYToPos(double x, double y, const QSize& size, QPointF* pos) const;

	double m_centerX;
	double m_centerY;
	double m_scale;

	bool m_isSelected;

	QPointF m_point1;
	QPointF m_point2;

	CoordinateSystem* m_coordinateSystem;

	std::string m_mapSetting;
	int m_requestId;
	tmsloader::TmsLoader m_loader;
	QImage m_image;
	QPointF m_imageTopLeft;
	QPointF m_imageBottomRight;

	QPoint m_previousPos;
	ViewOperationState m_viewOperationState;
	QTimer m_timer;

private:
	TmsRegionSelectWidget* m_widget;
};

#endif // TMSREGIONSELECTWIDGET_IMPL_H
