#ifndef GRID_H
#define GRID_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridtype.h"

#include <QList>
#include <QMap>

#include <string>

class GridAttributeContainer;
class PreProcessorGridDataItemInterface;

class QTextStream;
class QStringList;

class vtkAbstractPointLocator;
class vtkAlgorithm;
class vtkPolyDataAlgorithm;
class vtkPoints;
class vtkPointSet;

namespace iRICLib {
	class H5CgnsBase;
	class H5CgnsGridAttributes;
	class H5CgnsZone;
} // namespace iRICLib

/// Abstract class to store Grid
class GUICOREDLL_EXPORT Grid : public ProjectDataItem
{
	Q_OBJECT

public:
	static const int MAX_DRAWCELLCOUNT;
	static const int MAX_DRAWINDEXCOUNT;
	static const char LABEL_NAME[];

	Grid(vtkPointSet* ps, const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	Grid(vtkPointSet* ps, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	virtual ~Grid();

	SolverDefinitionGridType::GridType gridType() const;
	const std::string& zoneName() const;
	void setZoneName(const std::string& name);

	PreProcessorGridDataItemInterface* dataItem() const;
	void setDataItem(PreProcessorGridDataItemInterface* item);

	void setParent(QObject* parent);

	virtual int loadFromCgnsFile(const iRICLib::H5CgnsZone& zone) = 0;
	virtual int saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName) = 0;

	vtkPointSet* vtkGrid() const;

	vtkAbstractPointLocator* pointLocator() const;
	vtkPointSet* vtkFilteredGrid() const;
	vtkPointSet* vtkFilteredIndexGrid() const;
	void setPoints(vtkPoints* points);

	QList<GridAttributeContainer*>& gridAttributes();
	GridAttributeContainer* gridAttribute(const std::string& name) const;
	void addGridAttribute(GridAttributeContainer* cond);

	bool hasGeneratingAttributes() const;

	unsigned int nodeCount() const;
	virtual unsigned int cellCount() const = 0;

	bool isModified() const;
	void setModified(bool modified = true) override;

	virtual const QStringList checkShape(QTextStream& stream);
	virtual bool isValid(QTextStream& stream) const;

	bool isCustomModified();
	void setCustomModified(bool modified);

	virtual void updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax);
	bool isMasked() const;

	static void getCullSetting(bool* enable, int* cellLimit, int* indexLimit);

protected:
	int loadGridAttributes(const iRICLib::H5CgnsGridAttributes& atts);
	int saveGridAttributes(iRICLib::H5CgnsGridAttributes* atts);

	void setMasked(bool masked);
	void setFilteredGrid(vtkPointSet* data);
	void setFilteredIndexGrid(vtkPointSet* data);
	void setPointLocator(vtkAbstractPointLocator* locator);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	class Impl;
	Impl* impl;

public:
	friend class GridInternalImporter;
	friend class GridInternalExporter;
};

#ifdef _DEBUG
	#include "private/grid_impl.h"
#endif // _DEBUG

#endif // GRID_H
