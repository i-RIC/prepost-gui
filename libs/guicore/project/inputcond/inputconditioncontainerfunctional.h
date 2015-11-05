#ifndef CGNSFILEINPUTCONDITIONCONTAINERFUNCTIONAL_H
#define CGNSFILEINPUTCONDITIONCONTAINERFUNCTIONAL_H

#include "../../guicore_global.h"

#include "inputconditioncontainer.h"
#include <QVector>

class QDir;
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerFunctional : public InputConditionContainer
{
	Q_OBJECT

public:
	struct Data {
		QString name;
		QVector<double> values;
	};
	InputConditionContainerFunctional();
	InputConditionContainerFunctional(QString n, QDomNode defNode, const QDir& dir);
	InputConditionContainerFunctional(const InputConditionContainerFunctional& i) : InputConditionContainer(i) {
		copyValues(i);
	}
	InputConditionContainerFunctional& operator=(const InputConditionContainerFunctional& i) {
		copyValues(i);
		emit valueChanged();
		return *this;
	}
	void clear() {
		m_param.values.clear();
		for (int i = 0; i < m_values.count(); ++i) {
			m_values[i].values.clear();
		}
	}
	int load() override;
	int save() override;
	QVariant variantValue() const override {return QVariant(0);}

public:
	void setValue(const QVector<double>& x, const QVector<double>& y) {
		m_param.name = "Param";
		m_param.values = x;

		m_values.clear();
		Data val;
		val.name = "Value";
		val.values = y;
		m_values.append(val);
	}
	QVector<double>& x() {
		return param();
	}
	QVector<double>& y() {
		return value(0);
	}
	int valueCount() const {return m_values.count();}
	QVector<double>& param() {return m_param.values;}
	const QVector<double>& param() const {return m_param.values;}
	QVector<double>& value(int index) {return m_values[index].values;}
	const QVector<double>& value(int index) const {return m_values[index].values;}

signals:
	void valueChanged();

private:
	void copyValues(const InputConditionContainerFunctional& f) {
		m_name = f.m_name;
		m_param = f.m_param;
		m_values = f.m_values;
	}
	Data m_param;
	QList<Data> m_values;
};

#endif // CGNSFILEINPUTCONDITIONCONTAINERFUNCTIONAL_H
