#ifndef INPUTCONDITIONCONTAINERSTRING_H
#define INPUTCONDITIONCONTAINERSTRING_H

#include "../../guicore_global.h"
#include "inputconditioncontainer.h"
#include <QString>
class QDomNode;

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

	int load() override;
	int save() override;
	void clear() override;

	virtual QVariant variantValue() const override;

	void importFromYaml(const YAML::Node& doc, const QDir& dir);
	void exportToYaml(QTextStream* stream, const QDir& dir);

signals:
	void valueChanged(const QString& newvalue);
	void valueChanged();

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerString& i);

private:
	QString m_value;
	QString m_default;
};

#endif // INPUTCONDITIONCONTAINERSTRING_H
