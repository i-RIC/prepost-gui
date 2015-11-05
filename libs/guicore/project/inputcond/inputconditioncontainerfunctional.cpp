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

namespace {

void importDataFromYaml(const YAML::Node& node, InputConditionContainerFunctional::Data* data)
{
	if (! node[iRIC::toStr(data->name).c_str()]) {
		return;
	}
	const YAML::Node& myNode = node[iRIC::toStr(data->name).c_str()];
	YAML::Node::const_iterator it;
	data->values.clear();
	for (it = myNode.begin(); it != myNode.end(); ++it) {
		const YAML::Node& vNode = *it;
		data->values.append(vNode.as<double>());
	}
}

void exportDataToYaml(QTextStream* stream, const InputConditionContainerFunctional::Data& data)
{
	*stream << "  " << data.name << ": [";
	for (int i = 0 ; i < data.values.size(); ++i) {
		if (i != 0) {*stream << ", ";}
		*stream << data.values.at(i);
	}
	*stream << "]\r\n";
}

} // namespace

InputConditionContainerFunctional::InputConditionContainerFunctional()
	: InputConditionContainer()
{

}

InputConditionContainerFunctional::InputConditionContainerFunctional(QString n, QDomNode defNode, const QDir& dir)
	: InputConditionContainer(n)
{
	QDomElement paramElem = iRIC::getChildNode(defNode, "Parameter").toElement();
	m_param.name = paramElem.attribute("name", "Param");

	QDomNode valNode = defNode.firstChild();
	while (! valNode.isNull()) {
		if (valNode.nodeName() == "Value") {
			QDomElement valElem = valNode.toElement();
			Data valData;
			valData.name = valElem.attribute("name", "Value");
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

InputConditionContainerFunctional& InputConditionContainerFunctional::operator=(const InputConditionContainerFunctional& i)
{
	copyValues(i);
	emit valueChanged();
	return *this;
}

void InputConditionContainerFunctional::clear()
{
	m_param.values = m_paramDefault.values;
	for (int i = 0; i < m_values.count(); ++i){
		m_values[i].values = m_valuesDefault[i].values;
	}
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
//	double* data;
	int result;

	std::string tmpbcnameStr = iRIC::toStr(m_bcName);
	char* tmpbcname = const_cast<char*>(tmpbcnameStr.c_str());
	std::string tmpcomplexnameStr = iRIC::toStr(m_complexName);
	char* tmpcomplexname = const_cast<char*>(tmpcomplexnameStr.c_str());
	std::string tmpnameStr = iRIC::toStr(m_name);
	char* tmpname = const_cast<char*>(tmpnameStr.c_str());
	std::vector<double> data;

	if (m_isBoundaryCondition) {
		result = cg_iRIC_Read_BC_FunctionalSize(tmpbcname, m_bcIndex, tmpname, &length);
	} else if (m_isComplexCondition) {
		result = cg_iRIC_Read_Complex_FunctionalSize(tmpcomplexname, m_complexIndex, tmpname, &length);
	} else {
		result = cg_iRIC_Read_FunctionalSize(tmpname, &length);
	}
	if (result != 0 || length == 0) {goto ERROR;}
	data.assign(length, 0);

	// load parameter.
	if (m_isBoundaryCondition) {
		result = cg_iRIC_Read_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), data.data());
	} else if (m_isComplexCondition) {
		result = cg_iRIC_Read_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), data.data());
	} else {
		result = cg_iRIC_Read_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), data.data());
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
		if (m_isBoundaryCondition) {
			result = cg_iRIC_Read_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), data.data());
		} else if (m_isComplexCondition) {
			result = cg_iRIC_Read_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), data.data());
		} else {
			result = cg_iRIC_Read_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), data.data());
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
	std::string tmpbcnameStr = iRIC::toStr(m_bcName);
	char* tmpbcname = const_cast<char*>(tmpbcnameStr.c_str());
	std::string tmpcomplexnameStr = iRIC::toStr(m_complexName);
	char* tmpcomplexname = const_cast<char*>(tmpcomplexnameStr.c_str());
	std::string tmpnameStr = iRIC::toStr(m_name);
	char* tmpname = const_cast<char*>(tmpnameStr.c_str());
	cgsize_t length = m_param.values.count();
	cgsize_t tmplength = length;
	if (tmplength == 0) {tmplength = 1;}
	std::vector<double> data (tmplength, 0);
	// write parameter.
	for (int i = 0; i < length; ++i) {
		data[i] = m_param.values.at(i);
	}
	if (m_isBoundaryCondition) {
		cg_iRIC_Write_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), length, data.data());
	} else if (m_isComplexCondition) {
		cg_iRIC_Write_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), length, data.data());
	} else {
		cg_iRIC_Write_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), length, data.data());
	}

	// write values;
	for (int i = 0; i < m_values.count(); ++i) {
		Data& val = m_values[i];
		for (int i = 0; i < length; ++i) {
			data[i] = val.values.at(i);
		}
		if (m_isBoundaryCondition) {
			cg_iRIC_Write_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), length, data.data());
		} else if (m_isComplexCondition) {
			cg_iRIC_Write_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), length, data.data());
		} else {
			cg_iRIC_Write_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), length, data.data());
		}
	}
	return 0;
}

QVariant InputConditionContainerFunctional::variantValue() const
{
	return QVariant(0);
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

QVector<double>& InputConditionContainerFunctional::x()
{
	return param();
}

QVector<double>& InputConditionContainerFunctional::y()
{
	return value(0);
}

int InputConditionContainerFunctional::valueCount() const
{
	return m_values.count();
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

void InputConditionContainerFunctional::importFromYaml(const YAML::Node& doc)
{
	if (doc[iRIC::toStr(m_name).c_str()]) {
		const YAML::Node& myNode = doc[iRIC::toStr(m_name).c_str()];
		importDataFromYaml(myNode, &m_param);
		for (int i = 0; i < m_values.size(); ++i) {
			importDataFromYaml(myNode, &(m_values[i]));
		}
	}
	bool allLenSame = true;
	int paramLen = m_param.values.size();
	for (int i = 0; i < m_values.size(); ++i){
		allLenSame = allLenSame && (paramLen == m_values[i].values.size());
	}
	if (! allLenSame){
		clear();
	}
}

void InputConditionContainerFunctional::exportToYaml(QTextStream* stream)
{
	*stream << m_name << ": " << "\r\n";
	exportDataToYaml(stream, m_param);
	for (int i = 0; i < m_values.length(); ++i) {
		exportDataToYaml(stream, m_values.at(i));
	}
}

void InputConditionContainerFunctional::copyValues(const InputConditionContainerFunctional& f)
{
	m_name = f.m_name;
	m_param = f.m_param;
	m_values = f.m_values;
	m_paramDefault = f.m_paramDefault;
	m_valuesDefault = f.m_valuesDefault;
}

void InputConditionContainerFunctional::loadDefaultFromCsvFile(const QString& filename)
{
	QFile csvFile(filename);
	bool ok = csvFile.open(QFile::ReadOnly | QFile::Text);
	if (! ok) {return;}

	QTextStream stream(&csvFile);
	QString line;
	do {
		line = stream.readLine();
		if (line.isEmpty()) {break;}
		QStringList frags = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
		if (frags.length() < m_values.length() + 1) {break;}
		m_paramDefault.values.push_back(frags[0].toDouble());
		for (int i = 0; i < m_values.length(); ++i) {
			m_valuesDefault[i].values.push_back(frags[i + 1].toDouble());
		}
	} while (true);
	csvFile.close();
}
