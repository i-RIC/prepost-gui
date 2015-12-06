#ifndef INPUTCONDITIONCONTAINERFUNCTIONAL_H
#define INPUTCONDITIONCONTAINERFUNCTIONAL_H

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
		std::string name;
		QVector<double> values;
	};
	InputConditionContainerFunctional();
	InputConditionContainerFunctional(const QString& n, const QString& c, QDomNode defNode, const QDir& dir);
	InputConditionContainerFunctional(const InputConditionContainerFunctional& i);
	~InputConditionContainerFunctional();

	InputConditionContainerFunctional& operator=(const InputConditionContainerFunctional& i);

	int valueCount() const;
	QVector<double>& x();
	QVector<double>& y();

	QVector<double>& param();
	const QVector<double>& param() const;

	QVector<double>& value(int index);
	const QVector<double>& value(int index) const;
	void setValue(const QVector<double>& x, const QVector<double>& y);

	void removeAllValues();

	int load() override;
	int save() override;
	void clear() override;

	QVariant variantValue() const override;

	void importFromYaml(const YAML::Node& doc, const QDir& dir) override;
	void exportToYaml(QTextStream* stream, const QDir& dir) override;

	bool loadDataFromCsvFile(const QString& filename);
	bool saveDataToCsvFile(const QString& filename);

signals:
	void valueChanged();

private:
	void copyValues(const InputConditionContainerFunctional& f);

	bool loadDefaultFromCsvFile(const QString& filename);

	static bool loadFromCsvFile(const QString& filename, Data* param, QList<Data>* values);
	static bool saveToCsvFile(const QString& filename, const Data& param, const QList<Data>& values);

	Data m_param;
	QList<Data> m_values;

	Data m_paramDefault;
	QList<Data> m_valuesDefault;
};

#endif // INPUTCONDITIONCONTAINERFUNCTIONAL_H
