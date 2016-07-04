#ifndef XMLATTRIBUTECONTAINER_H
#define XMLATTRIBUTECONTAINER_H

#include "misc_global.h"

class QDomNode;
class QXmlStreamWriter;

/// Container class that has I/O function to store data as XML attributes.
class MISCDLL_EXPORT XmlAttributeContainer
{

public:
	/// @name Constructor and Destructor
	//@{
	XmlAttributeContainer();
	XmlAttributeContainer(const XmlAttributeContainer& c);
	virtual ~XmlAttributeContainer();
	//@}

	/// @name Operators
	//@{
	virtual XmlAttributeContainer& operator=(const XmlAttributeContainer& c);
	//@}

	/// @name XML file I/O functions
	//@{
	virtual void load(const QDomNode&) = 0;
	virtual void save(QXmlStreamWriter&) const = 0;
	//@}

	/// @name Property setting functions
	//@{
	virtual const QString& prefix() const;
	virtual void setPrefix(const QString& prefix);
	virtual QString attName(const QString& name) const;
	//@}

private:
	virtual void copyValue(const XmlAttributeContainer& c);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/xmlattributecontainer_impl.h"
#endif // _DEBUG

#endif // XMLATTRIBUTECONTAINER_H
