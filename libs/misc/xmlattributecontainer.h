#ifndef XMLATTRIBUTECONTAINER_H
#define XMLATTRIBUTECONTAINER_H

#include "misc_global.h"

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

/// Container class that has I/O function to store data as XML attributes.
class MISCDLL_EXPORT XmlAttributeContainer
{

public:
	XmlAttributeContainer();
	virtual ~XmlAttributeContainer();
	/// Load data from XML attributes
	virtual void load(const QDomNode&) = 0;
	/// Save data into XML node as attributes
	virtual void save(QXmlStreamWriter&) const = 0;
	/// Prefix for XML attributes
	const QString& prefix() const {return m_prefix;}
	/// Set prefix for XML attributes
	void setPrefix(const QString& prefix) {
		m_prefix = prefix;
	}
	/// Attributes name (Prefix added if set)
	const QString attName(const QString& name) const;

protected:
	QString m_prefix {};
};

#endif // XMLATTRIBUTECONTAINER_H
