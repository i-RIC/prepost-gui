#include "inputconditioncontainerfunctional.h"
#include "private/inputconditioncontainerfunctional_impl.h"

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

bool loadFromCsvFile(const QString& filename, InputConditionContainerFunctional::Data* param, std::vector<InputConditionContainerFunctional::Data>* values)
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
		if (frags.length() < values->size() + 1) {break;}
		param->values.push_back(frags[0].toDouble());
		for (int i = 0; i < values->size(); ++i) {
			(*values)[i].values.push_back(frags[i + 1].toDouble());
		}
	} while (true);
	csvFile.close();
	return true;
}

bool saveToCsvFile(const QString& filename, const InputConditionContainerFunctional::Data& param, const std::vector<InputConditionContainerFunctional::Data>& values)
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

} // namespace

InputConditionContainerFunctional::InputConditionContainerFunctional() :
	InputConditionContainer(),
	impl {new Impl {}}
{}

InputConditionContainerFunctional::InputConditionContainerFunctional(const std::string& n, const QString& c, QDomNode defNode, const QDir& dir) :
	InputConditionContainer(n, c),
	impl {new Impl {}}
{
	QDomElement defElem = defNode.toElement();
	if (defElem.hasAttribute("wml2:url")) {
		impl->m_wml2URL = defElem.attribute("wml2:url");
	}

	QDomElement paramElem = iRIC::getChildNode(defNode, "Parameter").toElement();
	impl->m_param.name = iRIC::toStr(paramElem.attribute("name", "Param"));
	if (! impl->m_wml2URL.isEmpty()) {
		impl->m_paramConvert.code = "time";
		impl->m_paramConvert.factor = 1.0 / paramElem.attribute("wml2:secondsPerUnit", "1").toDouble();
	}

	QDomNode valNode = defNode.firstChild();
	while (! valNode.isNull()) {
		if (valNode.nodeName() == "Value") {
			QDomElement valElem = valNode.toElement();
			Data valData;
			valData.name = iRIC::toStr(valElem.attribute("name", "Value"));
			impl->m_values.push_back(valData);
			if (! impl->m_wml2URL.isEmpty()) {
				Convert convert;
				convert.code = iRIC::toStr(valElem.attribute("wml2:nwisCode", "code"));
				convert.factor = valElem.attribute("wml2:conversionFactor", "1").toDouble();
				impl->m_valuesConvert.push_back(convert);
			}
		}
		valNode = valNode.nextSibling();
	}
	impl->m_paramDefault = impl->m_param;
	impl->m_valuesDefault = impl->m_values;

	// load default from CSV file if exist
	QDomElement elem = defNode.toElement();
	QString defaultCsv = elem.attribute("default");
	if (! defaultCsv.isNull()) {
		loadDefaultFromCsvFile(dir.absoluteFilePath(defaultCsv));
	}
}

InputConditionContainerFunctional::InputConditionContainerFunctional(const InputConditionContainerFunctional& i) :
	InputConditionContainer(i),
	impl {new Impl {}}
{
	copyValues(i);
}

InputConditionContainerFunctional::~InputConditionContainerFunctional()
{
	delete impl;
}

InputConditionContainerFunctional& InputConditionContainerFunctional::operator=(const InputConditionContainerFunctional& i)
{
	copyValues(i);
	emit valueChanged();
	return *this;
}

int InputConditionContainerFunctional::valueCount() const
{
	return static_cast<int>(impl->m_values.size());
}

std::vector<double>& InputConditionContainerFunctional::x()
{
	return param();
}

std::vector<double>& InputConditionContainerFunctional::y()
{
	return value(0);
}

std::vector<double>& InputConditionContainerFunctional::param()
{
	return impl->m_param.values;
}

const std::vector<double>& InputConditionContainerFunctional::param() const
{
	return impl->m_param.values;
}

double InputConditionContainerFunctional::paramFactor() const
{
	return impl->m_paramConvert.factor;
}

std::vector<double>& InputConditionContainerFunctional::value(int index)
{
	return impl->m_values[index].values;
}

const std::vector<double>& InputConditionContainerFunctional::value(int index) const
{
	return impl->m_values[index].values;
}

double InputConditionContainerFunctional::valueFactor(int index) const
{
	return impl->m_valuesConvert[index].factor;
}

int InputConditionContainerFunctional::codeCount() const
{
	return static_cast<int>(impl->m_valuesConvert.size());
}

std::string InputConditionContainerFunctional::code(int index) const
{
	return impl->m_valuesConvert[index].code;
}

std::vector<std::string> InputConditionContainerFunctional::valueNames() const
{
	std::vector<std::string> names;

	for (auto v : impl->m_values) {
		names.push_back(v.name);
	}

	return names;
}

bool InputConditionContainerFunctional::hasValue(const std::string& name) const
{
	for (auto v : impl->m_values) {
		if (v.name == name) {return true;}
	}
	return false;
}

std::vector<double>& InputConditionContainerFunctional::value(const std::string& name)
{
	for (auto& v : impl->m_values) {
		if (v.name == name) {return v.values;}
	}
	return impl->m_values[0].values;
}

const std::vector<double>& InputConditionContainerFunctional::value(const std::string& name) const
{
	for (auto& v : impl->m_values) {
		if (v.name == name) {return v.values;}
	}
	return impl->m_values[0].values;
}

void InputConditionContainerFunctional::setValue(const std::vector<double>& x, const std::vector<double>& y)
{
	impl->m_param.name = "Param";
	impl->m_param.values = x;

	impl->m_values.clear();
	Data val;
	val.name = "Value";
	val.values = y;
	impl->m_values.push_back(val);
}

QString& InputConditionContainerFunctional::wml2URL() const
{
	return impl->m_wml2URL;
}

QString& InputConditionContainerFunctional::site() const
{
	return impl->m_siteID;
}

QString& InputConditionContainerFunctional::startDate() const
{
	return impl->m_startDate;
}

QString& InputConditionContainerFunctional::endDate() const
{
	return impl->m_endDate;
}

void InputConditionContainerFunctional::setSite(const QString& site)
{
	impl->m_siteID = site;
}

void InputConditionContainerFunctional::setStartDate(const QString& date)
{
	impl->m_startDate = date;
}

void InputConditionContainerFunctional::setEndDate(const QString& date)
{
	impl->m_endDate = date;
}

void InputConditionContainerFunctional::removeAllValues(){
	impl->m_param.values.clear();
	for (int i = 0; i < impl->m_values.size(); ++i){
		impl->m_values[i].values.clear();
	}
}

bool InputConditionContainerFunctional::loadFunctionalString(const char* paramname, QString& str)
{
	int length;
	std::vector<char> vstr;
	if (isBoundaryCondition()) {
		if (cg_iRIC_Read_BC_FunctionalWithName_StringLen(toC(bcName()), bcIndex(), toC(name()), paramname, &length) == 0) {
			vstr.assign(' ', length + 1);
			if (cg_iRIC_Read_BC_FunctionalWithName_String(toC(bcName()), bcIndex(), toC(name()), paramname, vstr.data()) == 0) {
				str = QString::fromStdString(vstr.data());
				return true;
			}
		}
	}
	else if (isComplexCondition()) {
		if (cg_iRIC_Read_Complex_FunctionalWithName_StringLen(toC(complexName()), complexIndex(), toC(name()), paramname, &length) == 0) {
			vstr.assign(' ', length + 1);
			if (cg_iRIC_Read_Complex_FunctionalWithName_String(toC(complexName()), complexIndex(), toC(name()), paramname, vstr.data()) == 0) {
				str = QString::fromStdString(vstr.data());
				return true;
			}
		}
	}
	else {
		if (cg_iRIC_Read_FunctionalWithName_StringLen(toC(name()), paramname, &length) == 0) {
			vstr.assign(' ', length + 1);
			if (cg_iRIC_Read_FunctionalWithName_String(toC(name()), paramname, vstr.data()) == 0) {
				str = QString::fromStdString(vstr.data());
				return true;
			}
		}
	}
	return false;
}

bool InputConditionContainerFunctional::saveFunctionalString(const char* paramname, const QString& str)
{
	int result;
	if (isBoundaryCondition()) {
		result = cg_iRIC_Write_BC_FunctionalWithName_String(toC(bcName()), bcIndex(), toC(name()), paramname, toC(str.toStdString()));
	}
	else if (isComplexCondition()) {
		result = cg_iRIC_Write_Complex_FunctionalWithName_String(toC(complexName()), complexIndex(), toC(name()), paramname, toC(str.toStdString()));
	}
	else {
		result = cg_iRIC_Write_FunctionalWithName_String(toC(name()), paramname, toC(str.toStdString()));
	}
	return (result == 0);
}

int InputConditionContainerFunctional::load()
{
	cgsize_t length;
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
		result = cg_iRIC_Read_BC_FunctionalWithName(toC(bcName()), bcIndex(), toC(name()), toC(impl->m_param.name), data.data());
	} else if (isComplexCondition()) {
		result = cg_iRIC_Read_Complex_FunctionalWithName(toC(complexName()), complexIndex(), toC(name()), toC(impl->m_param.name), data.data());
	} else {
		result = cg_iRIC_Read_FunctionalWithName(toC(name()), toC(impl->m_param.name), data.data());
	}
	if (result != 0) {goto ERROR;}

	impl->m_param.values = data;
	// load values.
	for (int i = 0; i < impl->m_values.size(); ++i) {
		Data& val = impl->m_values[i];
		if (isBoundaryCondition()) {
			result = cg_iRIC_Read_BC_FunctionalWithName(toC(bcName()), bcIndex(), toC(name()), toC(val.name), data.data());
		} else if (isComplexCondition()) {
			result = cg_iRIC_Read_Complex_FunctionalWithName(toC(complexName()), complexIndex(), toC(name()), toC(val.name), data.data());
		} else {
			result = cg_iRIC_Read_FunctionalWithName(toC(name()), toC(val.name), data.data());
		}
		if (result != 0) {goto ERROR;}

		val.values = data;
	}

	// load wml2 info
	loadFunctionalString("_siteID", impl->m_siteID);
	loadFunctionalString("_startDate", impl->m_startDate);
	loadFunctionalString("_endDate", impl->m_endDate);

	emit valueChanged();
	return 0;

ERROR:
	clear();
	emit valueChanged();
	return 0;
}

int InputConditionContainerFunctional::save()
{
	cgsize_t length = static_cast<cgsize_t>(impl->m_param.values.size());
	cgsize_t tmplength = length;
	if (tmplength == 0) {tmplength = 1;}
	std::vector<double> data (tmplength, 0);
	// write parameter.
	for (int i = 0; i < length; ++i) {
		data[i] = impl->m_param.values.at(i);
	}
	if (isBoundaryCondition()) {
		cg_iRIC_Write_BC_FunctionalWithName(toC(bcName()), bcIndex(), toC(name()), toC(impl->m_param.name), length, data.data());
	} else if (isComplexCondition()) {
		cg_iRIC_Write_Complex_FunctionalWithName(toC(complexName()), complexIndex(), toC(name()), toC(impl->m_param.name), length, data.data());
	} else {
		cg_iRIC_Write_FunctionalWithName(toC(name()), toC(impl->m_param.name), length, data.data());
	}

	// write values;
	for (int i = 0; i < impl->m_values.size(); ++i) {
		Data& val = impl->m_values[i];
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

	// save wml2 info
	if (!impl->m_siteID.isEmpty()) {
		saveFunctionalString("_siteID", impl->m_siteID);
		saveFunctionalString("_startDate", impl->m_startDate);
		saveFunctionalString("_endDate", impl->m_endDate);
	}

	return 0;
}

void InputConditionContainerFunctional::clear()
{
	impl->m_param.values = impl->m_paramDefault.values;
	for (int i = 0; i < impl->m_values.size(); ++i){
		impl->m_values[i].values = impl->m_valuesDefault[i].values;
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
	return loadFromCsvFile(filename, &(impl->m_param), &(impl->m_values));
}

bool InputConditionContainerFunctional::saveDataToCsvFile(const QString& filename)
{
	return saveToCsvFile(filename, impl->m_param, impl->m_values);
}

void InputConditionContainerFunctional::copyValues(const InputConditionContainerFunctional& f)
{
	InputConditionContainer::copyValues(f);
	impl->m_param         = f.impl->m_param;
	impl->m_values        = f.impl->m_values;
	impl->m_paramDefault  = f.impl->m_paramDefault;
	impl->m_valuesDefault = f.impl->m_valuesDefault;
	impl->m_wml2URL       = f.impl->m_wml2URL;
	impl->m_paramConvert  = f.impl->m_paramConvert;
	impl->m_valuesConvert = f.impl->m_valuesConvert;
	impl->m_siteID        = f.impl->m_siteID;
	impl->m_startDate     = f.impl->m_startDate;
	impl->m_endDate       = f.impl->m_endDate;
}

bool InputConditionContainerFunctional::loadDefaultFromCsvFile(const QString& filename)
{
	return loadFromCsvFile(filename, &(impl->m_paramDefault), &(impl->m_valuesDefault));
}
