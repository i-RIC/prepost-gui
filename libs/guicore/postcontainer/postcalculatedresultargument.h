#ifndef POSTCALCULATEDRESULTARGUMENT_H
#define POSTCALCULATEDRESULTARGUMENT_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/stringcontainer.h>

#include <string>

class PostCalculatedResult;

class PostCalculatedResultArgument : public ProjectDataItem
{
public:
	PostCalculatedResultArgument(PostCalculatedResult* result);

	std::string name() const;
	void setName(const std::string& name);

	std::string refName() const;
	void setRefName(const std::string& name);

	double valueForTest() const;
	void setValueForTest(double v);

	void copyValue(const PostCalculatedResultArgument& arg);

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

#endif // POSTCALCULATEDRESULTARGUMENT_H
