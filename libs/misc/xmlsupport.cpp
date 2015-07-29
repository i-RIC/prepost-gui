#include "xmlsupport.h"

#include <QColor>
#include <QDomNode>
#include <QList>
#include <QString>
#include <QXmlStreamWriter>

#include <cmath>

/// Get the first child node that has the name nodeName.
QDomNode iRIC::getChildNode(const QDomNode& parent, const QString& nodeName)
{
	QDomNode child = parent.firstChild();
	while (! child.isNull()) {
		if (child.nodeName() == nodeName) {
			return child;
		}
		child = child.nextSibling();
	}
	return child;
}
QDomNode iRIC::getChildNodeWithAttribute(const QDomNode& parent, const QString& nodeName, const QString& attName, const QString& attVal)
{
	QDomNode child = parent.firstChild();
	while (! child.isNull()) {
		if (child.nodeName() == nodeName && child.toElement().attribute(attName) == attVal) {
			return child;
		}
		child = child.nextSibling();
	}
	return child;
}

QString iRIC::getText(const QDomNode& node)
{
	QDomNode child = node.firstChild();
	while (! child.isNull()) {
		if (child.isText() || child.isCDATASection()) {
			return child.toText().data();
		}
		child = child.nextSibling();
	}
	return "";
}

void iRIC::removeAllComments(QDomNode* node)
{
	QDomNodeList children = node->childNodes();
	QList<QDomNode> commentNodes;
	for (int i = 0; i < children.count(); ++i) {
		QDomNode childNode = children.at(i);
		if (childNode.nodeType() == QDomNode::CommentNode) {
			commentNodes.append(childNode);
		} else {
			// search child node recursively.
			removeAllComments(&childNode);
		}
	}
	for (int i = 0; i < commentNodes.count(); ++i) {
		node->removeChild(commentNodes[i]);
	}
}

bool iRIC::getBooleanAttribute(const QDomNode& node, const QString& name, bool defaultVal)
{
	QString attVal = node.toElement().attribute(name);
	if (attVal == "") {return defaultVal;}
	return (attVal == "true" || attVal == "1");
}

void iRIC::setBooleanAttribute(QXmlStreamWriter& writer, const QString& name, bool value)
{
	QString valstr = "false";
	if (value) {valstr = "true";}
	writer.writeAttribute(name, valstr);
}

int iRIC::getIntAttribute(const QDomNode& node, const QString& name, int defaultVal)
{
	QString val = node.toElement().attribute(name);
	if (val == "") {return defaultVal;}
	bool ok;
	double intVal = val.toInt(&ok);
	if (! ok || std::isnan(intVal)) {intVal = defaultVal;}
	return intVal;
}

void iRIC::setIntAttribute(QXmlStreamWriter& writer, const QString& name, int value)
{
	writer.writeAttribute(name, QString::number(value));
}

double iRIC::getDoubleAttribute(const QDomNode& node, const QString& name, double defaultVal)
{
	QString val = node.toElement().attribute(name);
	if (val == "") {return defaultVal;}
	bool ok;
	double doubleVal = val.toDouble(&ok);
	if (! ok || std::isnan(doubleVal)) {doubleVal = defaultVal;}
	return doubleVal;
}

void iRIC::setDoubleAttribute(QXmlStreamWriter& writer, const QString& name, double value)
{
	writer.writeAttribute(name, QString::number(value));
}

QColor iRIC::getColorAttribute(const QDomNode& node, const QString& name, QColor defaultVal)
{
	QString colval = node.toElement().attribute(name);
	if (colval == "") {return defaultVal;}
	return QColor(colval);
}

void iRIC::setColorAttribute(QXmlStreamWriter& writer, const QString& name, const QColor& color)
{
	writer.writeAttribute(name, color.name());
}
