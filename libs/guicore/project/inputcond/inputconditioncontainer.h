#ifndef CGNSFILEINPUTCONDITIONCONTAINER_H
#define CGNSFILEINPUTCONDITIONCONTAINER_H

#include "../../guicore_global.h"

#include <QString>
#include <QObject>
#include <QVariant>

#include <string>

class QDir;
class QTextStream;

namespace YAML {
	class Node;
} // namespace YAML

class GUICOREDLL_EXPORT InputConditionContainer : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	InputConditionContainer();
	/// Constructor
	InputConditionContainer(const QString& name, const QString& caption);
	/// Constructor (with copying)
	InputConditionContainer(const InputConditionContainer& c);
	void setBCProperty(const QString& bcname, int bcindex);
	void setComplexProperty(const QString& compname, int compindex);
	void setName(const QString& name);
	void copyValues(const InputConditionContainer& c);
	virtual void clear() = 0;
	virtual int load() = 0;
	virtual int save() = 0;
	virtual QVariant variantValue() const = 0;
	virtual void importFromYaml(const YAML::Node& doc, const QDir& dir) = 0;
	virtual void exportToYaml(QTextStream* stream, const QDir& dir) = 0;

protected:
	const QString& name() const;
	const QString& bcName() const;
	const QString& caption() const;

	int m_bcIndex;
	QString m_complexName;
	int m_complexIndex;
	bool m_isBoundaryCondition;
	bool m_isComplexCondition;

private:
	QString m_name;
	QString m_bcName;
	QString m_caption;

signals:
	void valueChanged();
};
#endif // CGNSFILEINPUTCONDITIONCONTAINER_H
