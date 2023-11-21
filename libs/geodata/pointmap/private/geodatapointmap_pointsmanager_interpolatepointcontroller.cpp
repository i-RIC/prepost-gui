#include "geodatapointmap_pointsmanager_addpointscommand.h"
#include "geodatapointmap_pointsmanager_interpolatepointcontroller.h"
#include "geodatapointmap_pointsmanager_interpolatepointcontroller_addpointcommand.h"
#include "geodatapointmap_pointsmanager_interpolatepointcontroller_settingdialog.h"
#include "geodatapointmap_pointsmanager_selectpointboundingboxcontroller.h"

#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>

namespace {

void TSplineSTL(std::vector<double>& x, std::vector<double>& y, int n,
								std::vector<double>& xout, std::vector<double>& yout, int iout, float sigma,
								std::vector<double>& yp, std::vector<double>& temp)
{
	// subroutine variables
	double delx1, dx1, delx2, delx12, c1, c2, c3, slpp1, deln, delnm1;
	double delnn, slppn, sigmap, dels, exps, sinhs, sinhin;
	double diag1, diag2, diagin, spdiag, dx2;
	double a, b, del1, del2, exps1, sinhd1, sinhd2;
	double tmp;
	int nm1, np1, i, ibak, nj;

	nm1 = n - 2; // may need to make adjustments here because of c arrays 0-n vs 1-n for fortran
	np1 = n;
	delx1 = x[1] - x[0];
	dx1 = (y[1] - y[0]) / delx1;
	delx2 = x[2] - x[1];
	delx12 = x[2] - x[0];
	c1 = - (delx12 + delx1) / delx12 / delx1;
	c2 = delx12 / delx1 / delx2;
	c3 = -delx1 / delx12 / delx2;
	slpp1 = c1 * y[0] + c2 * y[1] + c3 * y[2];
	deln = x[n - 1] - x[nm1];
	delnm1 = x[nm1] - x[n - 3];
	delnn = x[n - 1] - x[n - 3];
	c1 = (delnn + deln) / delnn / deln;
	c2 = - delnn / deln / delnm1;
	c3 = deln / delnn / delnm1;
	slppn = c3 * y[n - 3] + c2 * y[nm1] + c1 * y[n - 1];
	sigmap = fabs(sigma) * (double(n - 2)) / (x[n - 1] - x[0]);
	dels = sigmap * delx1;
	exps = exp(dels);
	sinhs = 0.5 * (exps - 1. / exps);
	sinhin = 1. / (delx1 * sinhs);
	diag1 = sinhin * (dels * 0.5 * (exps + 1. / exps) - sinhs);
	diagin = 1. / diag1;
	yp[0] = diagin * (dx1 - slpp1);
	spdiag = sinhin * (sinhs - dels);
	temp[0] = diagin * spdiag;
	for (i = 1; i <= nm1; i++) {
		delx2 = x[i + 1] - x[i];
		dx2 = (y[i + 1] - y[i]) / delx2;
		dels = sigmap * delx2;
		exps = exp(dels);
		sinhs = 0.5 * (exps - 1. / exps);
		sinhin = 1. / (delx2 * sinhs);
		diag2 = sinhin * (dels * (0.5 * (exps + 1. / exps)) - sinhs);
		diagin = 1. / (diag1 + diag2 - spdiag * temp[i - 1]);
		yp[i] = diagin * (dx2 - dx1 - spdiag * yp[i - 1]);
		spdiag = sinhin * (sinhs - dels);
		temp[i] = diagin * spdiag;
		dx1 = dx2;
		diag1 = diag2;
	}
	diagin = 1. / (diag1 - spdiag * temp[nm1]);
	yp[n - 1] = diagin * (slppn - dx2 - spdiag * yp[nm1]);
	for (i = 1; i <= n - 1; i++) {
		ibak = np1 - i - 1;
		yp[ibak] = yp[ibak] - temp[ibak] * yp[ibak + 1];
	}
	a = x[0];
	b = x[1];
	nj = 1;
	for (i = 0; i <= iout - 1; i++) {
		while (xout[i] > b) {
			a = b;
			nj = nj + 1;
			if (nj < n) {
				b = x[nj];
			}
		}
		del1 = xout[i] - a;
		del2 = b - xout[i];
		dels = b - a;
		exps1 = exp(sigmap * del1);
		sinhd1 = 0.5 * (exps1 - 1. / exps1);
		exps = exp(sigmap * del2);
		sinhd2 = 0.5 * (exps - 1. / exps);
		exps = exps * exps1;
		sinhs = 0.5 * (exps - 1. / exps);
		tmp = (yp[nj] * sinhd1 + yp[nj - 1] * sinhd2) /
					sinhs + ((y[nj] - yp[nj]) * del1 + (y[nj - 1] - yp[nj - 1]) * del2) / dels;
		yout[i] = (yp[nj] * sinhd1 + yp[nj - 1] * sinhd2) /
							sinhs + ((y[nj] - yp[nj]) * del1 + (y[nj - 1] - yp[nj - 1]) * del2) / dels;
	}
}

} // namespace

GeoDataPointmap::PointsManager::InterpolatePointController::InterpolatePointController(PointsManager* manager) :
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_pointsController {},
	m_mouseEventMode {MouseEventMode::SelectFirstPointNotPossible},
	m_manager {manager}
{
	auto v = manager->m_parent->dataModel()->graphicsView();
	m_pointsController.pointsActor()->GetProperty()->SetPointSize(7 * v->devicePixelRatioF());
}

GeoDataPointmap::PointsManager::InterpolatePointController::~InterpolatePointController()
{}

vtkActor* GeoDataPointmap::PointsManager::InterpolatePointController::pointsActor() const
{
	return m_pointsController.pointsActor();
}

void GeoDataPointmap::PointsManager::InterpolatePointController::setZDepthForPoints(double z)
{
	m_pointsController.pointsActor()->SetPosition(0, 0, z);
}

void GeoDataPointmap::PointsManager::InterpolatePointController::addActorsToRenderer(vtkRenderer* renderer)
{
	renderer->AddActor(m_pointsController.pointsActor());
}

void GeoDataPointmap::PointsManager::InterpolatePointController::removeActorsFromRenderer(vtkRenderer* renderer)
{
	renderer->RemoveActor(m_pointsController.pointsActor());
}

void GeoDataPointmap::PointsManager::InterpolatePointController::activate(VTK2DGraphicsView* v)
{
	auto w = m_manager->m_parent->preProcessorWindow();
	InformationDialog::information(w, GeoDataPointmap::tr("Information"), GeoDataPointmap::tr("To interpolate points, select existing points by mouse-clicking, use Alt to define path between known points, and finish by double-clicking or pressing return key."), "geodatapointmapinterpolatepoint");

	m_mouseEventMode = MouseEventMode::SelectFirstPointNotPossible;
	updateMouseCursor(v);
	m_pointsController.clear();

	m_manager->m_parent->actorCollection()->AddItem(m_pointsController.pointsActor());
	m_manager->m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPointmap::PointsManager::InterpolatePointController::deactivate(VTK2DGraphicsView* v)
{
	m_mouseEventMode = MouseEventMode::SelectFirstPointNotPossible;
	updateMouseCursor(v);
	m_pointsController.clear();

	m_pointsController.pointsActor()->VisibilityOff();
	m_manager->m_parent->actorCollection()->RemoveItem(m_pointsController.pointsActor());
}

void GeoDataPointmap::PointsManager::InterpolatePointController::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* /*v*/)
{
	if (iRIC::isEnterKey(event->key())) {
		finish();
	}
}

void GeoDataPointmap::PointsManager::InterpolatePointController::handleMouseDoubleClickEvent(QMouseEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{
	finish();
}

void GeoDataPointmap::PointsManager::InterpolatePointController::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (m_mouseEventMode == MouseEventMode::SelectFirstPointNotPossible || m_mouseEventMode == MouseEventMode::SelectFirstPointPossible) {
		auto worldPos = v->viewportToWorld(event->pos());
		double radius = v->stdRadius(iRIC::nearRadius());
		auto id = m_manager->closestPointWithinRadius(worldPos, radius);
		if (id >= 0) {
			m_mouseEventMode = MouseEventMode::SelectFirstPointPossible;
		} else {
			m_mouseEventMode = MouseEventMode::SelectFirstPointNotPossible;
		}
		updateMouseCursor(v);
	} else {
		auto worldPos = v->viewportToWorld(event->pos());
		double radius = v->stdRadius(iRIC::nearRadius());
		auto id = m_manager->closestPointWithinRadius(worldPos, radius);
		if (id >= 0) {
			m_mouseEventMode = MouseEventMode::AddExistingPoint;
		} else {
			m_mouseEventMode = MouseEventMode::AddNewPoint;
		}
		updateMouseCursor(v);
	}
}

void GeoDataPointmap::PointsManager::InterpolatePointController::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (m_mouseEventMode == MouseEventMode::SelectFirstPointPossible || m_mouseEventMode == MouseEventMode::SelectFirstPointNotPossible) {
		auto worldPos = v->viewportToWorld(event->pos());
		double radius = v->stdRadius(iRIC::nearRadius());
		auto id = m_manager->closestPointWithinRadius(worldPos, radius);
		if (id >= 0) {
			double p[3];
			m_manager->m_points->GetPoint(id, p);

			QPointF point(p[0], p[1]);
			double value = m_manager->m_points->GetPointData()->GetArray(VALUES)->GetTuple1(id);
			m_manager->m_parent->pushRenderCommand(new AddPointCommand(point, value, false, this));
		}
	} else if (m_mouseEventMode == MouseEventMode::AddExistingPoint || m_mouseEventMode == MouseEventMode::AddNewPoint) {
		auto worldPos = v->viewportToWorld(event->pos());
		double radius = v->stdRadius(iRIC::nearRadius());
		auto id = m_manager->closestPointWithinRadius(worldPos, radius);
		if (id >= 0) {
			double p[3];
			m_manager->m_points->GetPoint(id, p);

			QPointF point(p[0], p[1]);
			double value = m_manager->m_points->GetPointData()->GetArray(VALUES)->GetTuple1(id);
			m_manager->m_parent->pushRenderCommand(new AddPointCommand(point, value, false, this));
		} else {
			m_manager->m_parent->pushRenderCommand(new AddPointCommand(worldPos, 0, true, this));
		}
	}
}

void GeoDataPointmap::PointsManager::InterpolatePointController::updateMouseCursor(VTK2DGraphicsView* v)
{
	if (m_mouseEventMode == MouseEventMode::SelectFirstPointNotPossible) {
		v->setCursor(Qt::ArrowCursor);
	} else if (m_mouseEventMode == MouseEventMode::SelectFirstPointPossible) {
		v->setCursor(m_addCursor);
	} else if (m_mouseEventMode == MouseEventMode::AddExistingPoint) {
		v->setCursor(m_addCursor);
	} else if (m_mouseEventMode == MouseEventMode::AddNewPoint) {
		v->setCursor(Qt::CrossCursor);
	}
}

void GeoDataPointmap::PointsManager::InterpolatePointController::finish()
{
	auto w = m_manager->m_parent->preProcessorWindow();
	if (! (m_newFlags.at(m_newFlags.size() - 1) == false && m_pointsController.polyLine().size() > 1)) {
		int ret = QMessageBox::information(w, GeoDataPointmap::tr("Warning"), GeoDataPointmap::tr("To interpolate points, you must specify more than two points, and the last point should be existing point. "
																																							 "Do you want to retry, or cancel the operation?"), QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Retry) {return;};
	} else {
		double len = 0;
		std::vector<double> xarr, yarr, zarr;
		std::vector<double> lens_xy;
		std::vector<double> lens_z;

		auto line = m_pointsController.polyLine();

		lens_xy.push_back(0);
		lens_z.push_back(0);

		for (int i = 0; i < line.size(); ++i) {
			auto p = line.at(i);
			xarr.push_back(p.x());
			yarr.push_back(p.y());
			if (m_newFlags.at(i) == false) {
				zarr.push_back(m_values.at(i));
			}
		}

		for (int i = 0; i < line.size() - 1; ++i) {
			auto p1 = line.at(i);
			auto p2 = line.at(i + 1);
			len += iRIC::distance(p1, p2);
			lens_xy.push_back(len);

			if (m_newFlags[i + 1] == false) {
				lens_z.push_back(len);
			}
		}
		SettingDialog dialog(w);
		dialog.setLineLength(len);
		int ret = dialog.exec();
		if (ret == QDialog::Accepted) {
			double increment = dialog.increment();

			std::vector<QPointF> points;
			std::vector<double> xpoint, ypoint, values;

			// number of points to be added by interpolation
			int numNewPoints = static_cast<int>(len / increment) + 1;

			xpoint.assign(numNewPoints, 0);
			ypoint.assign(numNewPoints, 0);
			values.assign(numNewPoints, 0);

			std::vector<double> sout, yp, tmp;
			sout.assign(numNewPoints, 0);
			yp.assign(numNewPoints, 0);
			tmp.assign(numNewPoints, 0);
			for (int i = 0; i < numNewPoints; ++i) {
				sout[i] = i * len / (numNewPoints - 1);
			}

			// interpolation for x and y
			if (lens_xy.size() == 2) {
				for (int i = 0; i < numNewPoints; ++i) {
					double t = i / static_cast<double>(numNewPoints - 1);
					xpoint[i] = xarr.at(0) * (1 - t) + xarr.at(1) * t;
					ypoint[i] = yarr.at(0) * (1 - t) + yarr.at(1) * t;
				}
			} else {
				// spline interpolation for x and y
				::TSplineSTL(lens_xy, xarr, xarr.size(), sout, xpoint, numNewPoints, 5.0, yp, tmp);
				::TSplineSTL(lens_xy, yarr, yarr.size(), sout, ypoint, numNewPoints, 5.0, yp, tmp);
			}

			// interpolation for z
			if (lens_z.size() == 2) {
				for (int i = 0; i < numNewPoints; ++i) {
					double t = i / static_cast<double>(numNewPoints - 1);
					values[i] = zarr.at(0) * (1 - t) + zarr.at(1) * t;
				}
			} else {
				// spline interpolation for z
				::TSplineSTL(lens_z, zarr, zarr.size(), sout, values, numNewPoints, 10.0, yp, tmp);
			}

			// remove first and last points, because they are points already exists.
			xpoint.erase(xpoint.begin());
			xpoint.pop_back();
			ypoint.erase(ypoint.begin());
			ypoint.pop_back();
			values.erase(values.begin());
			values.pop_back();
			for (int i = 0; i < xpoint.size(); ++i) {
				points.push_back(QPointF(xpoint.at(i), ypoint.at(i)));
			}
			m_pointsController.clear();
			iRICUndoStack::instance().clear(); // to avoid crash while undoing
			m_manager->m_parent->pushModifyCommand(new AddPointsCommand(points, values, m_manager));
		}
	}
	m_manager->switchToSelectPointBoundingBoxMode();
}
