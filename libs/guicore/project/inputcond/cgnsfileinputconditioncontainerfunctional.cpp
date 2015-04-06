#include "cgnsfileinputconditioncontainerfunctional.h"
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <iriclib.h>
#include <stdlib.h>

#include <QDomNode>

CgnsFileInputConditionContainerFunctional::CgnsFileInputConditionContainerFunctional()
	: CgnsFileInputConditionContainer()
{

}

CgnsFileInputConditionContainerFunctional::CgnsFileInputConditionContainerFunctional(QString n, QDomNode defNode)
	: CgnsFileInputConditionContainer(n)
{
	QDomElement paramElem = iRIC::getChildNode(defNode, "Parameter").toElement();
	m_param.name = paramElem.attribute("name", "Param");

	QDomNode valNode = defNode.firstChild();
	while (! valNode.isNull()){
		if (valNode.nodeName() == "Value"){
			QDomElement valElem = valNode.toElement();
			Data valData;
			valData.name = valElem.attribute("name", "Value");
			m_values.append(valData);
		}
		valNode = valNode.nextSibling();
	}
}

int CgnsFileInputConditionContainerFunctional::load()
{
	cgsize_t length;
	double *tmpdata;
	double *data;
	int result;
	std::string tmpbcnameStr = iRIC::toStr(m_bcName);
	char* tmpbcname = const_cast<char*>(tmpbcnameStr.c_str());
	std::string tmpcomplexnameStr = iRIC::toStr(m_complexName);
	char* tmpcomplexname = const_cast<char*>(tmpcomplexnameStr.c_str());
	std::string tmpnameStr = iRIC::toStr(m_name);
	char* tmpname = const_cast<char*>(tmpnameStr.c_str());
	if (m_isBoundaryCondition){
		result = cg_iRIC_Read_BC_FunctionalSize(tmpbcname, m_bcIndex, tmpname, &length);
	} else if (m_isComplexCondition){
		result = cg_iRIC_Read_Complex_FunctionalSize(tmpcomplexname, m_complexIndex, tmpname, &length);
	} else {
		result = cg_iRIC_Read_FunctionalSize(tmpname, &length);
	}
	if (result != 0 || length == 0){goto ERROR;}
	data = new double[length];

	// load parameter.
	if (m_isBoundaryCondition){
		result = cg_iRIC_Read_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), data);
	} else if (m_isComplexCondition){
		result = cg_iRIC_Read_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), data);
	} else {
		result = cg_iRIC_Read_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), data);
	}
	if (result != 0){goto ERROR_AFTER_DATAALLOC;}

	m_param.values.clear();
	m_param.values.insert(0, length, 0);
	tmpdata = m_param.values.data();
	for (int i = 0; i < length; ++i){
		*(tmpdata + i) = *(data + i);
	}
	// load values.
	for (int i = 0; i < m_values.count(); ++i){
		Data& val = m_values[i];
		if (m_isBoundaryCondition){
			result = cg_iRIC_Read_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), data);
		} else if (m_isComplexCondition){
			result = cg_iRIC_Read_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), data);
		} else {
			result = cg_iRIC_Read_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), data);
		}
		if (result != 0){goto ERROR_AFTER_DATAALLOC;}

		val.values.clear();
		val.values.insert(0, length, 0);
		tmpdata = val.values.data();
		for (int j = 0; j < length; ++j){
			*(tmpdata + j) = *(data + j);
		}
	}
	delete data;
	emit valueChanged();
	return 0;

ERROR_AFTER_DATAALLOC:
	delete data;
ERROR:
	clear();
	emit valueChanged();
	return 0;
}

int CgnsFileInputConditionContainerFunctional::save()
{
	std::string tmpbcnameStr = iRIC::toStr(m_bcName);
	char* tmpbcname = const_cast<char*>(tmpbcnameStr.c_str());
	std::string tmpcomplexnameStr = iRIC::toStr(m_complexName);
	char* tmpcomplexname = const_cast<char*>(tmpcomplexnameStr.c_str());
	std::string tmpnameStr = iRIC::toStr(m_name);
	char* tmpname = const_cast<char*>(tmpnameStr.c_str());
	cgsize_t length = m_param.values.count();
	cgsize_t tmplength = length;
	if (tmplength == 0){tmplength = 1;}
	double *data = new double[tmplength];
	// write parameter.
	*data = 0;
	for (int i = 0; i < length; ++i){
		*(data + i) = m_param.values.at(i);
	}
	if (m_isBoundaryCondition){
		cg_iRIC_Write_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), length, data);
	} else if (m_isComplexCondition){
		cg_iRIC_Write_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), length, data);
	} else {
		cg_iRIC_Write_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(m_param.name).c_str()), length, data);
	}

	// write values;
	for (int i = 0; i < m_values.count(); ++i){
		Data& val = m_values[i];
		*data = 0;
		for (int i = 0; i < length; ++i){
			*(data + i) = val.values.at(i);
		}
		if (m_isBoundaryCondition){
			cg_iRIC_Write_BC_FunctionalWithName(tmpbcname, m_bcIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), length, data);
		} else if (m_isComplexCondition){
			cg_iRIC_Write_Complex_FunctionalWithName(tmpcomplexname, m_complexIndex, tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), length, data);
		} else {
			cg_iRIC_Write_FunctionalWithName(tmpname, const_cast<char*>(iRIC::toStr(val.name).c_str()), length, data);
		}
	}
	delete data;
	return 0;
}
