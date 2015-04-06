#ifndef CGNSFILEINPUTCONDITIONCONTAINERSTRING_H
#define CGNSFILEINPUTCONDITIONCONTAINERSTRING_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditioncontainer.h"
#include <QString>
class QDomNode;

class GUICOREDLL_EXPORT CgnsFileInputConditionContainerString : public CgnsFileInputConditionContainer
{
	Q_OBJECT

public:
	CgnsFileInputConditionContainerString() : CgnsFileInputConditionContainer(){}
	CgnsFileInputConditionContainerString(QString n, const QDomNode& defNode) : CgnsFileInputConditionContainer(n){
		setup(defNode);
	}
	CgnsFileInputConditionContainerString(const CgnsFileInputConditionContainerString &i) : CgnsFileInputConditionContainer(i){
		copyValues(i);
	}
	CgnsFileInputConditionContainerString &operator=(const CgnsFileInputConditionContainerString &i){
		copyValues(i);
		return *this;
	}
	void clear(){
		m_value = m_default;
		emit valueChanged(m_value);
	}
	int load();
	int save();
	virtual QVariant variantValue(){return QVariant(m_value);}
	void setValue(const QString& v){
		if (m_value != v){
			m_value = v;
			emit valueChanged(m_value);
			emit valueChanged();
		}
	}
	QString value(){return m_value;}

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const CgnsFileInputConditionContainerString& i){
		m_name = i.m_name;
		m_value = i.m_value;
		m_default = i.m_default;
	}
	QString m_default;
	QString m_value;

signals:
	void valueChanged(const QString& newvalue);
	void valueChanged();
};

#endif // CGNSFILEINPUTCONDITIONCONTAINERSTRING_H
