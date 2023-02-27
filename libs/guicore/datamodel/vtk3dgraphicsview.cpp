#define _USE_MATH_DEFINES
#include <cmath>

#include "vtk3dgraphicsview.h"

#include "graphicswindowsimpledatamodel.h"

#include <vtkCamera.h>
#include <vtkLightKit.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <QAction>
#include <QVector3D>

VTK3DGraphicsView::VTK3DGraphicsView(QWidget* parent) :
	VTKGraphicsView(parent)
{
	mainRenderer()->RemoveAllLights();

	m_lightKit = vtkLightKit::New();
	m_lightKit->AddLightsToRenderer(mainRenderer());
}

VTK3DGraphicsView::~VTK3DGraphicsView()
{
	m_lightKit->RemoveLightsFromRenderer(mainRenderer());
	m_lightKit->Delete();
}

void VTK3DGraphicsView::fitInView()
{
	mainRenderer()->ResetCamera();
	update2Ds();
}

void VTK3DGraphicsView::rotate(double r)
{
	mainRenderer()->GetActiveCamera()->Roll(r);
}

void VTK3DGraphicsView::resetRoll()
{
	double angle = mainRenderer()->GetActiveCamera()->GetRoll();
	mainRenderer()->GetActiveCamera()->Roll(-angle);
}

void VTK3DGraphicsView::toXYPlane()
{
	double focal[3];
	double dist;
	getFocalPointAndDistance(focal, &dist);

	double newpos[3];
	newpos[0] = focal[0];
	newpos[1] = focal[1];
	newpos[2] = focal[2] + dist;

	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	camera->SetPosition(newpos);
	camera->SetViewUp(0, 1, 0);
}

void VTK3DGraphicsView::toYZPlane()
{
	double focal[3];
	double dist;
	getFocalPointAndDistance(focal, &dist);

	double newpos[3];
	newpos[0] = focal[0] + dist;
	newpos[1] = focal[1];
	newpos[2] = focal[2];

	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	camera->SetPosition(newpos);
	camera->SetViewUp(0, 0, 1);
}

void VTK3DGraphicsView::toZXPlane()
{
	double focal[3];
	double dist;
	getFocalPointAndDistance(focal, &dist);

	double newpos[3];
	newpos[0] = focal[0];
	newpos[1] = focal[1] - dist;
	newpos[2] = focal[2];

	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	camera->SetPosition(newpos);
	camera->SetViewUp(0, 0, 1);
}

void VTK3DGraphicsView::getFocalPointAndDistance(double focal[3], double* distance)
{
	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	double pos[3];
	camera->GetFocalPoint(focal);
	camera->GetPosition(pos);
	QVector3D focalVec(focal[0], focal[1], focal[2]);
	QVector3D posVec(pos[0], pos[1], pos[2]);
	*distance = (focalVec - posVec).length();
}

void VTK3DGraphicsView::translate(int x, int y)
{
	double position[3];
	double focalpoint[3];
	double viewup[3];
	double s = mainRenderer()->GetActiveCamera()->GetParallelScale();
	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	camera->GetPosition(position);
	camera->GetFocalPoint(focalpoint);
	camera->GetViewUp(viewup);

	QVector3D viewupV(viewup[0], viewup[1], viewup[2]);
	QVector3D frontV(focalpoint[0] - position[0], focalpoint[1] - position[1], focalpoint[2] - position[2]);

	viewupV.normalize();
	frontV.normalize();

	QVector3D leftToRightV = QVector3D::crossProduct(frontV, viewupV);
	leftToRightV.normalize();

	QVector3D deltaV = leftToRightV * s * x + viewupV * s * y;

	position[0] += deltaV.x();
	position[1] += deltaV.y();
	position[2] += deltaV.z();
	focalpoint[0] += deltaV.x();
	focalpoint[1] += deltaV.y();
	focalpoint[2] += deltaV.z();

	camera->SetPosition(position);
	camera->SetFocalPoint(focalpoint);
}

void VTK3DGraphicsView::cameraResetRotation()
{
	resetRoll();
	model()->viewOperationEndedGlobal();
	render();
}

void VTK3DGraphicsView::cameraRotate90()
{
	rotate(90);
	model()->viewOperationEndedGlobal();
	render();
}

void VTK3DGraphicsView::cameraToXYPlane()
{
	toXYPlane();
	model()->viewOperationEndedGlobal();
	render();
}

void VTK3DGraphicsView::cameraToYZPlane()
{
	toYZPlane();
	model()->viewOperationEndedGlobal();
	render();
}

void VTK3DGraphicsView::cameraToZXPlane()
{
	toZXPlane();
	model()->viewOperationEndedGlobal();
	render();
}

void VTK3DGraphicsView::updateProjectionMenu(QAction* parallel, QAction* perspective)
{
	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	if (camera->GetParallelProjection()) {
		parallel->setChecked(true);
	} else {
		perspective->setChecked(true);
	}
}

void VTK3DGraphicsView::parallelProjection()
{
	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	camera->ParallelProjectionOn();
}

void VTK3DGraphicsView::perspectiveProjection()
{
	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	camera->ParallelProjectionOff();
}

void VTK3DGraphicsView::cameraParallelProjection()
{
	parallelProjection();
	model()->viewOperationEndedGlobal();
	render();
}

void VTK3DGraphicsView::cameraPerspectiveProjection()
{
	perspectiveProjection();
	model()->viewOperationEndedGlobal();
	render();
}
