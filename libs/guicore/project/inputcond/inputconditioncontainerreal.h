#ifndef CALCCOND_CONT_REAL_H
#define CALCCOND_CONT_REAL_H

#include "../../guicore_global.h"

#include "inputconditioncontainer.h"
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerReal : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerReal();
	InputConditionContainerReal(QString n, const QDomNode& defNode);
	InputConditionContainerReal(const InputConditionContainerReal& i);
	InputConditionContainerReal& operator=(const InputConditionContainerReal& i);
	void clear();
	int load() override;
	int save() override;
	virtual QVariant variantValue() const override;
	void setValue(double v);
	double value() const;
	void setDefaultValue(double d);
	double defaultValue() const;
	void importFromYaml(const YAML::Node& doc, const QDir& dir) override;
	void exportToYaml(QTextStream* stream, const QDir& dir) override;

signals:
	void valueChanged(double newvalue);
	void valueChanged();

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerReal& i);

private:
	double m_default;
	double m_value;
};

#endif
