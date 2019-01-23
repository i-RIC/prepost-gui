#include "postcalculatedresult.h"
#include "postcalculatedresultargument.h"

#include <misc/stringtool.h>

PostCalculatedResultArgument::PostCalculatedResultArgument(PostCalculatedResult* result) :
	ProjectDataItem {result}
{}

std::string PostCalculatedResultArgument::name() const
{
	return iRIC::toStr(m_setting.name);
}

void PostCalculatedResultArgument::setName(const std::string& name)
{
	m_setting.name = name.c_str();
}

std::string PostCalculatedResultArgument::refName() const
{
	return iRIC::toStr(m_setting.refName);
}

void PostCalculatedResultArgument::setRefName(const std::string& name)
{
	m_setting.refName = name.c_str();
}

double PostCalculatedResultArgument::valueForTest() const
{
	return m_setting.valueForTest;
}

void PostCalculatedResultArgument::setValueForTest(double v)
{
	m_setting.valueForTest = v;
}

void PostCalculatedResultArgument::copyValue(const PostCalculatedResultArgument& arg)
{
	m_setting = arg.m_setting;
}

void PostCalculatedResultArgument::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
}

void PostCalculatedResultArgument::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

PostCalculatedResultArgument::Setting::Setting() :
	CompositeContainer ({&name, &refName, &valueForTest}),
	name {"name"},
	refName {"refName"},
	valueForTest {"valueForTest"}
{}

PostCalculatedResultArgument::Setting::Setting(const PostCalculatedResultArgument::Setting& s) :
	Setting()
{
	copyValue(s);
}

PostCalculatedResultArgument::Setting& PostCalculatedResultArgument::Setting::operator=(const PostCalculatedResultArgument::Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}
