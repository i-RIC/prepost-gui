#ifndef VTK2DGRAPHICSVIEW_H
#define VTK2DGRAPHICSVIEW_H

#include "../guicore_global.h"
#include "vtkgraphicsview.h"

class QVector2D;

class GUICOREDLL_EXPORT VTK2DGraphicsView : public VTKGraphicsView
{
	Q_OBJECT

public:
	VTK2DGraphicsView(QWidget* parent);
	virtual ~VTK2DGraphicsView() {}
	void resetCamera();
	virtual void viewportToWorld(double& x, double& y) const;
	QPointF viewportToWorld(const QPoint p) const;
	virtual void worldToViewport(double& x, double& y) const;
	QPointF worldToViewport(QPointF p) const;
	void getDataRegion(double* xmin, double* xmax, double* ymin, double* ymax);
	void getDrawnRegion(double* xmin, double* xmax, double* ymin, double* ymax);
	void mouseMoveEvent(QMouseEvent* event) override;
	void ResetCameraClippingRange();
	double stdRadius(int pixels) const;
	void translate(int x, int y) override;
	void panTo(double x, double y);
	void emitPosition(int x, int y);
	void emitWorldPosition(int x, int y);

public slots:
	void cameraResetRotation();
	void cameraRotate90();

signals:
	void worldPositionChangedForStatusBar(const QVector2D& pos);
	void positionChanged(int x, int y);
	void worldPositionChanged(const QVector2D& v);

private:
	virtual void fitInView() override;
	void rotate(double r);
	void resetRoll();

};

#endif // VTK2DGRAPHICSVIEW_H
