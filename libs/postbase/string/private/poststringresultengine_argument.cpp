#include "poststringresultengine_argument.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>
#include <guicore/grid/v4grid.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/postbaseiterativenumericaldatacontainer.h>
#include <guicore/postcontainer/postbaseiterativestringdatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/stringtool.h>

#include <QJSValue>

PostStringResultEngine::Argument::Argument(const PostStringResultArgumentContainer& c) :
	m_setting {c}
{}

QString PostStringResultEngine::Argument::valueForTest() const
{
	return m_setting.valueForTest;
}

QJSValue PostStringResultEngine::Argument::value(v4PostZoneDataContainer* c) const
{
	if (m_setting.type == PostStringResultArgumentContainer::Type::BaseIterative) {
		return baseIterativeValue(c);
	} else if (m_setting.type == PostStringResultArgumentContainer::Type::GridNode) {
		return nodeValue(c);
	} else if (m_setting.type == PostStringResultArgumentContainer::Type::GridCell) {
		return cellValue(c);
	} else if (m_setting.type == PostStringResultArgumentContainer::Type::GridIEdge) {
		return iEdgeValue(c);
	} else if (m_setting.type == PostStringResultArgumentContainer::Type::GridJEdge) {
		return jEdgeValue(c);
	} else if (m_setting.type == PostStringResultArgumentContainer::Type::GridKEdge) {
		return kEdgeValue(c);
	}

	return 0;
}

std::string PostStringResultEngine::Argument::name() const
{
	return iRIC::toStr(m_setting.name);
}

QJSValue PostStringResultEngine::Argument::baseIterativeValue(v4PostZoneDataContainer* c) const
{
	auto n = name();
	auto s = c->solutionInfo();

	auto strData = s->baseIterativeStringResult(n);
	if (strData != nullptr) {
		return strData->data().c_str();
	}

	auto numData = s->baseIterativeNumericalResult(n);
	if (numData != nullptr) {
		return numData->data();
	}

	return 0;
}

QJSValue PostStringResultEngine::Argument::nodeValue(v4PostZoneDataContainer* c) const
{
	auto pd = c->gridData()->grid()->vtkData()->data()->GetPointData();
	int index = nodeIndex(c);
	auto da = pd->GetArray(name().c_str());
	if (da == nullptr) {return 0;}

	return *(da->GetTuple(index));
}

int PostStringResultEngine::Argument::nodeIndex(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto uGrid2d = dynamic_cast<v4Unstructured2dGrid*>(grid);
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	if (uGrid2d != nullptr) {
		return m_setting.index;
	} else if (sGrid2d != nullptr) {
		return sGrid2d->pointIndex(m_setting.i, m_setting.j);
	} else if (sGrid3d != nullptr) {
		return sGrid3d->pointIndex(m_setting.i, m_setting.j, m_setting.k);
	}
	return 0;
}

QJSValue PostStringResultEngine::Argument::cellValue(v4PostZoneDataContainer* c) const
{
	auto cd = c->gridData()->grid()->vtkData()->data()->GetCellData();
	int index = cellIndex(c);
	auto da = cd->GetArray(name().c_str());
	if (da == nullptr) {return 0;}

	return *(da->GetTuple(index));
}

int PostStringResultEngine::Argument::cellIndex(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto uGrid2d = dynamic_cast<v4Unstructured2dGrid*>(grid);
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	if (uGrid2d != nullptr) {
		return m_setting.index;
	} else if (sGrid2d != nullptr) {
		return sGrid2d->cellIndex(m_setting.i, m_setting.j);
	} else if (sGrid3d != nullptr) {
		return sGrid3d->cellIndex(m_setting.i, m_setting.j, m_setting.k);
	}
	return 0;
}

QJSValue PostStringResultEngine::Argument::iEdgeValue(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	int index = iEdgeIndex(c);
	vtkDataArray* da = nullptr;
	if (sGrid2d != nullptr) {
		auto cd = sGrid2d->vtkIEdgeData()->data()->GetCellData();
		da = cd->GetArray(name().c_str());
	} else if (sGrid3d != nullptr) {
		auto cd = sGrid3d->vtkIFaceData()->data()->GetCellData();
		da = cd->GetArray(name().c_str());
	}
	if (da == nullptr) {return 0;}

	return *(da->GetTuple(index));
}

int PostStringResultEngine::Argument::iEdgeIndex(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	if (sGrid2d != nullptr) {
		return sGrid2d->iEdgeIndex(m_setting.i, m_setting.j);
	} else if (sGrid3d != nullptr) {
		return sGrid3d->iFaceIndex(m_setting.i, m_setting.j, m_setting.k);
	}
	return 0;
}


QJSValue PostStringResultEngine::Argument::jEdgeValue(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	int index = jEdgeIndex(c);
	vtkDataArray* da = nullptr;
	if (sGrid2d != nullptr) {
		auto cd = sGrid2d->vtkJEdgeData()->data()->GetCellData();
		da = cd->GetArray(name().c_str());
	} else if (sGrid3d != nullptr) {
		auto cd = sGrid3d->vtkJFaceData()->data()->GetCellData();
		da = cd->GetArray(name().c_str());
	}
	if (da == nullptr) {return 0;}

	return *(da->GetTuple(index));
}

int PostStringResultEngine::Argument::jEdgeIndex(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	if (sGrid2d != nullptr) {
		return sGrid2d->jEdgeIndex(m_setting.i, m_setting.j);
	} else if (sGrid3d != nullptr) {
		return sGrid3d->jFaceIndex(m_setting.i, m_setting.j, m_setting.k);
	}
	return 0;
}

QJSValue PostStringResultEngine::Argument::kEdgeValue(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	int index = kEdgeIndex(c);
	vtkDataArray* da = nullptr;
	if (sGrid3d != nullptr) {
		auto cd = sGrid3d->vtkKFaceData()->data()->GetCellData();
		da = cd->GetArray(name().c_str());
	}
	if (da == nullptr) {return 0;}

	return *(da->GetTuple(index));
}

int PostStringResultEngine::Argument::kEdgeIndex(v4PostZoneDataContainer* c) const
{
	auto grid = c->gridData()->grid();
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid);

	if (sGrid3d != nullptr) {
		return sGrid3d->kFaceIndex(m_setting.i, m_setting.j, m_setting.k);
	}
	return 0;
}
