#ifndef CGNSFILEINPUTCONDITIONCONTAINERSTRING_H
#define CGNSFILEINPUTCONDITIONCONTAINERSTRING_H

#include "../../guicore_global.h"
#include "inputconditioncontainer.h"
#include <QString>
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerString : public InputConditionContainer
{
	Q_OBJECT

public:
	InputConditionContainerString() : InputConditionContainer() {}
	InputConditionContainerString(QString n, const QDomNode& defNode) : InputConditionContainer(n) {
		setup(defNode);
	}
	InputConditionContainerString(const InputConditionContainerString& i) : InputConditionContainer(i) {
		copyValues(i);
	}
	InputConditionContainerString& operator=(const InputConditionContainerString& i) {
		copyValues(i);
		return *this;
	}
	void clear() {
		m_value = m_default;
		emit valueChanged(m_value);
	}
	int load() override;
	int save() override;
	virtual QVariant variantValue() const override {return QVariant(m_value);}
	void setValue(const QString& v) {
		if (m_value != v) {
			m_value = v;
			emit valueChanged(m_value);
			emit valueChanged();
		}
	}
	const QString& value() const {return m_value;}

protected:
	void setup(const QDomNode& defNode);
	void copyValues(const InputConditionContainerString& i) {
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
