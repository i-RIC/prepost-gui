#include "postbaseiterativevaluescontainer_binaryio.h"
#include "postbaseiterativevaluescontainer_basecontainer.h"
#include "postbaseiterativevaluescontainer_impl.h"
#include "postbaseiterativevaluescontainer_integervaluecontainer.h"
#include "postbaseiterativevaluescontainer_realvaluecontainer.h"

#include <misc/stringtool.h>

#include <QDataStream>

namespace {

template<typename V>
QDataStream& operator>>(QDataStream& stream, std::vector<V>& vector)
{
	int num;
	V val;
	stream >> num;

	vector.clear();
	vector.reserve(num);
	for (int i = 0; i < num; ++i) {
		stream >> val;
		vector.push_back(val);
	}

	return stream;
}

template<typename V>
QDataStream& operator<<(QDataStream& stream, std::vector<V>& vector)
{
	int num = static_cast<int>(vector.size());
	stream << num;

	for (int i = 0; i < num; ++i) {
		stream << vector.at(i);
	}
	return stream;
}

template<typename V>
QDataStream& operator>>(QDataStream& stream, PostBaseIterativeValuesContainer::ValueContainerT<V>& c)
{
	QString name;
	int arrayId;
	stream >> name;

	c.setName(iRIC::toStr(name));

	std::vector<int> vals;
	stream >> c.values();

	return stream;
}

template<typename V>
QDataStream& operator<<(QDataStream& stream, PostBaseIterativeValuesContainer::ValueContainerT<V>& c)
{
	QString name = c.name().c_str();
	stream << name;
	stream << c.values();

	return stream;
}

QDataStream& operator>>(QDataStream& stream, PostBaseIterativeValuesContainer::BaseContainer& container)
{
	int baseId;
	stream >> baseId;
	container.setBaseId(baseId);

	int numContainers;

	stream >> numContainers;
	for (int i = 0; i < numContainers; ++i) {
		auto c = new PostBaseIterativeValuesContainer::IntegerValueContainer();
		stream >> *c;
		container.addContainer(c);
	}
	stream >> numContainers;
	for (int i = 0; i < numContainers; ++i) {
		auto c = new PostBaseIterativeValuesContainer::RealValueContainer();
		stream >> *c;
		container.addContainer(c);
	}

	return stream;
}

QDataStream& operator<<(QDataStream& stream, PostBaseIterativeValuesContainer::BaseContainer& container)
{
	stream << container.baseId();

	stream << static_cast<int> (container.integerContainers().size());
	for (auto c : container.integerContainers()) {
		stream << *c;
	}
	stream << static_cast<int> (container.realContainers().size());
	for (auto c : container.realContainers()) {
		stream << *c;
	}
	return stream;
}

} // namespace

PostBaseIterativeValuesContainer::BinaryIO::BinaryIO(PostBaseIterativeValuesContainer* container) :
	PostBaseIterativeValuesContainer::IO (container)
{}

void PostBaseIterativeValuesContainer::BinaryIO::load()
{
	m_container->clear();
	if (! m_file.exists() || m_file.size() == 0) {return;}

	bool ok = m_file.open(QIODevice::ReadOnly);
	if (! ok) {return;}

	QDataStream stream(&m_file);
	int baseNum;
	stream >> baseNum;
	for (int i = 0; i < baseNum; ++i) {
		auto c = new BaseContainer();
		stream >> *c;
		m_container->impl->m_baseContainers.push_back(c);
	}
	m_file.close();
}

bool PostBaseIterativeValuesContainer::BinaryIO::save()
{
	bool ok = m_file.open(QIODevice::WriteOnly);
	if (! ok) {return false;}

	QDataStream stream(&m_file);
	int baseNum = static_cast<int> (m_container->impl->m_baseContainers.size());
	stream << baseNum;
	for (auto c : m_container->impl->m_baseContainers) {
		stream << *c;
	}
	m_file.close();
	return true;
}
