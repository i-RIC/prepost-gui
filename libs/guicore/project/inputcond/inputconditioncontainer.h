#ifndef INPUTCONDITIONCONTAINER_H
#define INPUTCONDITIONCONTAINER_H

#include "../../guicore_global.h"

#include <QObject>

#include <string>

class QString;
class QDir;
class QTextStream;

namespace YAML {
	class Node;
} // namespace YAML

class GUICOREDLL_EXPORT InputConditionContainer : public QObject
{
	Q_OBJECT

public:
	InputConditionContainer();
	InputConditionContainer(const std::string& name, const QString& caption);
	InputConditionContainer(const InputConditionContainer& c);
	virtual ~InputConditionContainer();

	void setName(const std::string& name);
	void setBCProperty(const std::string& bcname, int bcindex);
	void setComplexProperty(const std::string& compname, int compindex);

	void copyValues(const InputConditionContainer& c);

	virtual int load() = 0;
	virtual int save() = 0;
	virtual void clear() = 0;

	virtual QVariant variantValue() const = 0;

	virtual void importFromYaml(const YAML::Node& doc, const QDir& dir) = 0;
	virtual void exportToYaml(QTextStream* stream, const QDir& dir) = 0;

signals:
	void valueChanged();

protected:
	const std::string& name() const;
	const QString& caption() const;

	bool isBoundaryCondition() const;
	const std::string& bcName() const;
	int bcIndex() const;

	bool isComplexCondition() const;
	const std::string& complexName() const;
	int complexIndex() const;

	static char* toC(const std::string& str);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/inputconditioncontainer_impl.h"
#endif // _DEBUG

#endif // INPUTCONDITIONCONTAINER_H
