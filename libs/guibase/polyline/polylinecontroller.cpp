#include "polylinecontroller.h"

#include "private/polylinecontroller_impl.h"

#include <QPointF>
#include <QVector2D>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>

namespace {

int LINEWIDTH_STD = 1;
int LINEWIDTH_FOCUSED = 3;

} // namespace

PolyLineController::Impl::Impl() :
	m_isFocused {false},
	m_polyData {vtkPolyData::New()}
{
	m_linesActor.setPolyData(m_polyData);
	m_pointsActor.setPolyData(m_polyData);
}

PolyLineController::Impl::~Impl()
{
	m_polyData->Delete();
}

PolyLineController::PolyLineController() :
	impl {new Impl {}}
{}

PolyLineController::~PolyLineController()
{
	delete impl;
}

std::vector<QPointF> PolyLineController::polyLine() const
{
	std::vector<QPointF> ret;
	for (vtkIdType i = 0; i < impl->m_polyData->GetNumberOfPoints(); ++i) {
		double v[3];
		impl->m_polyData->GetPoint(i, v);
		ret.push_back(QPointF(v[0], v[1]));
	}
	return ret;
}

void PolyLineController::setPolyLine(const std::vector<QPointF>& polyLine)
{
	impl->m_polyData->Initialize();

	vtkPoints* points = vtkPoints::New();
	for (QPointF p : polyLine) {
		points->InsertNextPoint(p.x(), p.y(), 0);
	}
	impl->m_polyData->SetPoints(points);
	points->Delete();

	vtkCellArray* ca = vtkCellArray::New();
	for (int i = 0; i < polyLine.size() - 1; ++i) {
		vtkIdType vids[2];
		vids[0] = i;
		vids[1] = i + 1;
		ca->InsertNextCell(2, vids);
	}
	impl->m_polyData->SetLines(ca);
	ca->Delete();

	impl->m_polyData->Modified();
}

bool PolyLineController::isFocused() const
{
	return impl->m_isFocused;
}

void PolyLineController::setFocused(bool focused)
{
	impl->m_isFocused = focused;
	auto prop = impl->m_linesActor.actor()->GetProperty();
	if (focused) {
		prop->SetLineWidth(LINEWIDTH_FOCUSED);
	} else {
		prop->SetLineWidth(LINEWIDTH_STD);
	}
}

bool PolyLineController::isVertexSelectable(const QPointF& pos, double limitDistance, int* vid)
{
	*vid = impl->m_polyData->FindPoint(pos.x(), pos.y(), 0.0);

	double v[3];
	impl->m_polyData->GetPoint(static_cast<vtkIdType>(*vid), v);
	QVector2D diff(pos.x() - v[0], pos.y() - v[1]);

	return diff.lengthSquared() < limitDistance * limitDistance;
}

bool PolyLineController::isEdgeSelectable(const QPointF& pos, double limitDistance, int* edgeId)
{
	double x[3] = {pos.x(), pos.y(), 0};
	int subId;
	double pcoords[3];
	double weights[32];

	double d2 = limitDistance * limitDistance;

	vtkIdType id = impl->m_polyData->FindCell(x, NULL, 0, d2, subId, pcoords, weights);
	if (id < 0) {return false;}

	*edgeId = id;
	return true;
}

vtkPolyData* PolyLineController::polyData() const
{
	return impl->m_polyData;
}

vtkActor* PolyLineController::pointsActor() const
{
	return impl->m_pointsActor.actor();
}

vtkActor* PolyLineController::linesActor() const
{
	return impl->m_linesActor.actor();
}
