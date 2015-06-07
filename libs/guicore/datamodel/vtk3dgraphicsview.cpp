#define _USE_MATH_DEFINES
#include <cmath>

#include "vtk3dgraphicsview.h"

#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <QVector3D>

VTK3DGraphicsView::VTK3DGraphicsView(QWidget* parent)
	: VTKGraphicsView(parent)
{

}

void VTK3DGraphicsView::fitInView()
{
	m_mainRenderer->ResetCamera();
	update2Ds();
	m_mainRenderer->GetRenderWindow()->Render();
}

void VTK3DGraphicsView::rotate(double r)
{
	m_mainRenderer->GetActiveCamera()->Roll(r);
	m_mainRenderer->GetRenderWindow()->Render();
}

void VTK3DGraphicsView::resetRoll()
{
	double angle = m_mainRenderer->GetActiveCamera()->GetRoll();
	m_mainRenderer->GetActiveCamera()->Roll(-angle);
	m_mainRenderer->GetRenderWindow()->Render();
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

	vtkCamera* camera = m_mainRenderer->GetActiveCamera();
	camera->SetPosition(newpos);
	camera->SetViewUp(0, 1, 0);
	m_mainRenderer->GetRenderWindow()->Render();
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

	vtkCamera* camera = m_mainRenderer->GetActiveCamera();
	camera->SetPosition(newpos);
	camera->SetViewUp(0, 0, 1);
	m_mainRenderer->GetRenderWindow()->Render();
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

	vtkCamera* camera = m_mainRenderer->GetActiveCamera();
	camera->SetPosition(newpos);
	camera->SetViewUp(0, 0, 1);
	m_mainRenderer->GetRenderWindow()->Render();
}

void VTK3DGraphicsView::getFocalPointAndDistance(double focal[3], double* distance)
{
	vtkCamera* camera = m_mainRenderer->GetActiveCamera();
	double pos[3];
	camera->GetFocalPoint(focal);
	camera->GetPosition(pos);
	QVector3D focalVec(focal[0], focal[1], focal[2]);
	QVector3D posVec(pos[0], pos[1], pos[2]);
	*distance = (focalVec - posVec).length();
}

double VTK3DGraphicsView::stdDistance(int pixels)
{
	double x0, y0, z0, x1, y1, z1;
	x0 = 0; y0 = 0; z0 = 0;
	x1 = pixels; y1 = 0; z1 = 0;
	m_mainRenderer->ViewportToNormalizedViewport(x0, y0);
	m_mainRenderer->NormalizedViewportToView(x0, y0, z0);
	m_mainRenderer->ViewToWorld(x0, y0, z0);

	m_mainRenderer->ViewportToNormalizedViewport(x1, y1);
	m_mainRenderer->NormalizedViewportToView(x1, y1, z1);
	m_mainRenderer->ViewToWorld(x1, y1, z1);

	QVector3D v0(x0, y0, z0);
	QVector3D v1(x1, y1, z1);

	return (v1 - v0).length();
}

void VTK3DGraphicsView::translate(int x, int y)
{
	double position[3];
	double focalpoint[3];
	double viewup[3];
	double s = m_mainRenderer->GetActiveCamera()->GetParallelScale();
//	double theta = m_mainRenderer->GetActiveCamera()->GetRoll() * M_PI /180;
	vtkCamera* camera = m_mainRenderer->GetActiveCamera();
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
	render();
}
