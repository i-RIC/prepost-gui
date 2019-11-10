#define _USE_MATH_DEFINES
#include <cmath>

#include "vtk2dgraphicsview.h"
#include <guibase/vtk2dinteractorstyle.h>
#include <guicore/datamodel/graphicswindowsimpledatamodel.h>

#include <misc/iricundostack.h>

#include <QVector2D>
#include <QMouseEvent>
#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

VTK2DGraphicsView::VTK2DGraphicsView(QWidget* parent)
	: VTKGraphicsView(parent)
{
	vtkRenderWindow* renderWindow = GetRenderWindow();

	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
	renderWindow->SetInteractor(iren);
	vtkInteractorStyle* style = vtk2DInteractorStyle::New();
	iren->SetInteractorStyle(style);
	iren->Initialize();

	iren->Delete();
	style->Delete();
}

void VTK2DGraphicsView::resetCamera()
{
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	mainRenderer()->SetActiveCamera(camera);
}

void VTK2DGraphicsView::fitInView()
{
	auto renderer = mainRenderer();
	setupCamera();
	renderer->ResetCamera();
	double bounds[6];
	renderer->ComputeVisiblePropBounds(bounds);

	double w1 = bounds[1] - bounds[0];
	double w2 = bounds[3] - bounds[2];
	w1 *= w1;
	w2 *= w2;
	double radius = w1 + w2;
	// compute the radius of the enclosing sphere
	radius = sqrt(radius)*0.5;
	if (radius == 0) {radius = 1;}
	renderer->GetActiveCamera()->SetParallelScale(radius);
	QUndoCommand* com = new VTKGraphicsViewArbitraryMove(mainRenderer()->GetActiveCamera(), this);
	com->redo();
	delete com;

	ResetCameraClippingRange();
	update2Ds();
}


void VTK2DGraphicsView::getDataRegion(double* xmin, double* xmax, double* ymin, double* ymax)
{
	double bounds[6];
	mainRenderer()->ComputeVisiblePropBounds(bounds);

	*xmin = bounds[0];
	*xmax = bounds[1];
	*ymin = bounds[2];
	*ymax = bounds[3];
}

void VTK2DGraphicsView::getDrawnRegion(double* xmin, double* xmax, double* ymin, double* ymax)
{
	QSize windowsize = size();
	// lowerleft
	double x = 0;
	double y = 0;
	viewportToWorld(x, y);
	*xmin = x;
	*xmax = x;
	*ymin = y;
	*ymax = y;

	// lowerright
	x = windowsize.width();
	y = 0;
	viewportToWorld(x, y);
	*xmin = qMin(*xmin, x);
	*xmax = qMax(*xmax, x);
	*ymin = qMin(*ymin, y);
	*ymax = qMax(*ymax, y);

	// topleft
	x = 0;
	y = windowsize.height();
	viewportToWorld(x, y);
	*xmin = qMin(*xmin, x);
	*xmax = qMax(*xmax, x);
	*ymin = qMin(*ymin, y);
	*ymax = qMax(*ymax, y);

	// topright
	x = windowsize.width();
	y = windowsize.height();
	viewportToWorld(x, y);
	*xmin = qMin(*xmin, x);
	*xmax = qMax(*xmax, x);
	*ymin = qMin(*ymin, y);
	*ymax = qMax(*ymax, y);
}

void VTK2DGraphicsView::rotate(double r)
{
	mainRenderer()->GetActiveCamera()->Roll(r);
}

void VTK2DGraphicsView::resetRoll()
{
	double angle = mainRenderer()->GetActiveCamera()->GetRoll();
	mainRenderer()->GetActiveCamera()->Roll(-angle);
}

void VTK2DGraphicsView::viewportToWorld(double& x, double& y) const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (mainRenderer());
	double z = 0.0;
	r->ViewportToNormalizedViewport(x, y);
	r->NormalizedViewportToView(x, y, z);
	y = -y;
	r->ViewToWorld(x, y, z);
}

QPointF VTK2DGraphicsView::viewportToWorld(const QPoint p) const
{
	double x = p.x();
	double y = p.y();
	viewportToWorld(x, y);

	return QPointF(x, y);
}

void VTK2DGraphicsView::worldToViewport(double& x, double& y) const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (mainRenderer());
	double z = 0.0;
	r->WorldToView(x, y, z);
	y = -y;
	r->ViewToNormalizedViewport(x, y, z);
	r->NormalizedViewportToViewport(x, y);
}

QPointF VTK2DGraphicsView::worldToViewport(QPointF p) const
{
	double x = p.x();
	double y = p.y();
	worldToViewport(x, y);

	return QPointF(x, y);
}

void VTK2DGraphicsView::ResetCameraClippingRange()
{
	double bounds[6];
	mainRenderer()->ComputeVisiblePropBounds(bounds);

	// move camera forward or backword.
	double center[3];
	double depth;
	double vn[3];

	vtkCamera* camera = mainRenderer()->GetActiveCamera();
	camera->GetFocalPoint(center);
	camera->GetViewPlaneNormal(vn);
	// focal point Z is always zero.
	center[2] = 0;

//	depth = (bounds[5] - center[2]) * 1.01;
	depth = 200;
	bounds[4] = 0;
	bounds[5] = depth;

	// update the camera
	camera->SetFocalPoint(center[0],center[1],center[2]);
	camera->SetPosition(center[0] + depth * vn[0], center[1] + depth * vn[1], center[2] + depth * vn[2]);
	mainRenderer()->ResetCameraClippingRange(bounds);
}

double VTK2DGraphicsView::stdRadius(int pixels) const
{
	double x1, x2, y1, y2;
	x1 = 0;
	y1 = 0;
	x2 = x1 + pixels;
	y2 = 0;
	viewportToWorld(x1, y1);
	viewportToWorld(x2, y2);
	double xdist = x2 - x1;
	double ydist = y2 - y1;
	xdist *= xdist;
	ydist *= ydist;
	return std::sqrt(xdist + ydist);
}


void VTK2DGraphicsView::emitPosition(int x, int y)
{
	emit positionChanged(x, y);
}
void VTK2DGraphicsView::emitWorldPosition(int x, int y)
{
	double dx, dy;
	dx = x;
	dy = y;

	viewportToWorld(dx, dy);
	emit worldPositionChanged(QVector2D(dx, dy));
}

void VTK2DGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	double dx, dy;
	dx = event->x();
	dy = event->y();

	viewportToWorld(dx, dy);
	emit worldPositionChangedForStatusBar(QVector2D(dx, dy));
	VTKGraphicsView::mouseMoveEvent(event);
}

void VTK2DGraphicsView::translate(int x, int y)
{
	auto r = mainRenderer();
	double position[3];
	double focalpoint[3];
	double s = r->GetActiveCamera()->GetParallelScale();
	double theta = r->GetActiveCamera()->GetRoll() * M_PI / 180;
	r->GetActiveCamera()->GetPosition(position);
	r->GetActiveCamera()->GetFocalPoint(focalpoint);
	double _x = (x * cos(theta) + y * sin(theta)) * s / 1.5;
	double _y = (-x * sin(theta)+ y * cos(theta)) * s / 1.5;
	position[0] += _x;
	position[1] += _y;
	focalpoint[0] += _x;
	focalpoint[1] += _y;
	r->GetActiveCamera()->SetPosition(position);
	r->GetActiveCamera()->SetFocalPoint(focalpoint);
}

void VTK2DGraphicsView::panTo(double x, double y)
{
	auto camera = mainRenderer()->GetActiveCamera();

	double pos[3];
	camera->GetPosition(pos);
	camera->SetPosition(x, y, pos[2]);
	camera->GetFocalPoint(pos);
	camera->SetFocalPoint(x, y, pos[2]);
	render();
}

void VTK2DGraphicsView::cameraResetRotation()
{
	resetRoll();
	model()->viewOperationEndedGlobal();
	render();
}

void VTK2DGraphicsView::cameraRotate90()
{
	rotate(90);
	model()->viewOperationEndedGlobal();
	render();
}
