#ifndef GRIDRELATEDCONDITIONDIMENSIONCONTAINERT_H
#define GRIDRELATEDCONDITIONDIMENSIONCONTAINERT_H

#include "../../../guicore_global.h"
#include "../../../project/projectcgnsfile.h"
#include "gridrelatedconditioncontainer.h"
#include "gridrelatedconditiondimensioncontainer.h"
#include "../../../solverdef/solverdefinitiongridrelatedcondition.h"
#include <misc/stringtool.h>

#include <QList>
#include <QFile>
#include <QDataStream>
#include <QVariant>

#include <cgnslib.h>

template <class V>
class GridRelatedConditionDimensionContainerT : public GridRelatedConditionDimensionContainer
{
public:
	GridRelatedConditionDimensionContainerT(SolverDefinitionGridRelatedConditionDimension* def)
		: GridRelatedConditionDimensionContainer(def)
	{}

	const QList<V>& values() const {return m_values;}
	QList<V>& values() {return m_values;}
	void setValues(const QList<V>& vals){
		QList<QVariant> oldVals = variantValues();
		m_values = vals;
		QList<QVariant> newVals = variantValues();
		emit valuesChanged();
		emit valuesChanged(oldVals, newVals);
	}
	V value(int index) const {
		if (index < 0){return 0;}
		if (index >= m_values.size()){return 0;}
		return m_values.at(index);
	}
	static V fromVariant(const QVariant& val)
	{
		return val.value<V>();
	}
	QVariant toVariant(V val)
	{
		return QVariant(val);
	}

	QVariant variantValue(int index) const {return QVariant(value(index));}
	QList<QVariant> variantValues() const
	{
		QList<QVariant> ret;
		for (int i = 0; i < count(); ++i){
			QVariant v = variantValue(i);
			ret.append(v);
		}
		return ret;
	}
	void setVariantValues(const QList<QVariant>& vals)
	{
		QList<QVariant> currentVals = variantValues();
		if (currentVals == vals){return;}
		QList<V> nativeVals;
		for (int i = 0; i < vals.size(); ++i){
			nativeVals.append(fromVariant(vals.at(i)));
		}
		setValues(nativeVals);
	}

	int count() const {return m_values.count();}
	bool loadFromExternalFile(const QString& filename)
	{
		QFile f(filename);
		bool ok = f.open(QIODevice::ReadOnly);
		if (!ok){ return false; }
		QDataStream s(&f);
		int count;
		V value;
		s >> count;
		m_values.clear();
		for (int i = 0; i < count; ++i){
			s >> value;
			m_values.append(value);
		}
		f.close();
		return true;
	}

	bool saveToExternalFile(const QString& filename)
	{
		QFile f(filename);
		bool ok = f.open(QIODevice::WriteOnly);
		if (!ok){ return false;}
		QDataStream s(&f);
		int count = m_values.size();
		s << count;
		for (int i = 0; i < count; ++i){
			s << m_values.at(i);
		}
		f.close();
		return true;
	}

	bool loadFromCgnsFile(int fn, int B, int Z)
	{
		// Goto "GridConditions" node.
		int ier;
		bool found = false;
		ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, iRIC::toStr(condition()->name()).c_str(), 0, "end");
		if (ier == 0){
			// the corresponding node found.
			// Find "Dimension_(name)" array.
			QString targetName = QString("Dimension_%1").arg(name());
			int narrays;
			cg_narrays(&narrays);
			for (int i = 1; i <= narrays; ++i){
				char arrayName[ProjectCgnsFile::BUFFERLEN];
				DataType_t dt;
				int dataDimension;
				cgsize_t dimensionVector[3];
				cgsize_t dataCount;
				cg_array_info(i, arrayName, &dt, &dataDimension, dimensionVector);
				if (dataDimension != 1){return false;}
				if (dt == dataType() && QString(arrayName) == targetName){
					// We've found the array!
					dataCount = dimensionVector[0];
					// load data.
					V* data = new V[dataCount];
					ier = cg_array_read(i, data);
					m_values.clear();
					for (unsigned int j = 0; j < dataCount; ++j){
						m_values.append(*(data + j));
					}
					delete[] data;
					found = true;
				}
			}
		}
		return found;
	}

	bool saveFromCgnsFile(int fn, int B, int Z)
	{
		QString arrayName = QString("Dimension_%1").arg(name());

		int ier;
		// Goto "GridConditions" node.
		ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, iRIC::toStr(m_condition->name()).c_str(), 0, "end");
		if (ier != 0){return false;}
		// Delete the array if it already exists.
		cg_delete_node(const_cast<char*>(iRIC::toStr(arrayName).c_str()));
		// Create the "array" array
		int dataCount = count();
		cgsize_t dimensions = m_values.count();
		V* data = new V[dataCount];
		for (int i = 0; i < dataCount; ++i){
			data[i] = m_values.at(i);
		}
		ier = cg_array_write(iRIC::toStr(arrayName).c_str(), dataType(), 1, &dimensions, data);
		delete[] data;
		if (ier != 0){return false;}
		return true;
	}

protected:
	virtual DataType_t dataType() const = 0;

private:
	QList<V> m_values;

};

class GridRelatedConditionDimensionIntegerContainer : public GridRelatedConditionDimensionContainerT<int>
{
public:
	GridRelatedConditionDimensionIntegerContainer(SolverDefinitionGridRelatedConditionDimension* def)
		: GridRelatedConditionDimensionContainerT<int>(def)
	{}
protected:
	DataType_t dataType() const {return Integer;}
};

class GridRelatedConditionDimensionRealContainer : public GridRelatedConditionDimensionContainerT<double>
{
public:
	GridRelatedConditionDimensionRealContainer(SolverDefinitionGridRelatedConditionDimension* def)
		: GridRelatedConditionDimensionContainerT<double>(def)
	{}
protected:
	DataType_t dataType() const {return RealDouble;}
};

#endif // GRIDRELATEDCONDITIONDIMENSIONCONTAINERT_H
