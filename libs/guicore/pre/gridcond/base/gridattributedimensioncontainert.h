#ifndef GRIDATTRIBUTEDIMENSIONCONTAINERT_H
#define GRIDATTRIBUTEDIMENSIONCONTAINERT_H

#include "../../../guicore_global.h"
#include "../../../project/projectcgnsfile.h"
#include "gridattributecontainer.h"
#include "gridattributedimensioncontainer.h"
#include "../../../solverdef/solverdefinitiongridattribute.h"
#include <misc/stringtool.h>

#include <QList>
#include <QFile>
#include <QDataStream>
#include <QVariant>

#include <cgnslib.h>

template <class V>
class GridAttributeDimensionContainerT : public GridAttributeDimensionContainer
{
public:
	GridAttributeDimensionContainerT(SolverDefinitionGridAttributeDimension* def);
	virtual ~GridAttributeDimensionContainerT();

	const QList<V>& values() const;
	QList<V>& values();
	void setValues(const QList<V>& vals);
	V value(int index) const;

	static V fromVariant(const QVariant& val);
	QVariant toVariant(V val) const;

	QVariant variantValue(int index) const override;
	QList<QVariant> variantValues() const override;
	void setVariantValues(const QList<QVariant>& vals) override;

	int count() const override;

	bool loadFromExternalFile(const QString& filename) override;
	bool saveToExternalFile(const QString& filename) override;
	bool loadFromCgnsFile(int fn, int B, int Z) override;
	bool saveFromCgnsFile(int fn, int B, int Z) override;

private:
	virtual DataType_t dataType() const = 0;

private:
	QList<V> m_values;
};

#include "private/gridattributedimensioncontainert_detail.h"

#endif // GRIDATTRIBUTEDIMENSIONCONTAINERT_H
