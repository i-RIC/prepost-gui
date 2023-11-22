#ifndef V4POSTCALCULATEDRESULTARGUMENT_H
#define V4POSTCALCULATEDRESULTARGUMENT_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/stringcontainer.h>

#include <string>

class v4PostCalculatedResult;

class v4PostCalculatedResultArgument : public ProjectDataItem
{
public:
	v4PostCalculatedResultArgument(v4PostCalculatedResult* result);

	std::string name() const;
	void setName(const std::string& name);

	std::string refName() const;
	void setRefName(const std::string& name);

	double valueForTest() const;
	void setValueForTest(double v);

	void copyValue(const v4PostCalculatedResultArgument& arg);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Setting : public CompositeContainer
	{
	public:
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		StringContainer name;
		StringContainer refName;
		DoubleContainer valueForTest;
	};

	Setting m_setting;
};

#endif // V4POSTCALCULATEDRESULTARGUMENT_H
