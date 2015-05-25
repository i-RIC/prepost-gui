#ifndef CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H
#define CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H

#include "../../guicore_global.h"

#include "cgnsfileinputconditioncontainer.h"
class QDomNode;

class GUICOREDLL_EXPORT CgnsFileInputConditionContainerInteger : public CgnsFileInputConditionContainer
{
	Q_OBJECT
public:
	CgnsFileInputConditionContainerInteger() : CgnsFileInputConditionContainer() {
		m_default = 0;
		m_value = 0;
	}
	/// Constructor
	CgnsFileInputConditionContainerInteger(QString n, const QDomNode& defNode) : CgnsFileInputConditionContainer(n) {
		setup(defNode);
	}
	/// Constructor (with copying)
	CgnsFileInputConditionContainerInteger(const CgnsFileInputConditionContainerInteger& i) : CgnsFileInputConditionContainer(i) {
		copyValues(i);
	}
	/// Copyer
	CgnsFileInputConditionContainerInteger& operator=(const CgnsFileInputConditionContainerInteger& i) {
		copyValues(i);
		return *this;
	}
	void clear();
	int load() override;
	int save() override;
	QVariant variantValue() const override {return QVariant(m_value);}
	void setValue(int v) {
		if (m_value != v) {
			m_value = v;
			emit valueChanged(m_value);
			emit valueChanged();
		}
	}
	int value() const {return m_value;}
	void setDefaultValue(int d) {m_default = d;}
	int defaultValue() const {return m_default;}

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const CgnsFileInputConditionContainerInteger& i) {
		m_name = i.m_name;
		m_value = i.m_value;
		m_default = i.m_default;
	}
	int m_default;
	int m_value;

signals:
	void valueChanged(int newvalue);
	void valueChanged();
};

#endif // CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H
