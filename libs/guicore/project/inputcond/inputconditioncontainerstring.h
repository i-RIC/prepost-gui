#ifndef CGNSFILEINPUTCONDITIONCONTAINERSTRING_H
#define CGNSFILEINPUTCONDITIONCONTAINERSTRING_H

#include "../../guicore_global.h"
#include "inputconditioncontainer.h"
#include <QString>
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerString : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerString();
	InputConditionContainerString(QString n, const QDomNode& defNode);
	InputConditionContainerString(const InputConditionContainerString& i);
	InputConditionContainerString& operator=(const InputConditionContainerString& i);
	void clear();
	int load() override;
	int save() override;
	virtual QVariant variantValue() const override;
	void setValue(const QString& v);
	const QString& value() const;
	void importFromYaml(const YAML::Node& doc);
	void exportToYaml(QTextStream* stream);

signals:
	void valueChanged(const QString& newvalue);
	void valueChanged();

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerString& i);

private:
	QString m_default;
	QString m_value;
};

#endif // CGNSFILEINPUTCONDITIONCONTAINERSTRING_H
