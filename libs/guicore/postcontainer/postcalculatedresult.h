#ifndef POSTCALCULATEDRESULT_H
#define POSTCALCULATEDRESULT_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/stringcontainer.h>

#include <QJSEngine>

#include <string>
#include <vector>

class PostCalculatedResultArgument;
class PostZoneDataContainer;

class vtkDataArray;
class vtkDataSetAttributes;

class QJSValue;
class QWidget;

class PostCalculatedResult : public ProjectDataItem
{
public:
	enum DataType {
		GridNode,
		GridCell,
		Particle,
		PolyData
	};

	PostCalculatedResult(PostZoneDataContainer* container);
	~PostCalculatedResult();

	std::string name() const;
	void setName(const std::string& name);

	DataType dataType() const;
	void setDataType(DataType dt);

	std::string polyDataName() const;
	void setPolyDataName(const std::string& name);

	std::vector<PostCalculatedResultArgument*>& arguments();
	const std::vector<PostCalculatedResultArgument*>& arguments() const;
	void clearArguments();

	const QString& script() const;
	void setScript(const QString& s);

	void updateValues();
	double calculateValue(const std::vector<double>& args);

	bool testDefinition(QWidget* w, double* calculatedValue = nullptr);
	bool checkArguments(const std::vector<PostCalculatedResult*>& calculatedResults, QWidget* w);

	void copyValue(const PostCalculatedResult& result);
	vtkDataSetAttributes* dataSetAttributes() const;
	vtkDataArray* dataSetAttributeArray(const std::string& name) const;
	void updateFunction();

private:
	PostZoneDataContainer* zoneDataContainer() const;

	QJSValue buildFunction();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Setting : public CompositeContainer
	{
	public:
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		StringContainer name;
		EnumContainerT<DataType> dataType;
		StringContainer polyDataName;
	};

	Setting m_setting;

	std::vector<PostCalculatedResultArgument*> m_arguments;
	QString m_script;

	QJSValue m_function;
	QJSEngine m_jsEngine;
};

#endif // POSTCALCULATEDRESULT_H
