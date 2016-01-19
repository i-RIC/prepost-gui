#ifndef GRIDATTRIBUTEDIMENSIONCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONCONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributebaseobject.h"

#include <QObject>

#include <vector>

class QVariant;

class SolverDefinitionGridAttributeDimension;

class GUICOREDLL_EXPORT GridAttributeDimensionContainer : public GridAttributeBaseObject
{
	Q_OBJECT
public:
	GridAttributeDimensionContainer(SolverDefinitionGridAttributeDimension* def);
	virtual ~GridAttributeDimensionContainer();

	// basic properties
	const std::string& name() const;
	const QString& caption() const;
	SolverDefinitionGridAttributeDimension* definition() const;

	// indices
	virtual int count() const = 0;
	int currentIndex() const;

	// value handling
	virtual QVariant variantValue(int index) const = 0;
	virtual std::vector<QVariant> variantValues() const = 0;
	virtual void setVariantValues(const std::vector<QVariant>& vals) = 0;
	QVariant currentVariantValue() const;

	// I/O functions
	virtual bool loadFromExternalFile(const QString& filename) = 0;
	virtual bool saveToExternalFile(const QString& filename) = 0;
	virtual bool loadFromCgnsFile(int fn, int B, int Z) = 0;
	virtual bool saveToCgnsFile(int fn, int B, int Z) = 0;

public slots:
	void setCurrentIndex(int index, bool noDraw = false);

signals:
	void valuesChanged();
	void valuesChanged(const std::vector<QVariant>& before, const std::vector<QVariant>& after);

	void currentIndexChanged(bool noDraw);
	void currentIndexChanged(int newIndex, bool noDraw);

private:
	SolverDefinitionGridAttributeDimension* m_definition;
	int m_currentIndex;
};

#endif // GRIDATTRIBUTEDIMENSIONCONTAINER_H
