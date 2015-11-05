#ifndef CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H
#define CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H

#include "../../guicore_global.h"

#include "inputconditioncontainer.h"
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerInteger : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerInteger();
	/// Constructor
	InputConditionContainerInteger(QString n, const QDomNode& defNode);
	/// Constructor (with copying)
	InputConditionContainerInteger(const InputConditionContainerInteger& i);
	/// Copyer
	InputConditionContainerInteger& operator=(const InputConditionContainerInteger& i);
	void clear();
	int load() override;
	int save() override;
	QVariant variantValue() const override;
	void setValue(int v);
	int value() const;
	void setDefaultValue(int d);
	int defaultValue() const;
	void importFromYaml(const YAML::Node& doc, const QDir& dir) override;
	void exportToYaml(QTextStream* stream, const QDir& dir) override;

signals:
	void valueChanged(int newvalue);
	void valueChanged();

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerInteger& i);

private:
	int m_default;
	int m_value;
};

#endif // CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H
