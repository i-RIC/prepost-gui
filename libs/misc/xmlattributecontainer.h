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
	/// Constructor
	XmlAttributeContainer();
	/// Copy constructor
	XmlAttributeContainer(const XmlAttributeContainer& c);
	/// Destructor
	virtual ~XmlAttributeContainer();
	//@}

	/// @name Operators
	//@{
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c);
	//@}

	/// @name XML file I/O functions
	//@{
	/// Load data from XML attributes
	virtual void load(const QDomNode&) = 0;
	/// Save data into XML node as attributes
	virtual void save(QXmlStreamWriter&) const = 0;
	//@}

	/// @name Property setting functions
	//@{
	/// Prefix for XML attributes
	virtual const QString& prefix() const;
	/// Set prefix for XML attributes
	virtual void setPrefix(const QString& prefix);
	/// Attributes name (Prefix added if set)
	virtual QString attName(const QString& name) const;
	//@}

private:
	virtual void copyValue(const XmlAttributeContainer& c);

	class Impl;
	Impl* m_impl;
};

#endif // XMLATTRIBUTECONTAINER_H
