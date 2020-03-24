#ifndef POSTSTRINGRESULT_H
#define POSTSTRINGRESULT_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

#include <QJSEngine>

#include <vector>

class PostStringResultArgument;
class PostZoneDataContainer;

class QJSValue;
class QString;

class GUICOREDLL_EXPORT PostStringResult : public ProjectDataItem
{
	Q_OBJECT

public:
	PostStringResult(ProjectDataItem* parent);
	~PostStringResult() override;


	std::vector<PostStringResultArgument*>& arguments();
	const std::vector<PostStringResultArgument*>& arguments() const;
	void removeArgument(int index);
	void clearArguments();

	const QString& script();
	void setScript(const QString& script);

	QString value() const;
	void updateValue();
	bool testDefinition(QWidget* w, QString* testResult = nullptr);
	void updateFunction();

	void copyValue(const PostStringResult &result);

	PostZoneDataContainer* zoneDataContainer() const;
	void setZoneDataContainer(PostZoneDataContainer* container, QWidget* parent);

private:
	QJSValue buildFunction();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	PostZoneDataContainer* m_container;
	std::vector<PostStringResultArgument*> m_arguments;
	QString m_script;

	QString m_value;
	QJSValue m_function;
	QJSEngine m_jsEngine;
};

#endif // POSTSTRINGRESULT_H
