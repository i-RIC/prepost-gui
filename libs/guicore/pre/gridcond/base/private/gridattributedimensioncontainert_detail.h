#ifndef GRIDATTRIBUTEDIMENSIONCONTAINERT_DETAIL_H
#define GRIDATTRIBUTEDIMENSIONCONTAINERT_DETAIL_H

#include "../gridattributedimensioncontainert.h"

template <class V>
GridAttributeDimensionContainerT<V>::GridAttributeDimensionContainerT(SolverDefinitionGridAttributeDimension* def) :
	GridAttributeDimensionContainer(def)
{}

template <class V>
GridAttributeDimensionContainerT<V>::~GridAttributeDimensionContainerT()
{}

template <class V>
const QList<V>& GridAttributeDimensionContainerT<V>::values() const
{
	return m_values;
}

template <class V>
QList<V>& GridAttributeDimensionContainerT<V>::values()
{
	return m_values;
}

template <class V>
void GridAttributeDimensionContainerT<V>::setValues(const QList<V>& vals)
{
	QList<QVariant> oldVals = variantValues();
	m_values = vals;
	QList<QVariant> newVals = variantValues();
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
QList<QVariant> GridAttributeDimensionContainerT<V>::variantValues() const
{
	QList<QVariant> ret;
	for (int i = 0; i < count(); ++i) {
		QVariant v = variantValue(i);
		ret.append(v);
	}
	return ret;
}

template <class V>
void GridAttributeDimensionContainerT<V>::setVariantValues(const QList<QVariant>& vals)
{
	QList<QVariant> currentVals = variantValues();
	if (currentVals == vals) {return;}
	QList<V> nativeVals;
	for (int i = 0; i < vals.size(); ++i) {
		nativeVals.append(fromVariant(vals.at(i)));
	}
	setValues(nativeVals);
}

template <class V>
int GridAttributeDimensionContainerT<V>::count() const
{
	return m_values.count();
}

template <class V>
bool GridAttributeDimensionContainerT<V>::loadFromExternalFile(const QString& filename)
{
	QFile f(filename);
	bool ok = f.open(QIODevice::ReadOnly);
	if (!ok) { return false; }
	QDataStream s(&f);
	int count;
	QList<V> vals;
	V value;
	s >> count;
	for (int i = 0; i < count; ++i) {
		s >> value;
		vals.append(value);
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
	if (!ok) { return false;}
	QDataStream s(&f);
	int count = m_values.size();
	s << count;
	for (int i = 0; i < count; ++i) {
		s << m_values.at(i);
	}
	f.close();
	return true;
}

template <class V>
bool GridAttributeDimensionContainerT<V>::loadFromCgnsFile(int fn, int B, int Z)
{
	// Goto "GridConditions" node.
	int ier;
	bool found = false;
	ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, gridAttribute()->name().c_str(), 0, "end");
	if (ier == 0) {
		// the corresponding node found.
		// Find "Dimension_(name)" array.
		QString targetName = QString("Dimension_%1").arg(name().c_str());
		int narrays;
		cg_narrays(&narrays);
		for (int i = 1; i <= narrays; ++i) {
			char arrayName[ProjectCgnsFile::BUFFERLEN];
			DataType_t dt;
			int dataDimension;
			cgsize_t dimensionVector[3];
			cgsize_t dataCount;
			cg_array_info(i, arrayName, &dt, &dataDimension, dimensionVector);
			if (dataDimension != 1) {return false;}
			if (dt == dataType() && QString(arrayName) == targetName) {
				// We've found the array!
				dataCount = dimensionVector[0];
				// load data.
				V* data = new V[dataCount];
				ier = cg_array_read(i, data);
				m_values.clear();
				for (cgsize_t j = 0; j < dataCount; ++j) {
					m_values.append(*(data + j));
				}
				delete[] data;
				found = true;
			}
		}
	}
	return found;
}

template <class V>
bool GridAttributeDimensionContainerT<V>::saveFromCgnsFile(int fn, int B, int Z)
{
	QString arrayName = QString("Dimension_%1").arg(name().c_str());

	int ier;
	// Goto "GridConditions" node.
	ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, gridAttribute()->name().c_str(), 0, "end");
	if (ier != 0) {return false;}
	// Delete the array if it already exists.
	cg_delete_node(const_cast<char*>(iRIC::toStr(arrayName).c_str()));
	// Create the "array" array
	int dataCount = count();
	cgsize_t dimensions = m_values.count();
	V* data = new V[dataCount];
	for (int i = 0; i < dataCount; ++i) {
		data[i] = m_values.at(i);
	}
	ier = cg_array_write(iRIC::toStr(arrayName).c_str(), dataType(), 1, &dimensions, data);
	delete[] data;
	if (ier != 0) {return false;}
	return true;
}

#endif // GRIDATTRIBUTEDIMENSIONCONTAINERT_DETAIL_H
