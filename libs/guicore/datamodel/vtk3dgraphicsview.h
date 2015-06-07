#ifndef VTK3DGRAPHICSVIEW_H
#define VTK3DGRAPHICSVIEW_H

#include "../guicore_global.h"

#include "vtkgraphicsview.h"

class GUICOREDLL_EXPORT VTK3DGraphicsView : public VTKGraphicsView
{

public:
	VTK3DGraphicsView(QWidget* parent);
	virtual ~VTK3DGraphicsView() {}
	/// Fit camera.
	virtual void fitInView() override;
	/// Rotate camera.
	void rotate(double r);
	/// Reset rotating the camera.
	void resetRoll();
	/// Go to XY plane
	void toXYPlane();
	/// Go to YZ plane
	void toYZPlane();
	/// Go to ZX plane
	void toZXPlane();
	/// Get distance in world corrdinate
	double stdDistance(int pixels);
	/// Translate the camera
	void translate(int x, int y) override;

private:
	void getFocalPointAndDistance(double focal[3], double* distance);
};

#endif // VTK3DGRAPHICSVIEW_H
