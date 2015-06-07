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
	virtual void fitInView() override;
	void rotate(double r);
	void resetCamera();
	void resetRoll();
	void viewportToWorld(double& x, double& y) const;
	void worldToViewport(double& x, double& y) const;
	void getDataRegion(double* xmin, double* xmax, double* ymin, double* ymax);
	void getDrawnRegion(double* xmin, double* xmax, double* ymin, double* ymax);
	void mouseMoveEvent(QMouseEvent* event) override;
	void ResetCameraClippingRange();
	double stdRadius(int pixels) const;
	void translate(int x, int y) override;
	void emitPosition(int x, int y);
	void emitWorldPosition(int x, int y);

signals:
	void worldPositionChangedForStatusBar(const QVector2D& pos);
	void positionChanged(int x, int y);
	void worldPositionChanged(const QVector2D& v);
};

#endif // VTK2DGRAPHICSVIEW_H
