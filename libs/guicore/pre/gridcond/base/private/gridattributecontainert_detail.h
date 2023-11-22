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

template <class V>
GridAttributeContainerT<V>::GridAttributeContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeContainer(grid, cond)
{}

template <class V>
GridAttributeContainerT<V>::~GridAttributeContainerT()
{}

template <class V>
bool GridAttributeContainerT<V>::loadFromCgnsFile(int fn, int B, int Z)
{
	GridAttributeDimensionsContainer* dims = dimensions();
	bool allok = true;
	if (dims == 0 || dims->containers().size() == 0) {
		allok = loadFromCgnsFileForIndex(fn, B, Z, 0);
	} else {
		for (int index = 0; index <= dims->maxIndex(); ++index) {
			bool ok = loadFromCgnsFileForIndex(fn, B, Z, index);
			if (ok) {
				auto fileName = temporaryExternalFilename(index);
				QFileInfo finfo(fileName);
				iRIC::mkdirRecursively(finfo.absolutePath());
				ok = saveToExternalFile(fileName);
			}
			allok = allok && ok;
		}
	}
	if (allok) {
		setMapped(true);
	}
	return allok;
}

template <class V>
bool GridAttributeContainerT<V>::loadFromCgnsFileForIndex(int fn, int B, int Z, int index)
{
	// Goto "GridConditions" node.
	int ier;
	bool found = false;
	cgsize_t count = dataCount();
	ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, name().c_str(), 0, "end");
	QString aName = arrayNameForIndex(index);
	if (ier == 0) {
		// the corresponding node found.
		// Find "Value" array.
		int narrays;
		cg_narrays(&narrays);
		for (int i = 1; i <= narrays; ++i) {
			char arrayName[ProjectCgnsFile::BUFFERLEN];
			DataType_t dt;
			int dataDimension;
			cgsize_t dimensionVector[3];
			cg_array_info(i, arrayName, &dt, &dataDimension, dimensionVector);

			if (dataDimension != 1 || dimensionVector[0] != count) { continue; }

			if (dt == dataType() && QString(arrayName) == aName) {
				std::vector<V> data(count, 0);
				ier = cg_array_read(i, data.data());
				for (cgsize_t j = 0; j < count; ++j) {
					setValue(j, data[j]);
				}
				found = true;
				break;
			} else if (dt == RealSingle && dataType() == RealDouble && QString(arrayName) == aName) {
				std::vector<float> data(count, 0);
				ier = cg_array_read(i, data.data());
				for (cgsize_t j = 0; j < count; ++j) {
					setValue(j, data[j]);
				}
				found = true;
				break;
			}
		}
	}
	if (! found) {
		// not found.
		// use default value.
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>*>(gridAttribute());

		V defaultVal = cond->defaultValue();
		for (cgsize_t j = 0; j < count; ++j) {
			setValue(j, defaultVal);
		}
	}
	return true;
}

template <class V>
bool GridAttributeContainerT<V>::saveToCgnsFile(int fn, int B, int Z)
{
	int ier;
	// Goto "GridConditions" node.
	ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, "end");
	if (ier != 0) {return false;}
	// Delete the array if it already exists.
	cg_delete_node(const_cast<char*>(name().c_str()));
	// Create the user defined data node.
	ier = cg_user_data_write(const_cast<char*>(name().c_str()));
	if (ier != 0) {return false;}
	// Go to the user defined data node.
	ier = cg_gorel(fn, const_cast<char*>(name().c_str()), 0, "end");
	if (ier != 0) {return false;}

	GridAttributeDimensionsContainer* dims = dimensions();

	// output dimension values
	for (GridAttributeDimensionContainer* dim : dims->containers()) {
		dim->saveToCgnsFile(fn, B, Z);
	}

	if (dims->containers().size() == 0) {
		return saveToCgnsFileForIndex(0);
	} else {
		bool allok = true;
		for (int index = 0; index <= dims->maxIndex(); ++index) {
			bool ok = loadFromExternalFile(temporaryExternalFilename(index));
			if (ok) {
				ok = saveToCgnsFileForIndex(index);
			}
			allok = allok && ok;
		}
		return allok;
	}
}

template <class V>
bool GridAttributeContainerT<V>::saveToCgnsFileForIndex(int index)
{
	int ier;
	// Create the "Value" array
	cgsize_t dimensions[3];
	unsigned int count = dataCount();
	dimensions[0] = count;
	dimensions[1] = dimensions[2] = 0;
	V* data = new V[count];
	for (unsigned int i = 0; i < count; ++i) {
		data[i] = value(i);
	}
	QString aName = arrayNameForIndex(index);
	ier = cg_array_write(iRIC::toStr(aName).c_str() , dataType(), 1, dimensions, data);
	delete[] data;
	if (ier != 0) {return false;}
	return true;
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

template <class V>
QString GridAttributeContainerT<V>::arrayNameForIndex(int index) const
{
	if (index == 0) {return "Value";}
	QString name("Value%1");
	return name.arg(index);
}

#endif // GRIDATTRIBUTECONTAINERT_DETAIL_H
