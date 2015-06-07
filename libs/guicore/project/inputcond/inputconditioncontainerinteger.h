#ifndef CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H
#define CGNSFILEINPUTCONDITIONCONTAINERINTEGER_H

#include "../../guicore_global.h"

#include "inputconditioncontainer.h"
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerInteger : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerInteger() : InputConditionContainer() {
		m_default = 0;
		m_value = 0;
	}
	/// Constructor
	InputConditionContainerInteger(QString n, const QDomNode& defNode) : InputConditionContainer(n) {
		setup(defNode);
	}
	/// Constructor (with copying)
	InputConditionContainerInteger(const InputConditionContainerInteger& i) : InputConditionContainer(i) {
		copyValues(i);
	}
	/// Copyer
	InputConditionContainerInteger& operator=(const InputConditionContainerInteger& i) {
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
	void copyValues(const InputConditionContainerInteger& i) {
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
