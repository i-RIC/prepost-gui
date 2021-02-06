#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsfile.h"
#include "../inputconditioncontainerstring.h"

InputConditionWidgetCgnsFile::InputConditionWidgetCgnsFile(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile *file) :
	InputConditionWidgetFilename(defnode, t, cont),
	m_cgnsFile {file}
{}

void InputConditionWidgetCgnsFile::setValue(const QString& newvalue)
{
	InputConditionWidgetFilename::setValue(newvalue);
	informChanged();
}

void InputConditionWidgetCgnsFile::handleEditingFinished()
{
	InputConditionWidgetFilename::handleEditingFinished();
	informChanged();
}

void InputConditionWidgetCgnsFile::informChanged()
{
	m_cgnsFile->setFileName(m_container->value());
}
