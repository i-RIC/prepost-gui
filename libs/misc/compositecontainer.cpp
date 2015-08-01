#include "compositecontainer.h"
#include <vector>

class CompositeContainer::Impl
{
public:
	std::vector<XmlAttributeContainer*> m_containers {};
};

CompositeContainer::~CompositeContainer()
{
	delete m_impl;
}

void CompositeContainer::load(const QDomNode& node)
{
	for (XmlAttributeContainer* c : m_impl->m_containers) {
		c->load(node);
	}
}

void CompositeContainer::save(QXmlStreamWriter& writer) const
{
	for (XmlAttributeContainer* c : m_impl->m_containers) {
		c->save(writer);
	}
}

void CompositeContainer::setPrefix(const QString& prefix)
{
	XmlAttributeContainer::setPrefix(prefix);
	for (XmlAttributeContainer* c : m_impl->m_containers) {
		c->setPrefix(prefix);
	}
}

CompositeContainer::CompositeContainer(std::initializer_list<XmlAttributeContainer*> list) :
	XmlAttributeContainer {},
	m_impl {new Impl {}}
{
	for (XmlAttributeContainer* c : list) {
		m_impl->m_containers.push_back(c);
	}
}

void CompositeContainer::copyValue(const XmlAttributeContainer& c)
{
	const CompositeContainer& c2 = dynamic_cast<const CompositeContainer&> (c);
	for (size_t i = 0; i < m_impl->m_containers.size(); ++i){
		XmlAttributeContainer* v1 = m_impl->m_containers.at(i);
		XmlAttributeContainer* v2 = c2.m_impl->m_containers.at(i);
		*(v1) = *(v2);
	}
}
