#ifndef XMLATTRIBUTECONTAINER_H
#define XMLATTRIBUTECONTAINER_H

#include "misc_global.h"

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

/// Container class that has I/O function to store data as xml attributes.
class MISCDLL_EXPORT XmlAttributeContainer
{
public:
	XmlAttributeContainer();
	virtual ~XmlAttributeContainer();

	virtual void load(const QDomNode&) = 0;
	virtual void save(QXmlStreamWriter& ) const = 0;
	const QString& prefix() const {return m_prefix;}
	void setPrefix(const QString& prefix)
	{
		m_prefix = prefix;
	}
	const QString attName(const QString& name) const;

protected:
	QString m_prefix;

};

#endif // XMLATTRIBUTECONTAINER_H
