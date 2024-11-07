#ifndef GRIDATTRIBUTECONTAINERT_DETAIL_H
#define GRIDATTRIBUTECONTAINERT_DETAIL_H

#include "../gridattributecontainerioi.h"
#include "../gridattributecontainert.h"
#include "../gridattributedimensionscontainer.h"
#include "../../../../project/projectcgnsfile.h"
#include "../../../../solverdef/solverdefinitiongridattribute.h"

#include <misc/stringtool.h>
#include <misc/filesystemfunction.h>

#include <QDataStream>
#include <QFile>
#include <QFileInfo>

#include <vtkDataSetAttributes.h>

#include <h5cgnsgridattributes.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

template <class V, class DA>
GridAttributeContainerT<V, DA>::GridAttributeContainerT(v4InputGrid* grid, GridAttributeContainerIoI* io, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeContainer(grid, io, cond)
{
	addArrayIfNeeded();
}

template <class V, class DA>
GridAttributeContainerT<V, DA>::~GridAttributeContainerT()
{}

template <class V, class DA>
V GridAttributeContainerT<V, DA>::value(unsigned int index) const
{
	return dataArray()->GetValue(index);
}

template <class V, class DA>
void GridAttributeContainerT<V, DA>::setValue(unsigned int index, V value)
{
	dataArray()->SetValue(index, value);
}

template <class V, class DA>
int GridAttributeContainerT<V, DA>::loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts)
{
	if (m_io == nullptr) {
		int ier = loadFromCgnsFileForIndex(atts, 0);
		if (ier == IRIC_NO_ERROR) {
			setMapped(true);
		}
		return ier;
	} else {
		return m_io->loadFromCgnsFile(atts, this);
	}
}

template <class V, class DA>
int GridAttributeContainerT<V, DA>::loadFromCgnsFileForIndex(const iRICLib::H5CgnsGridAttributes& atts, int index)
{
	std::vector<V> buffer;
	int ier = atts.readFunctional(name(), index + 1, &buffer);
	if (ier == IRIC_NO_ERROR) {
		for (unsigned int i = 0; i < dataCount() && i < buffer.size(); ++i) {
			setValue(i, buffer[i]);
		}
	} else {
		// use default value
		auto count = dataCount();
		auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>*>(gridAttribute());

		V defaultVal = cond->defaultValue();
		for (unsigned int i = 0; i < count; ++i) {
			setValue(i, defaultVal);
		}
	}
	return IRIC_NO_ERROR;
}

template <class V, class DA>
int GridAttributeContainerT<V, DA>::saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts)
{
	if (m_io == nullptr) {
		return IRIC_NO_ERROR;
	}

	m_io->saveToCgnsFile(atts, this);
}

template <class V, class DA>
int GridAttributeContainerT<V, DA>::saveToCgnsFileForIndex(iRICLib::H5CgnsGridAttributes* atts, int index)
{
	int ier;
	auto count = dataCount();
	std::vector<V> data(count, 0);
	for (unsigned int i = 0; i < count; ++i) {
		data[i] = value(i);
	}
	ier = atts->writeFunctional(name(), index + 1, data);
	if (ier != 0) {return ier;}

	return IRIC_NO_ERROR;
}

template <class V, class DA>
void GridAttributeContainerT<V, DA>::setDefaultValue()
{
	V defaultValue = gridAttribute()->variantDefaultValue().value<V>();
	for (unsigned int i = 0; i < dataCount(); ++i) {
		setValue(i, defaultValue);
	}
}

template <class V, class DA>
DA* GridAttributeContainerT<V, DA>::dataArray() const
{
	return DA::SafeDownCast(vtkAttributes()->GetArray(name().c_str()));
}

template <class V, class DA>
DA* GridAttributeContainerT<V, DA>::dataArrayCopy() const
{
	DA* copy = DA::New();
	copy->DeepCopy(dataArray());
	return copy;
}

template <class V, class DA>
void GridAttributeContainerT<V, DA>::allocate()
{
	addArrayIfNeeded();

	auto da = dataArray();
	auto count = dataCount();
	da->Allocate(count);
	for (unsigned int i = 0; i < count; ++i) {
		da->InsertNextValue(0);
	}
}

template <class V, class DA>
void GridAttributeContainerT<V, DA>::setModified()
{
	dataArray()->Modified();
}

template <class V, class DA>
bool GridAttributeContainerT<V, DA>::loadFromExternalFile(const QString& filename)
{
	QFile f(filename);
	bool ok = f.open(QIODevice::ReadOnly);
	if (!ok) {return false;}
	QDataStream s(&f);

	unsigned int count = dataCount();
	for (unsigned int i = 0; i < count; ++i) {
		V value;
		s >> value;
		setValue(i, value);
	}
	f.close();
	return true;
}

template <class V, class DA>
bool GridAttributeContainerT<V, DA>::saveToExternalFile(const QString& filename)
{
	QFile f(filename);
	bool ok = f.open(QIODevice::WriteOnly);
	if (!ok) { return false;}
	QDataStream s(&f);

	unsigned int count = dataCount();
	for (unsigned int i = 0; i < count; ++i) {
		V val = value(i);
		s << val;
	}
	f.close();
	return true;
}

template <class V, class DA>
bool GridAttributeContainerT<V, DA>::getValueRange(double* min, double* max)
{
	double range[2];
	dataArray()->GetRange(range);
	*min = range[0];
	*max = range[1];
	return true;
}

template <class V, class DA>
void GridAttributeContainerT<V, DA>::addArrayIfNeeded()
{
	bool needed = false;
	auto a = vtkAttributes()->GetArray(name().c_str());
	if (a == nullptr) {
		needed = true;
	} else {
		auto da = DA::SafeDownCast(a);
		needed = (da == nullptr);
	}

	if (needed) {
		DA* da = DA::New();
		da->SetName(name().c_str());
		vtkAttributes()->AddArray(da);
		da->Delete();
	}
}

#endif // GRIDATTRIBUTECONTAINERT_DETAIL_H
