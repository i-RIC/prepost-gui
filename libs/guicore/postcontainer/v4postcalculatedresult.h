#ifndef V4POSTCALCULATEDRESULT_H
#define V4POSTCALCULATEDRESULT_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/stringcontainer.h>

#include <QJSEngine>

#include <string>
#include <vector>

class v4PostCalculatedResultArgument;
class v4PostZoneDataContainer;

class vtkDataArray;
class vtkDataSetAttributes;

class QJSValue;
class QWidget;

class v4PostCalculatedResult : public ProjectDataItem
{
public:
	enum DataType {
		GridNode,
		GridCell,
		GridIEdge,
		GridJEdge,
		GridKEdge,
		Particle,
		PolyData
	};

	v4PostCalculatedResult(v4PostZoneDataContainer* container);
	~v4PostCalculatedResult() override;

	std::string name() const;
	void setName(const std::string& name);

	DataType dataType() const;
	void setDataType(DataType dt);

	std::string polyDataName() const;
	void setPolyDataName(const std::string& name);

	std::vector<v4PostCalculatedResultArgument*>& arguments();
	const std::vector<v4PostCalculatedResultArgument*>& arguments() const;
	void clearArguments();

	const QString& script() const;
	void setScript(const QString& s);

	void updateValues();
	double calculateValue(const std::vector<double>& args);

	bool testDefinition(QWidget* w, double* calculatedValue = nullptr);
	bool checkArguments(const std::vector<v4PostCalculatedResult*>& calculatedResults, QWidget* w);

	void copyValue(const v4PostCalculatedResult& result);
	vtkDataSetAttributes* dataSetAttributes() const;
	vtkIdType dataSetAttributeValueCount() const;
	vtkDataArray* dataSetAttributeArray(const std::string& name) const;
	void updateFunction();

private:
	v4PostZoneDataContainer* zoneDataContainer() const;

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

	std::vector<v4PostCalculatedResultArgument*> m_arguments;
	QString m_script;

	QJSValue m_function;
	QJSEngine m_jsEngine;
};

#endif // V4POSTCALCULATEDRESULT_H
