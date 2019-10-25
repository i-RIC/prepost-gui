#ifndef GRIDATTRIBUTECONTAINER_H
#define GRIDATTRIBUTECONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributebaseobject.h"
#include "../../grid/grid.h"
#include <QObject>

#include <vector>

class PreProcessorGeoDataGroupDataItemInterface;
class GridAttributeDimensionsContainer;

class GUICOREDLL_EXPORT GridAttributeContainer : public GridAttributeBaseObject
{
	Q_OBJECT

public:
	GridAttributeContainer(Grid* grid, SolverDefinitionGridAttribute* cond);
	virtual ~GridAttributeContainer();

	// basic properties
	const std::string& name() const;
	Grid* grid() const;
	GridAttributeDimensionsContainer* dimensions() const;
	virtual unsigned int dataCount() const = 0;
	virtual bool getValueRange(double* min, double* max) = 0;

	bool mapped() const;
	virtual void setMapped(bool mapped);

	bool isCustomModified() const;
	void setCustomModified(bool c);

	virtual void allocate() = 0;

	virtual bool loadFromCgnsFile(int fn, int B, int Z) = 0;
	virtual bool saveToCgnsFile(int fn, int B, int Z) = 0;

	void updateConnections();

public slots:
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex);
	void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after);

protected:
	QString temporaryExternalFilename(int index) const;

private:
	virtual bool loadFromExternalFile(const QString& filename) = 0;
	virtual bool saveToExternalFile(const QString& filename) = 0;
	virtual void setModified() = 0;

	bool m_mapped;
	bool m_isCustomModified;
	Grid* m_grid;
};

#endif // GRIDATTRIBUTECONTAINER_H
