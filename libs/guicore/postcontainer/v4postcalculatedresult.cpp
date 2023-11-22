#include "../grid/v4grid.h"
#include "../grid/v4structured2dgrid.h"
#include "../grid/v4structured3dgrid.h"
#include "v4postcalculatedresult.h"
#include "v4postcalculatedresultargument.h"
#include "v4postzonedatacontainer.h"
#include "v4solutiongrid.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>
#include <misc/stringtool.h>

#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <QDomNode>
#include <QMessageBox>
#include <QXmlStreamWriter>

v4PostCalculatedResult::v4PostCalculatedResult(v4PostZoneDataContainer* container) :
	ProjectDataItem {container}
{}

v4PostCalculatedResult::~v4PostCalculatedResult()
{
	clearArguments();
}

std::string v4PostCalculatedResult::name() const
{
	return iRIC::toStr(m_setting.name);
}

void v4PostCalculatedResult::setName(const std::string& name)
{
	m_setting.name = name.c_str();
}

v4PostCalculatedResult::DataType v4PostCalculatedResult::dataType() const
{
	return m_setting.dataType;
}

void v4PostCalculatedResult::setDataType(DataType dt)
{
	m_setting.dataType = dt;
}

std::string v4PostCalculatedResult::polyDataName() const
{
	return iRIC::toStr(m_setting.polyDataName);
}

void v4PostCalculatedResult::setPolyDataName(const std::string& name)
{
	m_setting.polyDataName = name.c_str();
}

std::vector<v4PostCalculatedResultArgument*>& v4PostCalculatedResult::arguments()
{
	return m_arguments;
}

const std::vector<v4PostCalculatedResultArgument*>& v4PostCalculatedResult::arguments() const
{
	return m_arguments;
}

void v4PostCalculatedResult::clearArguments()
{
	for (auto a : m_arguments) {
		delete a;
	}
	m_arguments.clear();
}

const QString& v4PostCalculatedResult::script() const
{
	return m_script;
}

void v4PostCalculatedResult::setScript(const QString& s)
{
	m_script = s;
}

void v4PostCalculatedResult::updateValues()
{
	auto f = buildFunction();

	std::vector<vtkDataArray*> args;
	for (auto a : m_arguments) {
		args.push_back(dataSetAttributeArray(a->name()));
	}
	auto vals = dataSetAttributeArray(iRIC::toStr(m_setting.name));

	for (vtkIdType i = 0; i < vals->GetNumberOfTuples(); ++i) {
		QJSValueList jsArgs;
		for (auto a : args) {
			jsArgs << *(a->GetTuple(i));
		}
		double val = f.call(jsArgs).toNumber();
		vals->SetTuple(i, &val);
	}
}

double v4PostCalculatedResult::calculateValue(const std::vector<double>& args)
{
	QJSValueList jsArgs;
	for (auto a : args) {
		jsArgs << a;
	}
	return m_function.call(jsArgs).toNumber();
}

bool v4PostCalculatedResult::testDefinition(QWidget* w, double* calculatedValue)
{
	auto f = buildFunction();
	if (f.isError()) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured: %1").arg(f.toString()));
		return false;
	}

	QJSValueList args;
	for (auto arg : m_arguments) {
		args << arg->valueForTest();
	}
	QJSValue val = f.call(args);

	if (val.isError()) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured: %1").arg(val.toString()));
		return false;
	}
	if (calculatedValue != nullptr) {
		*calculatedValue = val.toNumber();
	}
	return true;
}

bool v4PostCalculatedResult::checkArguments(const std::vector<v4PostCalculatedResult*>& calculatedResults, QWidget* w)
{
	QStringList invalids;
	for (auto arg : m_arguments) {
		if (dataSetAttributeArray(arg->name()) != nullptr) {continue;}
		bool found = false;
		for (auto r : calculatedResults) {
			if (arg->name() == r->name() && dataType() == r->dataType()) {
				found = true;
				break;
			}
		}
		if (found) {continue;}

		invalids.push_back(arg->name().c_str());
	}
	if (invalids.size() == 0) {return true;}

	QMessageBox::critical(w, tr("Error"), tr("Checking argument for %1 failed. Invalid arguments: %2.").arg(name().c_str()).arg(invalids.join(", ")));
	return false;
}

void v4PostCalculatedResult::copyValue(const v4PostCalculatedResult& result)
{
	m_setting = result.m_setting;
	m_script = result.m_script;

	clearArguments();
	for (auto a : result.m_arguments) {
		v4PostCalculatedResultArgument* copyArg = new v4PostCalculatedResultArgument(this);
		copyArg->copyValue(*a);
		m_arguments.push_back(copyArg);
	}
}

v4PostZoneDataContainer* v4PostCalculatedResult::zoneDataContainer() const
{
	return dynamic_cast<v4PostZoneDataContainer*> (parent());
}

vtkDataSetAttributes* v4PostCalculatedResult::dataSetAttributes() const
{
	DataType dt = m_setting.dataType.value();
	auto zdc = zoneDataContainer();
	if (zdc == nullptr) {return nullptr;}

	auto grid = zdc->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (zdc->gridData()->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (zdc->gridData()->grid());

	switch (dt) {
	case GridNode:
		return grid->vtkData()->data()->GetPointData();
	case GridCell:
		return grid->vtkData()->data()->GetCellData();
	case GridIEdge:
		if (sGrid2d != nullptr) {
			return sGrid2d->vtkIEdgeData()->data()->GetCellData();
		} else if (sGrid3d != nullptr) {
			return sGrid3d->vtkIFaceData()->data()->GetCellData();
		} else {
			return nullptr;
		}
	case GridJEdge:
		if (sGrid2d != nullptr) {
			return sGrid2d->vtkJEdgeData()->data()->GetCellData();
		} else if (sGrid3d != nullptr) {
			return sGrid3d->vtkJFaceData()->data()->GetCellData();
		} else {
			return nullptr;
		}
	case GridKEdge:
		if (sGrid3d != nullptr) {
			return sGrid3d->vtkKFaceData()->data()->GetCellData();
		} else {
			return nullptr;
		}
	case Particle:
		if (zdc->particleData() != nullptr) {
			return zdc->particleData()->grid()->vtkData()->data()->GetPointData();
		} else {
			return nullptr;
		}
	case PolyData:
		// @todo implement this after mergeing polydata add branch
		return nullptr;
		break;
	}
	// to avoid compiler warning
	return nullptr;
}

vtkIdType v4PostCalculatedResult::dataSetAttributeValueCount() const
{
	DataType dt = m_setting.dataType.value();
	auto zdc = zoneDataContainer();
	if (zdc == nullptr) {return 0;}

	auto grid = zdc->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (zdc->gridData()->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (zdc->gridData()->grid());

	switch (dt) {
	case GridNode:
		return grid->vtkData()->data()->GetNumberOfPoints();
	case GridCell:
		return grid->vtkData()->data()->GetNumberOfCells();
	case GridIEdge:
		if (sGrid2d != nullptr) {
			return sGrid2d->vtkIEdgeData()->data()->GetNumberOfCells();
		} else if (sGrid3d != nullptr) {
			return sGrid3d->vtkIFaceData()->data()->GetNumberOfCells();
		} else {
			return 0;
		}
	case GridJEdge:
		if (sGrid2d != nullptr) {
			return sGrid2d->vtkJEdgeData()->data()->GetNumberOfCells();
		} else if (sGrid3d != nullptr) {
			return sGrid3d->vtkJFaceData()->data()->GetNumberOfCells();
		} else {
			return 0;
		}
	case GridKEdge:
		if (sGrid3d != nullptr) {
			return sGrid3d->vtkKFaceData()->data()->GetNumberOfCells();
		} else {
			return 0;
		}
	case Particle:
		if (zdc->particleData() != nullptr) {
			return zdc->particleData()->grid()->vtkData()->data()->GetNumberOfPoints();
		} else {
			return 0;
		}
	case PolyData:
		// @todo implement this after mergeing polydata add branch
		return 0;
		break;
	}
	// to avoid compiler warning
	return 0;
}

vtkDataArray* v4PostCalculatedResult::dataSetAttributeArray(const std::string& name) const
{
	auto atts = dataSetAttributes();
	return atts->GetArray(name.c_str());
}

void v4PostCalculatedResult::updateFunction()
{
	m_function = buildFunction();
}

QJSValue v4PostCalculatedResult::buildFunction()
{
	QStringList refNames;
	for (auto arg : m_arguments) {
		refNames.push_back(arg->refName().c_str());
	}
	QString funcStr;
	funcStr.append("(function(").append(refNames.join(",").append(") {"));
	funcStr.append(m_script);
	funcStr.append("})");
	return m_jsEngine.evaluate(funcStr);
}

void v4PostCalculatedResult::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	auto children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		auto child = children.at(i);
		if (child.nodeName() == "Argument") {
			auto arg = new v4PostCalculatedResultArgument(this);
			arg->loadFromProjectMainFile(child);
			m_arguments.push_back(arg);
		} else if (child.nodeName() == "Script") {
			auto scriptChildren = child.childNodes();
			for (int j = 0; j < scriptChildren.size(); ++j) {
				auto sc = scriptChildren.at(j);
				m_script = sc.nodeValue();
			}
		}
	}
}

void v4PostCalculatedResult::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
	for (auto a : m_arguments) {
		writer.writeStartElement("Argument");
		a->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeStartElement("Script");
	writer.writeCharacters(m_script);
	writer.writeEndElement();
}

v4PostCalculatedResult::Setting::Setting() :
	CompositeContainer({&name, &dataType, &polyDataName}),
	name {"name"},
	dataType {"dataType"},
	polyDataName {"polyDataName"}
{}

v4PostCalculatedResult::Setting::Setting(const v4PostCalculatedResult::Setting& s) :
	Setting()
{
	copyValue(s);
}

v4PostCalculatedResult::Setting& v4PostCalculatedResult::Setting::operator=(const v4PostCalculatedResult::Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}
