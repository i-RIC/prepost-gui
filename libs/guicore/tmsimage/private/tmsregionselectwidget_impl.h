#ifndef TMSREGIONSELECTWIDGET_IMPL_H
#define TMSREGIONSELECTWIDGET_IMPL_H

#include "../tmsregionselectwidget.h"
#include <tmsloader/tmsloader.h>

#include <QCursor>
#include <QImage>
#include <QPixmap>
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
	void lonLatToXY(const QSize& size, double lon, double lat, double* x, double* y) const;
	void XYToLonLat(const QSize& size, double x, double y, double* lon, double* lat) const;

	double m_centerLon;
	double m_centerLat;
	double m_zoomLevel;

	bool m_isSelected;

	QPointF m_pointLonLat1;
	QPointF m_pointLonLat2;

	std::string m_mapSetting;
	int m_requestId;
	tmsloader::TmsLoader m_loader;

	QImage m_image;
	double m_imageLonMin;
	double m_imageLonMax;
	double m_imageLatMin;
	double m_imageLatMax;

	QPoint m_previousPos;
	ViewOperationState m_viewOperationState;
	QTimer m_timer;

	QPixmap m_movePixmap;
	QPixmap m_zoomPixmap;

	QCursor m_moveCursor;
	QCursor m_zoomCursor;

private:
	TmsRegionSelectWidget* m_widget;
};

#endif // TMSREGIONSELECTWIDGET_IMPL_H
