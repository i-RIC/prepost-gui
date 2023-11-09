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
	InputConditionContainerInteger(const std::string& n, const QString& c, const QDomNode& defNode);
	InputConditionContainerInteger(const InputConditionContainerInteger& i);
	~InputConditionContainerInteger();

	InputConditionContainerInteger& operator=(const InputConditionContainerInteger& i);

	int value() const;
	void setValue(int v);

	int defaultValue() const;
	void setDefaultValue(int d);

	int load(const iRICLib::H5CgnsConditionGroup& group) override;
	int save(iRICLib::H5CgnsConditionGroup* group) override;
	void clear() override;

	QVariant variantValue() const override;

	void importFromYaml(const YAML::Node& doc, const QDir& dir) override;
	void exportToYaml(QTextStream* stream, const QDir& dir) override;

signals:
	void valueChanged(int newvalue);
	void valueChanged();

private:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerInteger& i);

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/inputconditioncontainerinteger_impl.h"
#endif // _DEBUG

#endif // INPUTCONDITIONCONTAINERINTEGER_H
