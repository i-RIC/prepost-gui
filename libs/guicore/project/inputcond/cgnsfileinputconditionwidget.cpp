#include "cgnsfileinputconditionwidget.h"
#include "cgnsfileinputconditiondependency.h"
#include <misc/xmlsupport.h>

void CgnsFileInputConditionWidget::setDependency(CgnsFileInputConditionDependency* dep)
{
	if (m_dependency != nullptr) {delete m_dependency;}
	m_dependency = dep;
}

QList<QDomNode> CgnsFileInputConditionWidget::getEnums(QDomNode defNode)
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

bool CgnsFileInputConditionWidget::hasEnums(QDomNode defNode)
{
	QDomNode enumsNode = iRIC::getChildNode(defNode, "Enumerations");
	if (! enumsNode.isNull()) {return true;}
	QDomNode enumNode = iRIC::getChildNode(defNode, "Enumeration");
	if (! enumNode.isNull()) {return true;}
	return false;
}
