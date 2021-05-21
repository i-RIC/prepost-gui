#ifndef GRIDATTRIBUTEDIMENSIONCONTAINERT_H
#define GRIDATTRIBUTEDIMENSIONCONTAINERT_H

#include "../../../guicore_global.h"
#include "../../../project/projectcgnsfile.h"
#include "gridattributecontainer.h"
#include "gridattributedimensioncontainer.h"
#include "../../../solverdef/solverdefinitiongridattribute.h"
#include <misc/stringtool.h>

#include <cgnslib.h>

template <class V>
class GridAttributeDimensionContainerT : public GridAttributeDimensionContainer
{
public:
	GridAttributeDimensionContainerT(SolverDefinitionGridAttributeDimension* def);
	~GridAttributeDimensionContainerT() override;

	const std::vector<V>& values() const;
	std::vector<V>& values();
	void setValues(const std::vector<V>& vals);
	V value(int index) const;

	static V fromVariant(const QVariant& val);
	QVariant toVariant(V val) const;

	QVariant variantValue(int index) const override;
	std::vector<QVariant> variantValues() const override;
	void setVariantValues(const std::vector<QVariant>& vals) override;

	int count() const override;

	bool loadFromExternalFile(const QString& filename) override;
	bool saveToExternalFile(const QString& filename) override;
	int loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts) override;
	int saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts) override;

private:
	virtual DataType_t dataType() const = 0;

private:
	std::vector<V> m_values;
};

#include "private/gridattributedimensioncontainert_detail.h"

#endif // GRIDATTRIBUTEDIMENSIONCONTAINERT_H
