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

namespace iRICLib {
	class H5CgnsConditionGroup;
} // namespace iRICLib

class GUICOREDLL_EXPORT InputConditionContainer : public QObject
{
	Q_OBJECT

public:
	InputConditionContainer();
	InputConditionContainer(const std::string& name, const QString& caption);
	InputConditionContainer(const InputConditionContainer& c);
	virtual ~InputConditionContainer();

	void setName(const std::string& name);

	void copyValues(const InputConditionContainer& c);

	virtual int load(const iRICLib::H5CgnsConditionGroup& group) = 0;
	virtual int save(iRICLib::H5CgnsConditionGroup* group) = 0;
	virtual void clear() = 0;

	virtual QVariant variantValue() const = 0;

	virtual void importFromYaml(const YAML::Node& doc, const QDir& dir) = 0;
	virtual void exportToYaml(QTextStream* stream, const QDir& dir) = 0;
	virtual void importFromString(const QString& value, const QDir& dir) = 0;
	virtual void exportToString(QString* value, const QDir& dir) = 0;

signals:
	void valueChanged();

protected:
	const std::string& name() const;
	const QString& caption() const;

	static char* toC(const std::string& str);

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/inputconditioncontainer_impl.h"
#endif // _DEBUG

#endif // INPUTCONDITIONCONTAINER_H
