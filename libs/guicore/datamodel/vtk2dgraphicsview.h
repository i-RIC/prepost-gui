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
	virtual void fitInView();
	void rotate(double r);
	void resetCamera();
	void resetRoll();
	void viewportToWorld(double& x, double& y);
	void worldToViewport(double& x, double& y);
	void getDataRegion(double* xmin, double* xmax, double* ymin, double* ymax);
	void getDrawnRegion(double* xmin, double* xmax, double* ymin, double* ymax);
	void mouseMoveEvent(QMouseEvent* event);
	void ResetCameraClippingRange();
	double stdRadius(int pixels);
	void translate(int x, int y);
	void emitPosition(int x, int y);
	void emitWorldPosition(int x, int y);

signals:
	void worldPositionChangedForStatusBar(const QVector2D& pos);
	void positionChanged(int x, int y);
	void worldPositionChanged(const QVector2D& v);
};

#endif // VTK2DGRAPHICSVIEW_H
