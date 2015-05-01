#include "cgnsfileinputconditioncontainerreal.h"

#include <misc/stringtool.h>

#include <QDomElement>
#include <QDomNode>

#include <iriclib.h>

void CgnsFileInputConditionContainerReal::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	m_default = e.attribute("default", "0").toDouble();
	m_value = m_default;
}

void CgnsFileInputConditionContainerReal::clear()
{
	if (m_default != m_value) {
		m_value = m_default;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}
int CgnsFileInputConditionContainerReal::load()
{
	int ret;
	if (m_isBoundaryCondition) {
		ret = cg_iRIC_Read_BC_Real(const_cast<char*>(iRIC::toStr(m_bcName).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), &m_value);
	} else if (m_isComplexCondition) {
		ret = cg_iRIC_Read_Complex_Real(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), &m_value);
	} else {
		ret = cg_iRIC_Read_Real(const_cast<char*>(iRIC::toStr(m_name).c_str()), &m_value);
	}
	if (ret != 0) {
		clear();
	} else {
		emit valueChanged(m_value);
		emit valueChanged();
	}
	return ret;
}
int CgnsFileInputConditionContainerReal::save()
{
	if (m_isBoundaryCondition) {
		return cg_iRIC_Write_BC_Real(const_cast<char*>(iRIC::toStr(m_bcName).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), m_value);
	} else if (m_isComplexCondition) {
		return cg_iRIC_Write_Complex_Real(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), m_value);
	} else {
		return cg_iRIC_Write_Real(const_cast<char*>(iRIC::toStr(m_name).c_str()), m_value);
	}
}
