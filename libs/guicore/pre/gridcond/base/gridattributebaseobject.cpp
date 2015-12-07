#include "gridattributebaseobject.h"

GridAttributeBaseObject::GridAttributeBaseObject(SolverDefinitionGridAttribute* cond, QObject* parent) :
	QObject {parent},
	m_gridAttribute {cond}
{}

GridAttributeBaseObject::~GridAttributeBaseObject()
{}

SolverDefinitionGridAttribute* GridAttributeBaseObject::gridAttribute() const
{
	return m_gridAttribute;
}
