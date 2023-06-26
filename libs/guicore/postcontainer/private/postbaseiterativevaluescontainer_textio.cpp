#include "postbaseiterativevaluescontainer_basecontainer.h"
#include "postbaseiterativevaluescontainer_impl.h"
#include "postbaseiterativevaluescontainer_integervaluecontainer.h"
#include "postbaseiterativevaluescontainer_realvaluecontainer.h"
#include "postbaseiterativevaluescontainer_textio.h"

#include <misc/stringtool.h>

#include <QTextStream>

namespace {

QTextStream& operator>>(QTextStream& stream, PostBaseIterativeValuesContainer::BaseContainer& container)
{
	QString header;
	int baseId;
	stream >> header >> baseId;
	container.setBaseId(baseId);

	int dataCount;
	stream >> header >> dataCount;
	stream.readLine();

	QStringList names, types;
	names = stream.readLine().split(",");
	types = stream.readLine().split(",");

	for (int i = 0; i < names.size(); ++i) {
		QString name = names.at(i);
		QString type = types.at(i);

		if (type == "int") {
			auto c = new PostBaseIterativeValuesContainer::IntegerValueContainer(iRIC::toStr(name));
			container.addContainer(c);
		} else if (type == "real") {
			auto c = new PostBaseIterativeValuesContainer::RealValueContainer(iRIC::toStr(name));
			container.addContainer(c);
		}
	}
	for (int i = 0 ; i < dataCount; ++i) {
		QStringList vals = stream.readLine().split(",");
		int idx = 0;
		for (auto c : container.integerContainers()) {
			int val = vals.at(idx).toInt();
			c->values().push_back(val);
			++ idx;
		}
		for (auto c : container.realContainers()) {
			double val = vals.at(idx).toDouble();
			c->values().push_back(val);
			++ idx;
		}
	}
	return stream;
}

QTextStream& operator<<(QTextStream& stream, PostBaseIterativeValuesContainer::BaseContainer& container)
{
	stream << "BaseID: " << container.baseId() << endl;

	int dataCount = container.dataCount();
	stream << "DataCount: " << dataCount << endl;
	QStringList vals;

	for (auto c : container.integerContainers()) {
		vals.push_back(c->name().c_str());
	}
	for (auto c : container.realContainers()) {
		vals.push_back(c->name().c_str());
	}
	stream << vals.join(",") << endl;

	vals.clear();
	for (auto c : container.integerContainers()) {
		vals.push_back("int");
	}
	for (auto c : container.realContainers()) {
		vals.push_back("real");
	}
	stream << vals.join(",") << endl;

	for (int i = 0; i < dataCount; ++i) {
		QStringList vals;
		for (auto c : container.integerContainers()) {
			vals << QString::number(c->values().at(i));
		}
		for (auto c : container.realContainers()) {
			vals << QString::number(c->values().at(i), 'g', 10);
		}
		stream << vals.join(",") << endl;
	}

	return stream;
}

} // namespace


PostBaseIterativeValuesContainer::TextIO::TextIO(PostBaseIterativeValuesContainer* container) :
	PostBaseIterativeValuesContainer::IO (container)
{}

void PostBaseIterativeValuesContainer::TextIO::load()
{
	m_container->clear();
	if (! m_file.exists() || m_file.size() == 0) {return;}

	bool ok = m_file.open(QIODevice::ReadOnly);
	if (! ok) {return;}

	QTextStream stream(&m_file);
	QString header;
	int baseNum;
	stream >> header >> baseNum;
	for (int i = 0; i < baseNum; ++i) {
		auto c = new BaseContainer();
		stream >> *c;
		m_container->impl->m_baseContainers.push_back(c);
	}
	m_file.close();
}

bool PostBaseIterativeValuesContainer::TextIO::save()
{
	bool ok = m_file.open(QIODevice::WriteOnly);
	if (! ok) {return false;}

	QTextStream stream(&m_file);
	int baseNum = static_cast<int> (m_container->impl->m_baseContainers.size());
	stream << "BaseCount: " << baseNum << endl;
	for (auto c : m_container->impl->m_baseContainers) {
		stream << *c;
	}
	m_file.close();
	return true;
}
