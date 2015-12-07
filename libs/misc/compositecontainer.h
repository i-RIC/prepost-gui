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
	/// Constructor (copy)
	CompositeContainer(const CompositeContainer& c) = delete;
	//@}

	void copyValue(const XmlAttributeContainer& c) override;

private:
	class Impl;
	Impl* m_impl;
};

#ifdef _DEBUG
	#include "private/compositecontainer_impl.h"
#endif // _DEBUG

#endif // COMPOSITECONTAINER_H
