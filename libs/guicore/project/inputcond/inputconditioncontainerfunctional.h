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
	InputConditionContainerFunctional(const InputConditionContainerFunctional& i);
	InputConditionContainerFunctional& operator=(const InputConditionContainerFunctional& i);
	void removeAllValues();
	void clear();
	int load() override;
	int save() override;
	QVariant variantValue() const override;
	void setValue(const QVector<double>& x, const QVector<double>& y);
	QVector<double>& x();
	QVector<double>& y();
	int valueCount() const;
	QVector<double>& param();
	const QVector<double>& param() const;
	QVector<double>& value(int index);
	const QVector<double>& value(int index) const;
	void importFromYaml(const YAML::Node& doc);
	void exportToYaml(QTextStream* stream);
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

#endif // CGNSFILEINPUTCONDITIONCONTAINERFUNCTIONAL_H
