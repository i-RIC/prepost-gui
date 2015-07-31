#ifndef VTK3DGRAPHICSVIEW_H
#define VTK3DGRAPHICSVIEW_H

#include "../guicore_global.h"

#include "vtkgraphicsview.h"

class GUICOREDLL_EXPORT VTK3DGraphicsView : public VTKGraphicsView
{
	Q_OBJECT

public:
	VTK3DGraphicsView(QWidget* parent);
	virtual ~VTK3DGraphicsView() {}
	/// Get distance in world corrdinate
	double stdDistance(int pixels);
	/// Translate the camera
	void translate(int x, int y) override;

public slots:
	/// Reset the rotation
	void cameraResetRotation();
	/// Rotate 90 degree
	void cameraRotate90();
	/// Go to XY plane
	void cameraToXYPlane();
	/// Go to YZ plane
	void cameraToYZPlane();
	/// Go to ZX plane
	void cameraToZXPlane();

private:
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

	void getFocalPointAndDistance(double focal[3], double* distance);
};

#endif // VTK3DGRAPHICSVIEW_H
