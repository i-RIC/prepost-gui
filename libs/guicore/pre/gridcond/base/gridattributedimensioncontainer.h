#ifndef GRIDATTRIBUTEDIMENSIONCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONCONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributebaseobject.h"

#include <QObject>

#include <vector>

class QVariant;

class SolverDefinitionGridAttributeDimension;

namespace iRICLib {
	class H5CgnsGridAttributes;
} // namespace iRICLib

class GUICOREDLL_EXPORT GridAttributeDimensionContainer : public GridAttributeBaseObject
{
	Q_OBJECT
public:
	GridAttributeDimensionContainer(SolverDefinitionGridAttributeDimension* def);
	~GridAttributeDimensionContainer() override;

	// basic properties
	std::string name() const;
	QString caption() const;
	SolverDefinitionGridAttributeDimension* definition() const;

	// indices
	virtual int count() const = 0;
	int currentIndex() const;

	// value handling
	virtual QVariant variantValue(int index) const = 0;
	virtual std::vector<QVariant> variantValues() const = 0;
	virtual void setVariantValues(const std::vector<QVariant>& vals) = 0;
	QVariant currentVariantValue() const;
	void clear();

	// I/O functions
	virtual bool loadFromExternalFile(const QString& filename) = 0;
	virtual bool saveToExternalFile(const QString& filename) = 0;
	virtual int loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts) = 0;
	virtual int saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts) = 0;

public slots:
	void setCurrentIndex(int index, bool noDraw = false);

signals:
	void valuesChanged();
	void valuesChanged(const std::vector<QVariant>& before, const std::vector<QVariant>& after);

	void currentIndexChanged(bool noDraw);
	void currentIndexChanged(int newIndex, bool noDraw);

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/gridattributedimensioncontainer_impl.h"
#endif // _DEBUG

#endif // GRIDATTRIBUTEDIMENSIONCONTAINER_H
