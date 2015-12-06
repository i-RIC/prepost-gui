#ifndef INPUTCONDITIONCONTAINERINTEGER_H
#define INPUTCONDITIONCONTAINERINTEGER_H

#include "../../guicore_global.h"

#include "inputconditioncontainer.h"
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerInteger : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerInteger();
	InputConditionContainerInteger(const QString& n, const QString& c, const QDomNode& defNode);
	InputConditionContainerInteger(const InputConditionContainerInteger& i);
	~InputConditionContainerInteger();

	InputConditionContainerInteger& operator=(const InputConditionContainerInteger& i);

	int value() const;
	void setValue(int v);

	int defaultValue() const;
	void setDefaultValue(int d);

	int load() override;
	int save() override;
	void clear() override;

	QVariant variantValue() const override;

	void importFromYaml(const YAML::Node& doc, const QDir& dir) override;
	void exportToYaml(QTextStream* stream, const QDir& dir) override;

signals:
	void valueChanged(int newvalue);
	void valueChanged();

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerInteger& i);

private:
	int m_value;
	int m_default;
};

#endif // INPUTCONDITIONCONTAINERINTEGER_H
