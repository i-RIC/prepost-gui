#ifndef GRIDATTRIBUTECONTAINER_H
#define GRIDATTRIBUTECONTAINER_H

#include "../../../guicore_global.h"

#include "gridattributebaseobject.h"
#include "../../grid/v4inputgrid.h"

#include <QObject>

#include <vector>

class PreProcessorGeoDataGroupDataItemI;
class GridAttributeDimensionsContainer;

class vtkDataSetAttributes;

namespace iRICLib {
	class H5CgnsGridAttributes;
} // namespace iRICLib

class GUICOREDLL_EXPORT GridAttributeContainer : public GridAttributeBaseObject
{
	Q_OBJECT

public:
	GridAttributeContainer(v4InputGrid* grid, SolverDefinitionGridAttribute* cond);
	~GridAttributeContainer();

	// basic properties
	const std::string& name() const;
	v4InputGrid* grid() const;

	GridAttributeDimensionsContainer* dimensions() const;
	void setDimensions(GridAttributeDimensionsContainer* dims);

	QString temporaryDir() const;
	void setTemporaryDir(const QString& dir);
	void clearTemporaryData();

	unsigned int dataCount() const;
	virtual void setDefaultValue() = 0;
	virtual bool getValueRange(double* min, double* max) = 0;

	bool mapped() const;
	virtual void setMapped(bool mapped);

	bool isCustomModified() const;
	void setCustomModified(bool c);

	void updateValueRange();

	virtual void allocate() = 0;

	virtual int loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts) = 0;
	virtual int saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts) = 0;

public slots:
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex);
	void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after);

protected:
	vtkDataSetAttributes* vtkAttributes() const;
	QString temporaryExternalFilename(int index) const;

private:
	virtual bool loadFromExternalFile(const QString& filename) = 0;
	virtual bool saveToExternalFile(const QString& filename) = 0;
	virtual void setModified() = 0;

	v4InputGrid* m_grid;
	GridAttributeDimensionsContainer* m_dimensions;
	QString m_temporaryDir;
	bool m_mapped;
	bool m_isCustomModified;
};

#endif // GRIDATTRIBUTECONTAINER_H
