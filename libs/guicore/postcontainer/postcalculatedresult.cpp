#include "postcalculatedresult.h"
#include "postcalculatedresultargument.h"
#include "postzonedatacontainer.h"

#include <misc/stringtool.h>

#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <QDomNode>
#include <QMessageBox>
#include <QXmlStreamWriter>

PostCalculatedResult::PostCalculatedResult(PostZoneDataContainer* container) :
	ProjectDataItem {container}
{}

PostCalculatedResult::~PostCalculatedResult()
{
	clearArguments();
}

std::string PostCalculatedResult::name() const
{
	return iRIC::toStr(m_setting.name);
}

void PostCalculatedResult::setName(const std::string& name)
{
	m_setting.name = name.c_str();
}

PostCalculatedResult::DataType PostCalculatedResult::dataType() const
{
	return m_setting.dataType;
}

void PostCalculatedResult::setDataType(DataType dt)
{
	m_setting.dataType = dt;
}

std::string PostCalculatedResult::polyDataName() const
{
	return iRIC::toStr(m_setting.polyDataName);
}

void PostCalculatedResult::setPolyDataName(const std::string& name)
{
	m_setting.polyDataName = name.c_str();
}

std::vector<PostCalculatedResultArgument*>& PostCalculatedResult::arguments()
{
	return m_arguments;
}

const std::vector<PostCalculatedResultArgument*>& PostCalculatedResult::arguments() const
{
	return m_arguments;
}

void PostCalculatedResult::clearArguments()
{
	for (auto a : m_arguments) {
		delete a;
	}
	m_arguments.clear();
}

const QString& PostCalculatedResult::script() const
{
	return m_script;
}

void PostCalculatedResult::setScript(const QString& s)
{
	m_script = s;
}

void PostCalculatedResult::updateValues()
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

double PostCalculatedResult::calculateValue(const std::vector<double>& args)
{
	QJSValueList jsArgs;
	for (auto a : args) {
		jsArgs << a;
	}
	return m_function.call(jsArgs).toNumber();
}

bool PostCalculatedResult::testDefinition(QWidget* w, double* calculatedValue)
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

bool PostCalculatedResult::checkArguments(const std::vector<PostCalculatedResult*>& calculatedResults, QWidget* w)
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

void PostCalculatedResult::copyValue(const PostCalculatedResult& result)
{
	m_setting = result.m_setting;
	m_script = result.m_script;

	clearArguments();
	for (auto a : result.m_arguments) {
		PostCalculatedResultArgument* copyArg = new PostCalculatedResultArgument(this);
		copyArg->copyValue(*a);
		m_arguments.push_back(copyArg);
	}
}

PostZoneDataContainer* PostCalculatedResult::zoneDataContainer() const
{
	return dynamic_cast<PostZoneDataContainer*> (parent());
}

vtkDataSetAttributes* PostCalculatedResult::dataSetAttributes() const
{
	DataType dt = m_setting.dataType.value();
	auto zdc = zoneDataContainer();

	switch (dt) {
	case GridNode:
		return zdc->data()->GetPointData();
	case GridCell:
		return zdc->data()->GetCellData();
	case Particle:
		return zdc->data()->GetPointData();
	case PolyData:
		// @todo implement this after mergeing polydata add branch
		return nullptr;
		break;
	}
}

vtkDataArray* PostCalculatedResult::dataSetAttributeArray(const std::string& name) const
{
	auto atts = dataSetAttributes();
	return atts->GetArray(name.c_str());
}

void PostCalculatedResult::updateFunction()
{
	m_function = buildFunction();
}

QJSValue PostCalculatedResult::buildFunction()
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

void PostCalculatedResult::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	auto children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		auto child = children.at(i);
		if (child.nodeName() == "Argument") {
			auto arg = new PostCalculatedResultArgument(this);
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

void PostCalculatedResult::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

PostCalculatedResult::Setting::Setting() :
	CompositeContainer({&name, &dataType, &polyDataName}),
	name {"name"},
	dataType {"dataType"},
	polyDataName {"polyDataName"}
{}

PostCalculatedResult::Setting::Setting(const PostCalculatedResult::Setting& s) :
	Setting()
{
	copyValue(s);
}

PostCalculatedResult::Setting& PostCalculatedResult::Setting::operator=(const PostCalculatedResult::Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}
