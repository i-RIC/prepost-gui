#ifndef POSTSTRINGRESULTARGUMENT_H
#define POSTSTRINGRESULTARGUMENT_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>

class PostStringResult;
class PostZoneDataContainer;

class QJSValue;

class PostStringResultArgument : public ProjectDataItem
{
	Q_OBJECT

public:
	enum class Type {
		BaseIterative,
		GridNode,
		GridCell,
		GridEdgeI,
		GridEdgeJ,
		GridEdgeK
	};

	class Setting : public CompositeContainer
	{
	public:
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		EnumContainerT<Type> type;
		StringContainer name;
		StringContainer refName;

		IntContainer i;
		IntContainer j;
		IntContainer k;
		IntContainer index;

		StringContainer valueForTest;
	};

	PostStringResultArgument(PostStringResult* result);

	Type type() const;
	void setType(Type type);

	std::string name() const;
	void setName(const std::string& name);

	std::string refName() const;
	void setRefName(const std::string& name);

	int i() const;
	void setI(int i);

	int j() const;
	void setJ(int j);

	int k() const;
	void setK(int k);

	int index() const;
	void setIndex(int index);

	QString valueForTest() const;
	void setValueForTest(const QString& v);

	QJSValue jsValue() const;

	void copyValue(const PostStringResultArgument& arg);

	Setting setting() const;
	void setSetting(const Setting& s);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QJSValue baseIterativeJsValue() const;
	QJSValue nodeJsValue() const;
	QJSValue cellJsValue() const;
	QJSValue edgeIJsValue() const;
	QJSValue edgeJJsValue() const;
	QJSValue edgeKJsValue() const;
	int arrayIndex() const;

	PostStringResult* result() const;
	PostZoneDataContainer* zoneDataContainer() const;

	Setting m_setting;
};

#endif // POSTSTRINGRESULTARGUMENT_H
