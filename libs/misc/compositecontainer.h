#ifndef COMPOSITECONTAINER_H
#define COMPOSITECONTAINER_H

#include "misc_global.h"
#include "xmlattributecontainer.h"

#include <initializer_list>

class MISCDLL_EXPORT CompositeContainer : public XmlAttributeContainer
{
public:
	virtual ~CompositeContainer();

	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	/// @name XML file I/O functions
	//@{
	void load(const QDomNode& node);
	virtual void save(QXmlStreamWriter& writer) const;
	//@}

	/// @name Property setting functions
	//@{
	/// Set prefix for XML attributes
	void setPrefix(const QString& prefix) override;
	//@}

protected:
	/// @name Protected constructors
	//@{
	CompositeContainer(std::initializer_list<XmlAttributeContainer*> list);
	CompositeContainer() = delete;
	CompositeContainer(const CompositeContainer& c) = delete;
	//@}

	void copyValue(const XmlAttributeContainer& c) override;

	void addContainer(XmlAttributeContainer* c);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/compositecontainer_impl.h"
#endif // _DEBUG

#endif // COMPOSITECONTAINER_H
