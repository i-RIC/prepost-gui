#include "postbaseiterativenumericaldatacontainer.h"
#include "postbaseiterativestringdatacontainer.h"
#include "postsolutioninfo.h"
#include "poststringresult.h"
#include "poststringresultargument.h"
#include "postzonedatacontainer.h"

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
	auto pd = z->data()->GetPointData();
	int index = arrayIndex();
	vtkDataArray* da = pd->GetArray(name().c_str());
	return *(da->GetTuple(index));
}

QJSValue PostStringResultArgument::cellJsValue() const
{
	auto z = zoneDataContainer();
	auto pd = z->data()->GetCellData();
	int index = arrayIndex();
	vtkDataArray* da = pd->GetArray(name().c_str());
	return *(da->GetTuple(index));
}

QJSValue PostStringResultArgument::edgeIJsValue() const
{
	// @todo implement this
	return 0;
}

QJSValue PostStringResultArgument::edgeJJsValue() const
{
	// @todo implement this
	return 0;
}

QJSValue PostStringResultArgument::edgeKJsValue() const
{
	// @todo implement this
	return 0;
}

int PostStringResultArgument::arrayIndex() const
{
	auto z = zoneDataContainer();
	auto pointSet = z->data();
	auto st = dynamic_cast<vtkStructuredGrid*>(pointSet);
	auto ust = dynamic_cast<vtkUnstructuredGrid*>(pointSet);

	if (ust != nullptr) {
		// unstructured grid
		return m_setting.index;
	} else if (st != nullptr) {
		auto t = m_setting.type.value();
		if (t == Type::GridNode) {
			return z->nodeIndex(m_setting.i, m_setting.j, m_setting.k);
		} else if (t == Type::GridNode) {
			return z->cellIndex(m_setting.i, m_setting.j, m_setting.k);
		} else if (t == Type::GridEdgeI) {
			// @todo implement this
			return 0;
		} else if (t == Type::GridEdgeJ) {
			// @todo implement this
			return 0;
		} else if (t == Type::GridEdgeK) {
			// @todo implement this
			return 0;
		}
	}
	// just for avoiding compiler warning
	return 0;
}

PostStringResult* PostStringResultArgument::result() const
{
	return dynamic_cast<PostStringResult*>(parent());
}

PostZoneDataContainer* PostStringResultArgument::zoneDataContainer() const
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
