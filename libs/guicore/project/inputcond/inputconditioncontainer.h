#ifndef CGNSFILEINPUTCONDITIONCONTAINER_H
#define CGNSFILEINPUTCONDITIONCONTAINER_H

#include "../../guicore_global.h"

#include <QString>
#include <QObject>
#include <QVariant>

#include <string>

class QTextStream;

namespace YAML {
	class Node;
} // namespace YAML

class GUICOREDLL_EXPORT InputConditionContainer : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	InputConditionContainer() {
		m_name = QString();
		m_isBoundaryCondition = false;
		m_isComplexCondition = false;
	}
	/// Constructor
	InputConditionContainer(const QString& name)
		: QObject(0) {
		m_name = name;
		m_isBoundaryCondition = false;
		m_isComplexCondition = false;
	}
	/// Constructor (with copying)
	InputConditionContainer(const InputConditionContainer& c)
		: QObject(0) {
		m_name = c.m_name;
		m_isBoundaryCondition = false;
		m_isComplexCondition = false;
	}
	void setBCProperty(const QString& bcname, int bcindex) {
		m_bcName = bcname;
		m_bcIndex = bcindex;
		m_isBoundaryCondition = true;
		m_isComplexCondition = false;
	}
	void setComplexProperty(const QString& compname, int compindex) {
		m_complexName = compname;
		m_complexIndex = compindex;
		m_isBoundaryCondition = false;
		m_isComplexCondition = true;
	}

	void setName(const QString& name) {m_name = name;}
	virtual void clear() = 0;
	virtual int load() = 0;
	virtual int save() = 0;
	virtual QVariant variantValue() const = 0;
	virtual void importFromYaml(const YAML::Node& doc) = 0;
	virtual void exportToYaml(QTextStream* stream) = 0;

protected:
	QString m_name;
	QString m_bcName;
	int m_bcIndex;
	QString m_complexName;
	int m_complexIndex;
	bool m_isBoundaryCondition;
	bool m_isComplexCondition;

signals:
	void valueChanged();
};
#endif // CGNSFILEINPUTCONDITIONCONTAINER_H
