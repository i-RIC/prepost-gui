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

CompositeContainer& CompositeContainer::operator=(const CompositeContainer& c)
{
	for (size_t i = 0; i < m_impl->m_containers.size(); ++i){
		XmlAttributeContainer* c1 = m_impl->m_containers.at(i);
		XmlAttributeContainer* c2 = c.m_impl->m_containers.at(i);
		*(c1) = *(c2);
	}
	return *this;
}
