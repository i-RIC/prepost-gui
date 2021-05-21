#ifndef GRIDATTRIBUTEDIMENSIONCONTAINERT_DETAIL_H
#define GRIDATTRIBUTEDIMENSIONCONTAINERT_DETAIL_H

#include "../gridattributedimensioncontainert.h"

#include <h5cgnsgridattributes.h>

#include <QFile>

template <class V>
GridAttributeDimensionContainerT<V>::GridAttributeDimensionContainerT(SolverDefinitionGridAttributeDimension* def) :
	GridAttributeDimensionContainer(def)
{}

template <class V>
GridAttributeDimensionContainerT<V>::~GridAttributeDimensionContainerT()
{}

template <class V>
const std::vector<V> &GridAttributeDimensionContainerT<V>::values() const
{
	return m_values;
}

template <class V>
std::vector<V> &GridAttributeDimensionContainerT<V>::values()
{
	return m_values;
}

template <class V>
void GridAttributeDimensionContainerT<V>::setValues(const std::vector<V> &vals)
{
	std::vector<QVariant> oldVals = variantValues();
	m_values = vals;
	std::vector<QVariant> newVals = variantValues();
	emit valuesChanged();
	emit valuesChanged(oldVals, newVals);
}

template <class V>
V GridAttributeDimensionContainerT<V>::value(int index) const
{
	if (index < 0) {return 0;}
	if (index >= m_values.size()) {return 0;}
	return m_values.at(index);
}

template <class V>
V GridAttributeDimensionContainerT<V>::fromVariant(const QVariant& val)
{
	return val.value<V>();
}

template <class V>
QVariant GridAttributeDimensionContainerT<V>::toVariant(V val) const
{
	return QVariant(val);
}

template <class V>
QVariant GridAttributeDimensionContainerT<V>::variantValue(int index) const
{
	return QVariant(value(index));
}

template <class V>
std::vector<QVariant> GridAttributeDimensionContainerT<V>::variantValues() const
{
	std::vector<QVariant> ret;
	ret.reserve(count());
	for (int i = 0; i < count(); ++i) {
		QVariant v = variantValue(i);
		ret.push_back(v);
	}
	return ret;
}

template <class V>
void GridAttributeDimensionContainerT<V>::setVariantValues(const std::vector<QVariant> &vals)
{
	std::vector<QVariant> currentVals = variantValues();
	if (currentVals == vals) {return;}
	std::vector<V> nativeVals;
	for (int i = 0; i < vals.size(); ++i) {
		nativeVals.push_back(fromVariant(vals.at(i)));
	}
	setValues(nativeVals);
}

template <class V>
int GridAttributeDimensionContainerT<V>::count() const
{
	return static_cast<int> (m_values.size());
}

template <class V>
bool GridAttributeDimensionContainerT<V>::loadFromExternalFile(const QString& filename)
{
	QFile f(filename);
	bool ok = f.open(QIODevice::ReadOnly);
	if (! ok) {return false;}
	QDataStream s(&f);
	int count;
	std::vector<V> vals;
	V value;
	s >> count;
	vals.reserve(count);
	for (int i = 0; i < count; ++i) {
		s >> value;
		vals.push_back(value);
	}
	f.close();
	setValues(vals);
	return true;
}

template <class V>
bool GridAttributeDimensionContainerT<V>::saveToExternalFile(const QString& filename)
{
	QFile f(filename);
	bool ok = f.open(QIODevice::WriteOnly);
	if (! ok) {return false;}
	QDataStream s(&f);
	int count = static_cast<int> (m_values.size());
	s << count;
	for (int i = 0; i < count; ++i) {
		s << m_values.at(i);
	}
	f.close();
	return true;
}

template <class V>
int GridAttributeDimensionContainerT<V>::loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts)
{
	return atts.readFunctionalDimension(gridAttribute()->name(), name(), &m_values);
}

template <class V>
int GridAttributeDimensionContainerT<V>::saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts)
{
	return atts->writeFunctionalDimension(gridAttribute()->name(), name(), m_values);
}

#endif // GRIDATTRIBUTEDIMENSIONCONTAINERT_DETAIL_H
