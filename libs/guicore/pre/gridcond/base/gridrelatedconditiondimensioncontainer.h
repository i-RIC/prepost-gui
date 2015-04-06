#ifndef GRIDRELATEDCONDITIONDIMENSIONCONTAINER_H
#define GRIDRELATEDCONDITIONDIMENSIONCONTAINER_H

#include "../../../guicore_global.h"
#include "gridrelatedconditionbaseobject.h"

#include <QObject>
#include <QVariant>
#include <QList>

class SolverDefinitionGridRelatedConditionDimension;

class GUICOREDLL_EXPORT GridRelatedConditionDimensionContainer : public GridRelatedConditionBaseObject
{
	Q_OBJECT

public:
	GridRelatedConditionDimensionContainer(SolverDefinitionGridRelatedConditionDimension* def);
	~GridRelatedConditionDimensionContainer();
	SolverDefinitionGridRelatedConditionDimension* definition() const {return m_definition;}
	const QString& name() const;
	const QString& caption() const;
	int currentIndex() const {return m_currentIndex;}
	virtual int count() const = 0;
	virtual QVariant variantValue(int index) const = 0;
	virtual QList<QVariant> variantValues() const = 0;
	virtual void setVariantValues(const QList<QVariant>& vals) = 0;
	QVariant currentVariantValue() const {return variantValue(currentIndex());}
	virtual bool loadFromExternalFile(const QString& filename) = 0;
	virtual bool saveToExternalFile(const QString& filename) = 0;
	virtual bool loadFromCgnsFile(int fn, int B, int Z) = 0;
	virtual bool saveFromCgnsFile(int fn, int B, int Z) = 0;

public slots:
	void setCurrentIndex(int index, bool noDraw = false);

signals:
	void valuesChanged();
	void valuesChanged(const QList<QVariant> before, const QList<QVariant> after);
	void currentIndexChanged(bool noDraw);
	void currentIndexChanged(int newIndex, bool noDraw);

protected:
	SolverDefinitionGridRelatedConditionDimension* m_definition;

private:
	int m_currentIndex;

};

#endif // GRIDRELATEDCONDITIONDIMENSIONCONTAINER_H
