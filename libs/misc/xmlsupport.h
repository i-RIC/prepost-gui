#ifndef XMLSUPPORT_H
#define XMLSUPPORT_H

#include "misc_global.h"
#include <Qt>
#include <QColor>

class QColor;
class QDomNode;
class QString;
class QXmlStreamWriter;

namespace iRIC
{
	/// Get the first child node that has the name nodeName.
	QDomNode MISCDLL_EXPORT getChildNode(const QDomNode& parent, const QString& nodeName);
	/// Get the fisrt child node that has the specified attribute.
	QDomNode MISCDLL_EXPORT getChildNodeWithAttribute(const QDomNode& parent, const QString& nodeName, const QString& attName, const QString& attVal);
	/// Get the text included in the node.
	QString MISCDLL_EXPORT getText(const QDomNode& node);
	/// Remove all comment nodes under the specified node.
	void MISCDLL_EXPORT removeAllComments(QDomNode* node);
	bool MISCDLL_EXPORT getBooleanAttribute(const QDomNode& node, const QString& name, bool defaultVal = false);
	void MISCDLL_EXPORT setBooleanAttribute(QXmlStreamWriter& writer, const QString& name, bool value);
	int MISCDLL_EXPORT getIntAttribute(const QDomNode& node, const QString& name, int defaultVal = 0);
	void MISCDLL_EXPORT setIntAttribute(QXmlStreamWriter& writer, const QString& name, int value);
	double MISCDLL_EXPORT getDoubleAttribute(const QDomNode& node, const QString& name, double defaultVal = 0);
	void MISCDLL_EXPORT setDoubleAttribute(QXmlStreamWriter& writer, const QString& name, double value);
	QColor MISCDLL_EXPORT getColorAttribute(const QDomNode& node, const QString& name, QColor defaultVal = QColor(Qt::black));
	void MISCDLL_EXPORT setColorAttribute(QXmlStreamWriter& writer, const QString& name, const QColor& color);
}

#endif // XMLSUPPORT_H
