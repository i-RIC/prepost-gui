#include "v4postcalculatedresult.h"
#include "v4postcalculatedresultargument.h"

#include <misc/stringtool.h>

v4PostCalculatedResultArgument::v4PostCalculatedResultArgument(v4PostCalculatedResult* result) :
	ProjectDataItem {result}
{}

std::string v4PostCalculatedResultArgument::name() const
{
	return iRIC::toStr(m_setting.name);
}

void v4PostCalculatedResultArgument::setName(const std::string& name)
{
	m_setting.name = name.c_str();
}

std::string v4PostCalculatedResultArgument::refName() const
{
	return iRIC::toStr(m_setting.refName);
}

void v4PostCalculatedResultArgument::setRefName(const std::string& name)
{
	m_setting.refName = name.c_str();
}

double v4PostCalculatedResultArgument::valueForTest() const
{
	return m_setting.valueForTest;
}

void v4PostCalculatedResultArgument::setValueForTest(double v)
{
	m_setting.valueForTest = v;
}

void v4PostCalculatedResultArgument::copyValue(const v4PostCalculatedResultArgument& arg)
{
	m_setting = arg.m_setting;
}

void v4PostCalculatedResultArgument::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
}

void v4PostCalculatedResultArgument::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

v4PostCalculatedResultArgument::Setting::Setting() :
	CompositeContainer ({&name, &refName, &valueForTest}),
	name {"name"},
	refName {"refName"},
	valueForTest {"valueForTest"}
{}

v4PostCalculatedResultArgument::Setting::Setting(const v4PostCalculatedResultArgument::Setting& s) :
	Setting()
{
	copyValue(s);
}

v4PostCalculatedResultArgument::Setting& v4PostCalculatedResultArgument::Setting::operator=(const v4PostCalculatedResultArgument::Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}
