#include "poststringresult.h"
#include "poststringresultargument.h"
#include "postzonedatacontainer.h"

#include <misc/stringtool.h>

#include <QDomElement>
#include <QMessageBox>
#include <QXmlStreamWriter>

PostStringResult::PostStringResult(ProjectDataItem* parent) :
	ProjectDataItem {parent},
	m_script{ "return \"\";" }
{
	updateFunction();
}

PostStringResult::~PostStringResult()
{
	clearArguments();
}

std::vector<PostStringResultArgument*>& PostStringResult::arguments()
{
	return m_arguments;
}

const std::vector<PostStringResultArgument*>& PostStringResult::arguments() const
{
	return m_arguments;
}

void PostStringResult::removeArgument(int index)
{
	if (index < 0 || index >= m_arguments.size()) {return;}
	auto arg = m_arguments.at(index);
	delete arg;
	m_arguments.erase(m_arguments.begin() + index);
}

void PostStringResult::clearArguments()
{
	for (auto a : m_arguments) {
		delete a;
	}
	m_arguments.clear();
}

const QString& PostStringResult::script()
{
	return m_script;
}

void PostStringResult::setScript(const QString& script)
{
	m_script = script;
}

QString PostStringResult::value() const
{
	return m_value;
}

void PostStringResult::updateValue()
{
	QJSValueList jsArgs;
	for (auto a : m_arguments) {
		jsArgs << a->jsValue();
	}
	m_value = m_function.call(jsArgs).toString();
}

bool PostStringResult::testDefinition(QWidget* w, QString* testResult)
{
	auto f = buildFunction();
	if (f.isError()) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured: %1").arg(f.toString()));
		return false;
	}

	QJSValueList jsArgs;
	for (auto a : m_arguments) {
		jsArgs << m_jsEngine.evaluate(a->valueForTest());
	}
	QJSValue val = f.call(jsArgs);

	if (val.isError()) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured: %1").arg(val.toString()));
		return false;
	}
	if (testResult != nullptr) {
		*testResult = val.toString();
	}
	return true;
}

void PostStringResult::updateFunction()
{
	m_function = buildFunction();
}

void PostStringResult::copyValue(const PostStringResult& result)
{
	m_script = result.m_script;

	clearArguments();
	for (auto a : result.m_arguments) {
		PostStringResultArgument* copyArg = new PostStringResultArgument(this);
		copyArg->copyValue(*a);
		m_arguments.push_back(copyArg);
	}
	updateFunction();
}

PostZoneDataContainer* PostStringResult::zoneDataContainer() const
{
	return m_container;
}

void PostStringResult::setZoneDataContainer(PostZoneDataContainer* container, QWidget* parent)
{
	m_container = container;
	bool fixed = false;
	for (auto a : m_arguments) {
		bool tmpFixed = false;
		a->fixIndexIfNeeded(&tmpFixed);
		fixed = fixed || tmpFixed;
	}
	if (fixed) {
		QMessageBox::warning(parent, tr("Warning"), tr("I, J, K, index for label is reset because the grid size is changed."));
	}
}

QJSValue PostStringResult::buildFunction()
{
	QStringList refNames;
	for (auto a : m_arguments) {
		refNames.push_back(a->refName().c_str());
	}
	QString funcStr;
	funcStr.append("(function(").append(refNames.join(",").append(") {"));
	funcStr.append(m_script);
	funcStr.append("})");
	return m_jsEngine.evaluate(funcStr);
}

void PostStringResult::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		auto c = children.at(i);
		if (c.nodeName() == "Argument") {
			auto a = new PostStringResultArgument(this);
			a->loadFromProjectMainFile(c);
			m_arguments.push_back(a);
		} else if (c.nodeName() == "Script") {
			auto scriptChildren = c.childNodes();
			for (int j = 0; j < scriptChildren.size(); ++j) {
				auto sc = scriptChildren.at(j);
				m_script = sc.nodeValue();
			}
		}
	}
	updateFunction();
}

void PostStringResult::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto a : m_arguments) {
		writer.writeStartElement("Argument");
		a->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeStartElement("Script");
	writer.writeCharacters(m_script);
	writer.writeEndElement();
}
