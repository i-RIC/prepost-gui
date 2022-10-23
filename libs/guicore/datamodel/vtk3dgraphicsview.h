#ifndef VTK3DGRAPHICSVIEW_H
#define VTK3DGRAPHICSVIEW_H

#include "../guicore_global.h"

#include "vtkgraphicsview.h"

class vtkLightKit;

class GUICOREDLL_EXPORT VTK3DGraphicsView : public VTKGraphicsView
{
	Q_OBJECT

public:
	VTK3DGraphicsView(QWidget* parent);
	~VTK3DGraphicsView() override;

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
	/// Set projection to parallel
	void cameraParallelProjection();
	/// Set projection to perspective
	void cameraPerspectiveProjection();
	/// Update menu
	void updateProjectionMenu(QAction* parallel, QAction* perspective);

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
	/// Set projection to parallel
	void parallelProjection();
	/// Set projection to perspective
	void perspectiveProjection();

	void getFocalPointAndDistance(double focal[3], double* distance);

	vtkLightKit* m_lightKit;
};

#endif // VTK3DGRAPHICSVIEW_H
