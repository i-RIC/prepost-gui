#include "compositecontainer.h"
#include "private/compositecontainer_impl.h"

CompositeContainer::~CompositeContainer()
{
	delete impl;
}

XmlAttributeContainer& CompositeContainer::operator=(const XmlAttributeContainer& c)
{
	copyValue(dynamic_cast<const CompositeContainer&> (c));
	return *this;
}

void CompositeContainer::load(const QDomNode& node)
{
	for (XmlAttributeContainer* c : impl->m_containers) {
		c->load(node);
	}
}

void CompositeContainer::save(QXmlStreamWriter& writer) const
{
	for (XmlAttributeContainer* c : impl->m_containers) {
		c->save(writer);
	}
}

void CompositeContainer::setPrefix(const QString& prefix)
{
	XmlAttributeContainer::setPrefix(prefix);
	for (XmlAttributeContainer* c : impl->m_containers) {
		c->setPrefix(prefix);
	}
}

CompositeContainer::CompositeContainer(std::initializer_list<XmlAttributeContainer*> list) :
	XmlAttributeContainer {},
	impl {new Impl {}}
{
	for (XmlAttributeContainer* c : list) {
		impl->m_containers.push_back(c);
	}
}

void CompositeContainer::copyValue(const XmlAttributeContainer& c)
{
	const CompositeContainer& c2 = dynamic_cast<const CompositeContainer&> (c);
	for (size_t i = 0; i < impl->m_containers.size(); ++i){
		XmlAttributeContainer* v1 = impl->m_containers.at(i);
		XmlAttributeContainer* v2 = c2.impl->m_containers.at(i);
		*(v1) = *(v2);
	}
}

void CompositeContainer::addContainer(XmlAttributeContainer* c)
{
	impl->m_containers.push_back(c);
}

const std::vector<XmlAttributeContainer*>& CompositeContainer::containers() const
{
	return impl->m_containers;
}
