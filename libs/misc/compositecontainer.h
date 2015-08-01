#ifndef COMPOSITECONTAINER_H
#define COMPOSITECONTAINER_H

#include "misc_global.h"
#include "xmlattributecontainer.h"

#include <initializer_list>

class MISCDLL_EXPORT CompositeContainer : public XmlAttributeContainer
{

public:
	/// Destructor
	virtual ~CompositeContainer();

	/// @name XML file I/O functions
	//@{
	/// Load data from XML attributes
	void load(const QDomNode&);
	/// Save data into XML node as attributes
	virtual void save(QXmlStreamWriter&) const;
	//@}

	/// @name Property setting functions
	//@{
	/// Set prefix for XML attributes
	void setPrefix(const QString& prefix) override;
	//@}

protected:
	/// @name Protected constructors
	//@{
	/// Constructor (initializer list)
	CompositeContainer(std::initializer_list<XmlAttributeContainer*> list);
	//@}

private:
	void copyValue(const XmlAttributeContainer& c) override;

	class Impl;
	Impl* m_impl;
};

#endif // COMPOSITECONTAINER_H
