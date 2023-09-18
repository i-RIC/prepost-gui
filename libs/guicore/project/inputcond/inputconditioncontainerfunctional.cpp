#include "inputconditioncontainerfunctional.h"
#include "private/inputconditioncontainerfunctional_impl.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomNode>
#include <QFile>
#include <QRegExp>
#include <QTextStream>

#include <stdlib.h>

#include <yaml-cpp/yaml.h>

#include <h5cgnsconditiongroup.h>
#include <iriclib_errorcodes.h>

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
{}

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

int InputConditionContainerFunctional::loadFunctionalString(const std::string& paramname, std::string* str, const iRICLib::H5CgnsConditionGroup& group)
{
	int length;

	int ier = group.readFunctionalWithNameStringLen(name(), paramname, &length);
	if (ier != IRIC_NO_ERROR) {return ier;}

	std::vector<char> buffer(length + 1, 0);

	ier = group.readFunctionalWithNameString(name(), paramname, buffer.data());
	if (ier != IRIC_NO_ERROR) {return ier;}

	*str = buffer.data();

	return IRIC_NO_ERROR;
}

int InputConditionContainerFunctional::saveFunctionalString(const std::string& paramname, const std::string& str, iRICLib::H5CgnsConditionGroup* group)
{
	return group->writeFunctionalWithNameString(name(), paramname, str);
}

int InputConditionContainerFunctional::load(const iRICLib::H5CgnsConditionGroup& group)
{
	int length;
	std::vector<double> data;
	std::string tmpStr;

	int ret = group.readFunctionalSize(name(), &length);
	if (ret != IRIC_NO_ERROR || length == 0) {goto ERROR;}

	data.assign(length, 0);

	// load parameter.
	ret = group.readFunctionalWithName(name(), impl->m_param.name, data.data());
	if (ret != IRIC_NO_ERROR) {goto ERROR;}

	impl->m_param.values = data;

	// load values.
	for (int i = 0; i < static_cast<int> (impl->m_values.size()); ++i) {
		Data& val = impl->m_values[i];
		ret = group.readFunctionalWithName(name(), val.name, data.data());
		if (ret != IRIC_NO_ERROR) {goto ERROR;}

		val.values = data;
	}

	// load wml2 info
	loadFunctionalString("_siteID", &tmpStr, group);
	impl->m_siteID = tmpStr.c_str();
	loadFunctionalString("_startDate", &tmpStr, group);
	impl->m_startDate = tmpStr.c_str();
	loadFunctionalString("_endDate", &tmpStr, group);
	impl->m_endDate = tmpStr.c_str();

	emit valueChanged();
	return IRIC_NO_ERROR;

ERROR:
	clear();
	emit valueChanged();
	return IRIC_NO_ERROR;;
}

int InputConditionContainerFunctional::save(iRICLib::H5CgnsConditionGroup* group)
{
	auto length = static_cast<int>(impl->m_param.values.size());
	if (length == 0) {return IRIC_NO_ERROR;}

	auto tmplength = length;
	if (tmplength == 0) {tmplength = 1;}
	std::vector<double> data (tmplength, 0);

	// write parameter.
	for (int i = 0; i < length; ++i) {
		data[i] = impl->m_param.values.at(i);
	}
	group->writeFunctionalWithName(name(), impl->m_param.name, length, data.data());

	// write values;
	for (int i = 0; i < static_cast<int> (impl->m_values.size()); ++i) {
		Data& val = impl->m_values[i];
		for (int i = 0; i < length; ++i) {
			data[i] = val.values.at(i);
		}
		group->writeFunctionalWithName(name(), val.name, length, data.data());
	}

	// save wml2 info
	if (! impl->m_siteID.isEmpty()) {
		saveFunctionalString("_siteID", iRIC::toStr(impl->m_siteID), group);
		saveFunctionalString("_startDate", iRIC::toStr(impl->m_startDate), group);
		saveFunctionalString("_endDate", iRIC::toStr(impl->m_endDate), group);
	}

	return IRIC_NO_ERROR;
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
