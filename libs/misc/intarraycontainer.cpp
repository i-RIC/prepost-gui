#include "intarraycontainer.h"

#include <QDomElement>
#include <QStringList>
#include <QXmlStreamWriter>

IntArrayContainer::IntArrayContainer(const QString& name) :
	ValueArrayContainerT<int> {name}
{}

IntArrayContainer::IntArrayContainer(const QString& name, const std::vector<int>& vals) :
	ValueArrayContainerT<int> {name, vals}
{}

IntArrayContainer::IntArrayContainer(const IntArrayContainer& c) :
	ValueArrayContainerT<int> {c}
{}

IntArrayContainer::~IntArrayContainer()
{}

void IntArrayContainer::load(const QDomNode& node)
{
	m_value.clear();

	auto elem = node.toElement();
	QStringList frag = elem.attribute(m_name).split(",");
	for (const QString& f : frag) {
		bool ok;
		int val;
		val = f.toInt(&ok);
		if (! ok) {continue;}
		m_value.push_back(val);
	}
}

void IntArrayContainer::save(QXmlStreamWriter& writer) const
{
	QStringList strVals;
	for (int v : m_value) {
		strVals.append(QString::number(v));
	}
	writer.writeAttribute(m_name, strVals.join(","));
}

IntArrayContainer& IntArrayContainer::operator=(const std::vector<int>& val)
{
	ValueArrayContainerT<int>::operator =(val);
	return *this;
}

void IntArrayContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const IntArrayContainer&> (c);
	m_value = c2.m_value;
}
