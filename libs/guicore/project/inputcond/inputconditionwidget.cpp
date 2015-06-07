#include "inputconditiondependency.h"
#include "inputconditionwidget.h"

#include <misc/xmlsupport.h>

void InputConditionWidget::setDependency(InputConditionDependency* dep)
{
	if (m_dependency != nullptr) {delete m_dependency;}
	m_dependency = dep;
}

QList<QDomNode> InputConditionWidget::getEnums(QDomNode defNode)
{
	QDomNode enumsNode = iRIC::getChildNode(defNode, "Enumerations");
	if (! enumsNode.isNull()) {
		// In case "Enumeration" elements are under "Enumerations" element.
		QDomNodeList noms = enumsNode.childNodes();
		QList<QDomNode> list;
		for (int i = 0; i < noms.count(); ++i) {
			list.append(noms.item(i));
		}
		return list;
	}

	// In case "Enumeration" elements are just under "Definition" element.
	QDomNodeList children = defNode.childNodes();
	QList<QDomNode> list;
	for (int i = 0; i < children.count(); ++i) {
		QDomNode node = children.at(i);
		if (node.nodeName() == "Enumeration") {
			list.append(node);
		}
	}
	return list;
}

bool InputConditionWidget::hasEnums(QDomNode defNode)
{
	QDomNode enumsNode = iRIC::getChildNode(defNode, "Enumerations");
	if (! enumsNode.isNull()) {return true;}
	QDomNode enumNode = iRIC::getChildNode(defNode, "Enumeration");
	if (! enumNode.isNull()) {return true;}
	return false;
}
