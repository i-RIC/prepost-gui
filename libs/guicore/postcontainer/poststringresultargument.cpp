#include "../grid/v4structured2dgrid.h"
#include "../grid/v4structured3dgrid.h"
#include "postbaseiterativenumericaldatacontainer.h"
#include "postbaseiterativestringdatacontainer.h"
#include "postsolutioninfo.h"
#include "poststringresult.h"
#include "poststringresultargument.h"
#include "v4postzonedatacontainer.h"
#include "v4solutiongrid.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <misc/stringtool.h>

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

PostStringResultArgument::PostStringResultArgument(PostStringResult* result) :
	ProjectDataItem {result}
{}

PostStringResultArgument::Type PostStringResultArgument::type() const
{
	return m_setting.type;
}

void PostStringResultArgument::setType(Type type)
{
	m_setting.type = type;
}

std::string PostStringResultArgument::name() const
{
	return iRIC::toStr(m_setting.name);
}

void PostStringResultArgument::setName(const std::string& name)
{
	m_setting.name = name.c_str();
}

std::string PostStringResultArgument::refName() const
{
	return iRIC::toStr(m_setting.refName);
}

void PostStringResultArgument::setRefName(const std::string& name)
{
	m_setting.refName = name.c_str();
}

int PostStringResultArgument::i() const
{
	return m_setting.i;
}

void PostStringResultArgument::setI(int i)
{
	m_setting.i = i;
}

int PostStringResultArgument::j() const
{
	return m_setting.j;
}

void PostStringResultArgument::setJ(int j)
{
	m_setting.j = j;
}

int PostStringResultArgument::k() const
{
	return m_setting.k;
}

void PostStringResultArgument::setK(int k)
{
	m_setting.k = k;
}

int PostStringResultArgument::index() const
{
	return m_setting.index;
}

void PostStringResultArgument::setIndex(int index)
{
	m_setting.index = index;
}

QString PostStringResultArgument::valueForTest() const
{
	return m_setting.valueForTest;
}

void PostStringResultArgument::setValueForTest(const QString &v)
{
	m_setting.valueForTest = v;
}

QJSValue PostStringResultArgument::jsValue() const
{
	auto t = m_setting.type.value();
	if (t == Type::BaseIterative) {
		return baseIterativeJsValue();
	} else if (t == Type::GridNode) {
		return nodeJsValue();
	} else if (t == Type::GridCell) {
		return cellJsValue();
	} else if (t == Type::GridEdgeI) {
		return edgeIJsValue();
	} else if (t == Type::GridEdgeJ) {
		return edgeJJsValue();
	} else if (t == Type::GridEdgeK) {
		return edgeKJsValue();
	} else {
		return "";
	}
}

void PostStringResultArgument::copyValue(const PostStringResultArgument& arg)
{
	m_setting = arg.m_setting;
}

PostStringResultArgument::Setting PostStringResultArgument::setting() const
{
	return m_setting;
}

void PostStringResultArgument::setSetting(const Setting& s)
{
	m_setting = s;
}

void PostStringResultArgument::fixIndexIfNeeded(bool* fixed)
{
	auto t = m_setting.type.value();
	if (t == Type::GridNode) {
		fixNodeIndexIfNeeded(fixed);
	} else if (t == Type::GridCell) {
		fixCellIndexIfNeeded(fixed);
	} else if (t == Type::GridEdgeI) {
		fixEdgeIIndexIfNeeded(fixed);
	} else if (t == Type::GridEdgeJ) {
		fixEdgeJIndexIfNeeded(fixed);
	} else if (t == Type::GridEdgeK) {
		fixEdgeKIndexIfNeeded(fixed);
	}
}

void PostStringResultArgument::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
}

void PostStringResultArgument::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

QJSValue PostStringResultArgument::baseIterativeJsValue() const
{
	auto z = zoneDataContainer();
	auto s = z->solutionInfo();
	auto strData = s->baseIterativeStringResult(name());
	if (strData != nullptr) {
		return strData->data().c_str();
	}
	auto numData = s->baseIterativeNumericalResult(name());
	if (numData != nullptr) {
		return numData->data();
	}
	return 0;
}

QJSValue PostStringResultArgument::nodeJsValue() const
{
	auto z = zoneDataContainer();
	auto pd = z->gridData()->grid()->vtkData()->data()->GetPointData();
	int index = arrayIndex();
	vtkDataArray* da = pd->GetArray(name().c_str());
	return *(da->GetTuple(index));
}

QJSValue PostStringResultArgument::cellJsValue() const
{
	auto z = zoneDataContainer();
	auto pd = z->gridData()->grid()->vtkData()->data()->GetCellData();
	int index = arrayIndex();
	vtkDataArray* da = pd->GetArray(name().c_str());
	return *(da->GetTuple(index));
}

QJSValue PostStringResultArgument::edgeIJsValue() const
{
	auto z = zoneDataContainer();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(z->gridData()->grid());
	auto pd = sGrid->vtkIEdgeData()->data()->GetCellData();
	int index = arrayIndex();
	vtkDataArray* da = pd->GetArray(name().c_str());
	return *(da->GetTuple(index));
}

QJSValue PostStringResultArgument::edgeJJsValue() const
{
	auto z = zoneDataContainer();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(z->gridData()->grid());
	auto pd = sGrid->vtkJEdgeData()->data()->GetCellData();
	int index = arrayIndex();
	vtkDataArray* da = pd->GetArray(name().c_str());
	return *(da->GetTuple(index));
}

QJSValue PostStringResultArgument::edgeKJsValue() const
{
	// @todo implement this
	return 0;
}

int PostStringResultArgument::arrayIndex() const
{
	auto z = zoneDataContainer();
	auto grid = z->gridData();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid->grid());
	auto uGrid = dynamic_cast<vtkUnstructuredGrid*>(grid->grid());

	if (uGrid != nullptr) {
		// unstructured grid
		return m_setting.index;
	} else if (sGrid2d != nullptr) {
		auto t = m_setting.type.value();
		if (t == Type::GridNode) {
			return sGrid2d->pointIndex(m_setting.i,  m_setting.j);
		} else if (t == Type::GridCell) {
			return sGrid2d->cellIndex(m_setting.i, m_setting.j);
		} else if (t == Type::GridEdgeI) {
			return sGrid2d->iEdgeIndex(m_setting.i, m_setting.j);
		} else if (t == Type::GridEdgeJ) {
			return sGrid2d->jEdgeIndex(m_setting.i, m_setting.j);
		} else if (t == Type::GridEdgeK) {
			return 0;
		}
	} else if (sGrid3d != nullptr) {
		auto t = m_setting.type.value();
		if (t == Type::GridNode) {
			return sGrid3d->pointIndex(m_setting.i,  m_setting.j, m_setting.k);
		} else if (t == Type::GridCell) {
			return sGrid3d->cellIndex(m_setting.i, m_setting.j, m_setting.k);
		} else if (t == Type::GridEdgeI) {
			return sGrid3d->iFaceIndex(m_setting.i, m_setting.j, m_setting.k);
		} else if (t == Type::GridEdgeJ) {
			return sGrid3d->jFaceIndex(m_setting.i, m_setting.j, m_setting.k);
		} else if (t == Type::GridEdgeK) {
			return sGrid3d->kFaceIndex(m_setting.i, m_setting.j, m_setting.k);
		}
	}
	// just for avoiding compiler warning
	return 0;
}

void PostStringResultArgument::fixNodeIndexIfNeeded(bool* fixed)
{
	auto z = zoneDataContainer();
	auto grid = z->gridData();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid->grid());
	auto uGrid = dynamic_cast<vtkUnstructuredGrid*>(grid->grid());

	*fixed = false;
	if (uGrid != nullptr) {
		if (m_setting.index >= uGrid->GetNumberOfPoints()) {
			*fixed = true;
			m_setting.index = 0;
		}
	} else if (sGrid2d != nullptr) {
		if (m_setting.i >= sGrid2d->dimensionI()) {
			*fixed = true;
		} else if (m_setting.j >= sGrid2d->dimensionJ()) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	} else if (sGrid3d != nullptr) {
		if (m_setting.i >= sGrid3d->dimensionI()) {
			*fixed = true;
		} else if (m_setting.j >= sGrid3d->dimensionJ()) {
			*fixed = true;
		} else if (m_setting.k >= sGrid3d->dimensionK()) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	}
}

void PostStringResultArgument::fixCellIndexIfNeeded(bool* fixed)
{
	auto z = zoneDataContainer();
	auto grid = z->gridData();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid->grid());
	auto uGrid = dynamic_cast<vtkUnstructuredGrid*>(grid->grid());

	*fixed = false;
	if (uGrid != nullptr) {
		if (m_setting.index >= uGrid->GetNumberOfCells()) {
			*fixed = true;
			m_setting.index = 0;
		}
	} else if (sGrid2d != nullptr) {
		if (m_setting.i >= sGrid2d->dimensionI() - 1) {
			*fixed = true;
		} else if (m_setting.j >= sGrid2d->dimensionJ() - 1) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	} else if (sGrid3d != nullptr) {
		if (m_setting.i >= sGrid3d->dimensionI() - 1) {
			*fixed = true;
		} else if (m_setting.j >= sGrid3d->dimensionJ() - 1) {
			*fixed = true;
		} else if (m_setting.k >= sGrid3d->dimensionK() - 1) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	}
}

void PostStringResultArgument::fixEdgeIIndexIfNeeded(bool* fixed)
{
	auto z = zoneDataContainer();
	auto grid = z->gridData();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid->grid());

	*fixed = false;
	if (sGrid2d != nullptr) {
		if (m_setting.i >= sGrid2d->dimensionI()) {
			*fixed = true;
		} else if (m_setting.j >= sGrid2d->dimensionJ() - 1) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	} else if (sGrid3d != nullptr) {
		if (m_setting.i >= sGrid3d->dimensionI()) {
			*fixed = true;
		} else if (m_setting.j >= sGrid3d->dimensionJ() - 1) {
			*fixed = true;
		} else if (m_setting.k >= sGrid3d->dimensionK() - 1) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	}
}

void PostStringResultArgument::fixEdgeJIndexIfNeeded(bool* fixed)
{
	auto z = zoneDataContainer();
	auto grid = z->gridData();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*>(grid->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid->grid());

	*fixed = false;
	if (sGrid2d != nullptr) {
		if (m_setting.i >= sGrid2d->dimensionI() - 1) {
			*fixed = true;
		} else if (m_setting.j >= sGrid2d->dimensionJ()) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	} else if (sGrid3d != nullptr) {
		if (m_setting.i >= sGrid3d->dimensionI() - 1) {
			*fixed = true;
		} else if (m_setting.j >= sGrid3d->dimensionJ()) {
			*fixed = true;
		} else if (m_setting.k >= sGrid3d->dimensionK() - 1) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	}
}

void PostStringResultArgument::fixEdgeKIndexIfNeeded(bool* fixed)
{
	auto z = zoneDataContainer();
	auto grid = z->gridData();
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*>(grid->grid());

	*fixed = false;
	if (sGrid3d != nullptr) {
		if (m_setting.i >= sGrid3d->dimensionI() - 1) {
			*fixed = true;
		} else if (m_setting.j >= sGrid3d->dimensionJ() - 1) {
			*fixed = true;
		} else if (m_setting.k >= sGrid3d->dimensionK()) {
			*fixed = true;
		}
		if (*fixed) {
			m_setting.i = 0;
			m_setting.j = 0;
			m_setting.k = 0;
		}
	}
}

PostStringResult* PostStringResultArgument::result() const
{
	return dynamic_cast<PostStringResult*>(parent());
}

v4PostZoneDataContainer* PostStringResultArgument::zoneDataContainer() const
{
	return result()->zoneDataContainer();
}

PostStringResultArgument::Setting::Setting() :
	CompositeContainer ({&type, &name, &refName, &i, &j, &k, &index, &valueForTest}),
	type {"type"},
	name {"name"},
	refName {"refName"},
	i {"i"},
	j {"j"},
	k {"k"},
	index {"index"},
	valueForTest {"valueForTest"}
{}

PostStringResultArgument::Setting::Setting(const PostStringResultArgument::Setting& s) :
	Setting()
{
	copyValue(s);
}

PostStringResultArgument::Setting& PostStringResultArgument::Setting::operator=(const PostStringResultArgument::Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}
