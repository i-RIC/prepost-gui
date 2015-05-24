#ifndef GRIDRELATEDCONDITIONCONTAINER_H
#define GRIDRELATEDCONDITIONCONTAINER_H

#include "../../../guicore_global.h"
#include "gridrelatedconditionbaseobject.h"
#include "../../grid/grid.h"
#include <QObject>

class PreProcessorRawDataGroupDataItemInterface;
class GridRelatedConditionDimensionsContainer;

class GUICOREDLL_EXPORT GridRelatedConditionContainer : public GridRelatedConditionBaseObject
{
	Q_OBJECT

public:
	enum DefinitionPosition {
		CellCenter,
		Node,
	};
	/// Constructor
	GridRelatedConditionContainer(Grid* grid, SolverDefinitionGridRelatedCondition* cond);
	Grid* grid() const {return m_grid;}
	const QString& name() const;
	bool mapped() const {return m_mapped;}
	virtual void setMapped(bool mapped) {
		m_mapped = mapped;
		setModified();
	}
	virtual bool loadFromCgnsFile(int fn, int B, int Z) = 0;
	virtual bool saveToCgnsFile(int fn, int B, int Z) = 0;

	virtual void allocate() = 0;
	virtual unsigned int dataCount() const = 0;
	virtual bool getValueRange(double* min, double* max) = 0;
	bool isCustomModified() const {return m_isCustomModified;}
	void setCustomModified(bool c) {m_isCustomModified = c;}
	void updateConnections();
	GridRelatedConditionDimensionsContainer* dimensions() const;

public slots:
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex);
	void handleDimensionValuesChange(const QList<QVariant>& before, const QList<QVariant>& after);

protected:
	const QString temporaryExternalFilename(int index) const;
	virtual bool loadFromExternalFile(const QString& filename) = 0;
	virtual bool saveToExternalFile(const QString& filename) = 0;

	virtual void setModified() = 0;
	bool m_mapped;
	bool m_isCustomModified;
	Grid* m_grid;
};

#endif // GRIDRELATEDCONDITIONCONTAINER_H
