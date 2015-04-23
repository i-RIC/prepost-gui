#ifndef CGNSFILEINPUTCONDITIONCONTAINER_H
#define CGNSFILEINPUTCONDITIONCONTAINER_H

#include "../../guicore_global.h"

#include <string>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>

class GUICOREDLL_EXPORT CgnsFileInputConditionContainer : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	CgnsFileInputConditionContainer(){
		m_name = QString();
		m_isBoundaryCondition = false;
		m_isComplexCondition = false;
	}
	/// Constructor
	CgnsFileInputConditionContainer(const QString& name)
		: QObject(0)
	{
		m_name = name;
		m_isBoundaryCondition = false;
		m_isComplexCondition = false;
	}
	/// Constructor (with copying)
	CgnsFileInputConditionContainer(const CgnsFileInputConditionContainer& c)
		: QObject(0)
	{
		m_name = c.m_name;
		m_isBoundaryCondition = false;
		m_isComplexCondition = false;
	}
	void setBCProperty(const QString& bcname, int bcindex){
		m_bcName = bcname;
		m_bcIndex = bcindex;
		m_isBoundaryCondition = true;
		m_isComplexCondition = false;
	}
	void setComplexProperty(const QString& compname, int compindex){
		m_complexName = compname;
		m_complexIndex = compindex;
		m_isBoundaryCondition = false;
		m_isComplexCondition = true;
	}

	void setName(const QString& name){m_name = name;}
	virtual void clear() = 0;
	virtual int load() = 0;
	virtual int save() = 0;
	virtual QVariant variantValue() = 0;
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
