#ifndef GRIDATTRIBUTECONTAINERT_DETAIL_H
#define GRIDATTRIBUTECONTAINERT_DETAIL_H

#include "../gridattributecontainert.h"
#include "../gridattributedimensionscontainer.h"
#include "../../../../project/projectcgnsfile.h"

#include <misc/stringtool.h>
#include <misc/filesystemfunction.h>

#include <QDataStream>
#include <QFile>
#include <QFileInfo>

#include <h5cgnsgridattributes.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

template <class V>
GridAttributeContainerT<V>::GridAttributeContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeContainer(grid, cond)
{}

template <class V>
GridAttributeContainerT<V>::~GridAttributeContainerT()
{}

template <class V>
int GridAttributeContainerT<V>::loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts)
{
	GridAttributeDimensionsContainer* dims = dimensions();
	int ier = 0;
	if (dims == 0 || dims->containers().size() == 0) {
		ier = loadFromCgnsFileForIndex(atts, 0);
	} else {
		for (int index = 0; index <= dims->maxIndex(); ++index) {
			ier = loadFromCgnsFileForIndex(atts, index);
			if (ier == IRIC_NO_ERROR) {
				auto fileName = temporaryExternalFilename(index);
				QFileInfo finfo(fileName);
				iRIC::mkdirRecursively(finfo.absolutePath());
				bool ok = saveToExternalFile(fileName);
				if (! ok) {return IRIC_FILE_COPY_FAIL;}
			}
		}
	}
	if (ier == IRIC_NO_ERROR) {
		setMapped(true);
	}
	return ier;
}

template <class V>
int GridAttributeContainerT<V>::loadFromCgnsFileForIndex(const iRICLib::H5CgnsGridAttributes& atts, int index)
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

template <class V>
int GridAttributeContainerT<V>::saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts)
{
	GridAttributeDimensionsContainer* dims = dimensions();

	// output dimension values
	for (auto dim : dims->containers()) {
		int ier = dim->saveToCgnsFile(atts);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	if (dims->containers().size() == 0) {
		return saveToCgnsFileForIndex(atts, 0);
	} else {
		for (int index = 0; index <= dims->maxIndex(); ++index) {
			bool ok = loadFromExternalFile(temporaryExternalFilename(index));
			if (ok) {
				int ier = saveToCgnsFileForIndex(atts, index);
				if (ier != IRIC_NO_ERROR) {return ier;}
			}
		}
		return IRIC_NO_ERROR;
	}
}

template <class V>
int GridAttributeContainerT<V>::saveToCgnsFileForIndex(iRICLib::H5CgnsGridAttributes* atts, int index)
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

template <class V>
void GridAttributeContainerT<V>::setModified()
{
	dataArray()->Modified();
}

template <class V>
bool GridAttributeContainerT<V>::loadFromExternalFile(const QString& filename)
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

template <class V>
bool GridAttributeContainerT<V>::saveToExternalFile(const QString& filename)
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

template <class V>
bool GridAttributeContainerT<V>::getValueRange(double* min, double* max)
{
	double range[2];
	dataArray()->GetRange(range);
	*min = range[0];
	*max = range[1];
	return true;
}

#endif // GRIDATTRIBUTECONTAINERT_DETAIL_H
