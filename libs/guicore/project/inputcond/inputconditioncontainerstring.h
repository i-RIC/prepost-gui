#ifndef INPUTCONDITIONCONTAINERSTRING_H
#define INPUTCONDITIONCONTAINERSTRING_H

#include "../../guicore_global.h"
#include "inputconditioncontainer.h"

class QDomNode;
class QString;

class GUICOREDLL_EXPORT InputConditionContainerString : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerString();
	InputConditionContainerString(const std::string& n, const QString& c, const QDomNode& defNode);
	InputConditionContainerString(const InputConditionContainerString& i);
	~InputConditionContainerString();

	InputConditionContainerString& operator=(const InputConditionContainerString& i);

	const QString& value() const;
	void setValue(const QString& v);

	const QString& defaultValue() const;
	void setDefaultValue(const QString& v);

	int load(const iRICLib::H5CgnsConditionGroup& group) override;
	int save(iRICLib::H5CgnsConditionGroup* group) override;
	void clear() override;

	virtual QVariant variantValue() const override;

	void importFromYaml(const YAML::Node& doc, const QDir& dir);
	void exportToYaml(QTextStream* stream, const QDir& dir);
	void importFromString(const QString& value, const QDir& dir) override;
	void exportToString(QString* value, const QDir& dir) override;

signals:
	void valueChanged(const QString& newvalue);
	void valueChanged();

private:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerString& i);

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/inputconditioncontainerstring_impl.h"
#endif // _DEBUG

#endif // INPUTCONDITIONCONTAINERSTRING_H
