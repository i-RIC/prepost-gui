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
	InputConditionContainerReal(const std::string& n, const QString& c, const QDomNode& defNode);
	InputConditionContainerReal(const InputConditionContainerReal& i);
	~InputConditionContainerReal();

	InputConditionContainerReal& operator=(const InputConditionContainerReal& i);

	double value() const;
	void setValue(double v);

	double defaultValue() const;
	void setDefaultValue(double d);

	int load() override;
	int save() override;
	void clear() override;

	virtual QVariant variantValue() const override;

	void importFromYaml(const YAML::Node& doc, const QDir& dir) override;
	void exportToYaml(QTextStream* stream, const QDir& dir) override;

signals:
	void valueChanged(double newvalue);
	void valueChanged();

private:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerReal& i);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/inputconditioncontainerreal_impl.h"
#endif // _DEBUG

#endif
