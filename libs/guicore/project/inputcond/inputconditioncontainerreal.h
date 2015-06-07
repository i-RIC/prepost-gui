#ifndef CALCCOND_CONT_REAL_H
#define CALCCOND_CONT_REAL_H

#include "../../guicore_global.h"

#include "inputconditioncontainer.h"
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerReal : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerReal() : InputConditionContainer() {
		m_value = 0;
		m_default = 0;
	}
	InputConditionContainerReal(QString n, const QDomNode& defNode) : InputConditionContainer(n) {
		setup(defNode);
	}
	InputConditionContainerReal(const InputConditionContainerReal& i) : InputConditionContainer(i) {
		copyValues(i);
	}
	InputConditionContainerReal& operator=(const InputConditionContainerReal& i) {
		copyValues(i);
		return *this;
	}
	void clear();
	int load() override;
	int save() override;
	virtual QVariant variantValue() const override {return QVariant(m_value);}
	void setValue(double v) {
		if (m_value != v) {
			m_value = v;
			emit valueChanged(m_value);
			emit valueChanged();
		}
	}
	double value() const {return m_value;}
	void setDefaultValue(double d) {m_default = d;}
	double defaultValue() const {return m_default;}

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerReal& i) {
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
