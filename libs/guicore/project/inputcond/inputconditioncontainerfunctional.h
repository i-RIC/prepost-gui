#ifndef INPUTCONDITIONCONTAINERFUNCTIONAL_H
#define INPUTCONDITIONCONTAINERFUNCTIONAL_H

#include "../../guicore_global.h"

#include "inputconditioncontainer.h"

#include <vector>

class QDir;
class QDomNode;

class GUICOREDLL_EXPORT InputConditionContainerFunctional : public InputConditionContainer
{
	Q_OBJECT

public:
	struct Data {
		std::string name;
		std::vector<double> values;
	};
	struct Convert {
		std::string code;
		double factor;
	};


	InputConditionContainerFunctional();
	InputConditionContainerFunctional(const std::string& n, const QString& c, QDomNode defNode, const QDir& dir);
	InputConditionContainerFunctional(const InputConditionContainerFunctional& i);
	~InputConditionContainerFunctional();

	InputConditionContainerFunctional& operator=(const InputConditionContainerFunctional& i);

	int valueCount() const;
	std::vector<double>& x();
	std::vector<double>& y();

	std::vector<double>& param();
	const std::vector<double>& param() const;

	double paramFactor() const;

	std::vector<double>& value(int index);
	const std::vector<double>& value(int index) const;

	std::vector<std::string> valueNames() const;
	bool hasValue(const std::string& name) const;

	std::vector<double>& value(const std::string& name);
	const std::vector<double>& value(const std::string& name) const;

	int codeCount() const;
	std::string code(int index) const;
	double valueFactor(int index) const;

	void setValue(const std::vector<double>& x, const std::vector<double>& y);

	QString& wml2URL() const;
	QString& site() const;
	QString& startDate() const;
	QString& endDate() const;

	void setSite(const QString& site);
	void setStartDate(const QString& date);
	void setEndDate(const QString& date);

	void removeAllValues();

	int load(const iRICLib::H5CgnsConditionGroup& group) override;
	int save(iRICLib::H5CgnsConditionGroup* group) override;
	void clear() override;

	QVariant variantValue() const override;

	void importFromYaml(const YAML::Node& doc, const QDir& dir) override;
	void exportToYaml(QTextStream* stream, const QDir& dir) override;

	bool loadDataFromCsvFile(const QString& filename);
	bool saveDataToCsvFile(const QString& filename);

	int loadFunctionalString(const std::string& paramname, std::string* str, const iRICLib::H5CgnsConditionGroup& group);
	int saveFunctionalString(const std::string& paramname, const std::string& str, iRICLib::H5CgnsConditionGroup* group);

signals:
	void valueChanged();

private:
	void copyValues(const InputConditionContainerFunctional& f);
	bool loadDefaultFromCsvFile(const QString& filename);

	class Impl;
	Impl* impl;
};

#endif // INPUTCONDITIONCONTAINERFUNCTIONAL_H
