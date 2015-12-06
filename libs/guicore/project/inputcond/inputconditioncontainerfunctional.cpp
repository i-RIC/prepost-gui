#include "inputconditioncontainerfunctional.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomNode>
#include <QFile>
#include <QRegExp>
#include <QTextStream>

#include <iriclib.h>
#include <stdlib.h>

#include <yaml-cpp/yaml.h>

InputConditionContainerFunctional::InputConditionContainerFunctional() :
	InputConditionContainer()
{}

InputConditionContainerFunctional::InputConditionContainerFunctional(const std::string& n, const QString& c, QDomNode defNode, const QDir& dir) :
	InputConditionContainer(n, c)
{
	QDomElement paramElem = iRIC::getChildNode(defNode, "Parameter").toElement();
	m_param.name = iRIC::toStr(paramElem.attribute("name", "Param"));

	QDomNode valNode = defNode.firstChild();
	while (! valNode.isNull()) {
		if (valNode.nodeName() == "Value") {
			QDomElement valElem = valNode.toElement();
			Data valData;
			valData.name = iRIC::toStr(valElem.attribute("name", "Value"));
			m_values.append(valData);
		}
		valNode = valNode.nextSibling();
	}
	m_paramDefault = m_param;
	m_valuesDefault = m_values;

	// load default from CSV file if exist
	QDomElement elem = defNode.toElement();
	QString defaultCsv = elem.attribute("default");
	if (! defaultCsv.isNull()) {
		loadDefaultFromCsvFile(dir.absoluteFilePath(defaultCsv));
	}
}

InputConditionContainerFunctional::InputConditionContainerFunctional(const InputConditionContainerFunctional& i) :
	InputConditionContainer(i)
{
	copyValues(i);
}

InputConditionContainerFunctional::~InputConditionContainerFunctional()
{}

InputConditionContainerFunctional& InputConditionContainerFunctional::operator=(const InputConditionContainerFunctional& i)
{
	copyValues(i);
	emit valueChanged();
	return *this;
}

int InputConditionContainerFunctional::valueCount() const
{
	return m_values.count();
}

QVector<double>& InputConditionContainerFunctional::x()
{
	return param();
}

QVector<double>& InputConditionContainerFunctional::y()
{
	return value(0);
}

QVector<double>& InputConditionContainerFunctional::param()
{
	return m_param.values;
}

const QVector<double>& InputConditionContainerFunctional::param() const
{
	return m_param.values;
}

QVector<double>& InputConditionContainerFunctional::value(int index)
{
	return m_values[index].values;
}

const QVector<double>& InputConditionContainerFunctional::value(int index) const
{
	return m_values[index].values;
}

void InputConditionContainerFunctional::setValue(const QVector<double>& x, const QVector<double>& y)
{
	m_param.name = "Param";
	m_param.values = x;

	m_values.clear();
	Data val;
	val.name = "Value";
	val.values = y;
	m_values.append(val);
}

void InputConditionContainerFunctional::removeAllValues(){
	m_param.values.clear();
	for (int i = 0; i < m_values.count(); ++i){
		m_values[i].values.clear();
	}
}

int InputConditionContainerFunctional::load()
{
	cgsize_t length;
	double* tmpdata;
	int result;

	std::vector<double> data;

	if (isBoundaryCondition()) {
		result = cg_iRIC_Read_BC_FunctionalSize(toC(bcName()), bcIndex(), toC(name()), &length);
	} else if (isComplexCondition()) {
		result = cg_iRIC_Read_Complex_FunctionalSize(toC(complexName()), complexIndex(), toC(name()), &length);
	} else {
		result = cg_iRIC_Read_FunctionalSize(toC(name()), &length);
	}
	if (result != 0 || length == 0) {goto ERROR;}
	data.assign(length, 0);

	// load parameter.
	if (isBoundaryCondition()) {
		result = cg_iRIC_Read_BC_FunctionalWithName(toC(bcName()), bcIndex(), toC(name()), toC(m_param.name), data.data());
	} else if (isComplexCondition()) {
		result = cg_iRIC_Read_Complex_FunctionalWithName(toC(complexName()), complexIndex(), toC(name()), toC(m_param.name), data.data());
	} else {
		result = cg_iRIC_Read_FunctionalWithName(toC(name()), toC(m_param.name), data.data());
	}
	if (result != 0) {goto ERROR;}

	m_param.values.clear();
	m_param.values.insert(0, length, 0);
	tmpdata = m_param.values.data();
	for (int i = 0; i < length; ++i) {
		*(tmpdata + i) = data[i];
	}
	// load values.
	for (int i = 0; i < m_values.count(); ++i) {
		Data& val = m_values[i];
		if (isBoundaryCondition()) {
			result = cg_iRIC_Read_BC_FunctionalWithName(toC(bcName()), bcIndex(), toC(name()), toC(val.name), data.data());
		} else if (isComplexCondition()) {
			result = cg_iRIC_Read_Complex_FunctionalWithName(toC(complexName()), complexIndex(), toC(name()), toC(val.name), data.data());
		} else {
			result = cg_iRIC_Read_FunctionalWithName(toC(name()), toC(val.name), data.data());
		}
		if (result != 0) {goto ERROR;}

		val.values.clear();
		val.values.insert(0, length, 0);
		tmpdata = val.values.data();
		for (int j = 0; j < length; ++j) {
			*(tmpdata + j) = data[j];
		}
	}
	emit valueChanged();
	return 0;

ERROR:
	clear();
	emit valueChanged();
	return 0;
}

int InputConditionContainerFunctional::save()
{
	cgsize_t length = m_param.values.count();
	cgsize_t tmplength = length;
	if (tmplength == 0) {tmplength = 1;}
	std::vector<double> data (tmplength, 0);
	// write parameter.
	for (int i = 0; i < length; ++i) {
		data[i] = m_param.values.at(i);
	}
	if (isBoundaryCondition()) {
		cg_iRIC_Write_BC_FunctionalWithName(toC(bcName()), bcIndex(), toC(name()), toC(m_param.name), length, data.data());
	} else if (isComplexCondition()) {
		cg_iRIC_Write_Complex_FunctionalWithName(toC(complexName()), complexIndex(), toC(name()), toC(m_param.name), length, data.data());
	} else {
		cg_iRIC_Write_FunctionalWithName(toC(name()), toC(m_param.name), length, data.data());
	}

	// write values;
	for (int i = 0; i < m_values.count(); ++i) {
		Data& val = m_values[i];
		for (int i = 0; i < length; ++i) {
			data[i] = val.values.at(i);
		}
		if (isBoundaryCondition()) {
			cg_iRIC_Write_BC_FunctionalWithName(toC(bcName()), bcIndex(), toC(name()), toC(val.name), length, data.data());
		} else if (isComplexCondition()) {
			cg_iRIC_Write_Complex_FunctionalWithName(toC(complexName()), complexIndex(), toC(name()), toC(val.name), length, data.data());
		} else {
			cg_iRIC_Write_FunctionalWithName(toC(name()), toC(val.name), length, data.data());
		}
	}
	return 0;
}

void InputConditionContainerFunctional::clear()
{
	m_param.values = m_paramDefault.values;
	for (int i = 0; i < m_values.count(); ++i){
		m_values[i].values = m_valuesDefault[i].values;
	}
}

QVariant InputConditionContainerFunctional::variantValue() const
{
	return QVariant(0);
}

void InputConditionContainerFunctional::importFromYaml(const YAML::Node& doc, const QDir& dir)
{
	if (doc[toC(name())]) {
		QString filename = doc[toC(name())].as<std::string>().c_str();
		loadDataFromCsvFile(dir.absoluteFilePath(filename));
	}
}

void InputConditionContainerFunctional::exportToYaml(QTextStream* stream, const QDir& dir)
{
	*stream << name().c_str() << ": " << name().c_str() << ".csv" << "\t#[function] " << caption() << "\r\n";
	QString filename = QString("%1.csv").arg(name().c_str());
	saveDataToCsvFile(dir.absoluteFilePath(filename));
}

bool InputConditionContainerFunctional::loadDataFromCsvFile(const QString& filename)
{
	removeAllValues();
	return loadFromCsvFile(filename, &m_param, &m_values);
}

bool InputConditionContainerFunctional::saveDataToCsvFile(const QString& filename)
{
	return saveToCsvFile(filename, m_param, m_values);
}

void InputConditionContainerFunctional::copyValues(const InputConditionContainerFunctional& f)
{
	InputConditionContainer::copyValues(f);
	m_param = f.m_param;
	m_values = f.m_values;
	m_paramDefault = f.m_paramDefault;
	m_valuesDefault = f.m_valuesDefault;
}

bool InputConditionContainerFunctional::loadDefaultFromCsvFile(const QString& filename)
{
	return loadFromCsvFile(filename, &m_paramDefault, &m_valuesDefault);
}

bool InputConditionContainerFunctional::loadFromCsvFile(const QString& filename, Data* param, QList<Data>* values)
{
	QFile csvFile(filename);
	bool ok = csvFile.open(QFile::ReadOnly | QFile::Text);
	if (! ok) {return false;}

	QTextStream stream(&csvFile);
	QString line;
	do {
		line = stream.readLine();
		if (line.isEmpty()) {break;}
		QStringList frags = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
		if (frags.length() < values->length() + 1) {break;}
		param->values.push_back(frags[0].toDouble());
		for (int i = 0; i < values->length(); ++i) {
			(*values)[i].values.push_back(frags[i + 1].toDouble());
		}
	} while (true);
	csvFile.close();
	return true;
}

bool InputConditionContainerFunctional::saveToCsvFile(const QString& filename, const Data& param, const QList<Data>& values)
{
	QFile csvFile(filename);
	bool ok = csvFile.open(QFile::WriteOnly | QFile::Text);
	if (! ok) {return false;}

	QTextStream stream(&csvFile);
	for (int i = 0; i < param.values.size(); ++i) {
		stream  << param.values.at(i);
		for (int j = 0; j < values.size(); ++j){
			stream << "," << values.at(j).values.at(i);
		}
		stream << "\n";
	}
	csvFile.close();
	return true;
}
