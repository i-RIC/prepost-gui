#include "intvectorcontainer.h"

#include <QDomNode>
#include <QStringList>
#include <QXmlStreamWriter>

IntVectorContainer::IntVectorContainer(const QString& name) :
	SimpleValueContainerT<std::vector<int> > {name}
{}

IntVectorContainer::IntVectorContainer(const QString& name, const std::vector<int>& defaultVal) :
	SimpleValueContainerT<std::vector<int> > {name, defaultVal}
{}

IntVectorContainer::IntVectorContainer(const IntVectorContainer& c) :
	SimpleValueContainerT<std::vector<int> > {c}
{}

IntVectorContainer::~IntVectorContainer()
{}

void IntVectorContainer::load(const QDomNode& node)
{
	QString vals = node.toElement().attribute(attName(m_name), "");
	QStringList valList = vals.split(',');

	std::vector<int> newVals;
	newVals.reserve(valList.size());

	for (int i = 0; i < valList.size(); ++i) {
		int val = valList.at(i).toInt();
		newVals.push_back(val);
	}
	m_value = newVals;
}

void IntVectorContainer::save(QXmlStreamWriter& writer) const
{
	QStringList valList;
	for (int i = 0; i < m_value.size(); ++i) {
		valList.push_back(QString::number(m_value[i]));
	}
	writer.writeAttribute(attName(m_name), valList.join(','));
}

IntVectorContainer& IntVectorContainer::operator=(const std::vector<int>& val)
{
	SimpleValueContainerT<std::vector<int> >::operator =(val);
	return *this;
}

void IntVectorContainer::copyValue(const XmlAttributeContainer& c)
{
	const IntVectorContainer& c2 = dynamic_cast<const IntVectorContainer&> (c);
	m_value = c2.m_value;
}
