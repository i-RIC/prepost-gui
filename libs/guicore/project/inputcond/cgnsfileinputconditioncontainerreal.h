#ifndef CALCCOND_CONT_REAL_H
#define CALCCOND_CONT_REAL_H

#include "../../guicore_global.h"

#include "cgnsfileinputconditioncontainer.h"
class QDomNode;

class GUICOREDLL_EXPORT CgnsFileInputConditionContainerReal : public CgnsFileInputConditionContainer
{
	Q_OBJECT
public:
	CgnsFileInputConditionContainerReal() : CgnsFileInputConditionContainer() {
		m_value = 0;
		m_default = 0;
	}
	CgnsFileInputConditionContainerReal(QString n, const QDomNode& defNode) : CgnsFileInputConditionContainer(n) {
		setup(defNode);
	}
	CgnsFileInputConditionContainerReal(const CgnsFileInputConditionContainerReal& i) : CgnsFileInputConditionContainer(i) {
		copyValues(i);
	}
	CgnsFileInputConditionContainerReal& operator=(const CgnsFileInputConditionContainerReal& i) {
		copyValues(i);
		return *this;
	}
	void clear();
	int load();
	int save();
	virtual QVariant variantValue() {return QVariant(m_value);}
	void setValue(double v) {
		if (m_value != v) {
			m_value = v;
			emit valueChanged(m_value);
			emit valueChanged();
		}
	}
	double value() {return m_value;}
	void setDefaultValue(double d) {
		m_default = d;
	}
	double defaultValue() {return m_default;}

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const CgnsFileInputConditionContainerReal& i) {
		m_name = i.m_name;
		m_value = i.m_value;
		m_default = i.m_default;
	}
	double m_default;
	double m_value;
signals:
	void valueChanged(double newvalue);
	void valueChanged();
};

#endif
