#ifndef GRID_H
#define GRID_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridtype.h"

#include <vtkSmartPointer.h>
#include <vtkAlgorithm.h>
#include <QList>
#include <QMap>
#include <cgnslib.h>

#include <string>

#if CGNS_VERSION < 3100
#define cgsize_t int
#endif

class SolverDefinition;
class QLocale;
class QTextStream;
class QStringList;
class GridInternalImporter;
class GridInternalExporter;
class GridAttributeContainer;
class vtkPointSet;

/// Abstract class to store Grid
class GUICOREDLL_EXPORT Grid : public ProjectDataItem
{
	Q_OBJECT

public:
	static const int MAX_DRAWCELLCOUNT;
	static const int MAX_DRAWINDEXCOUNT;
	static const char LABEL_NAME[];

	Grid(const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	Grid(SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	virtual ~Grid();

	SolverDefinitionGridType::GridType gridType() const;
	const std::string& zoneName() const;
	void setZoneName(const std::string& name);

	void setParent(QObject* parent);

	void loadFromCgnsFile(const int fn) override;
	virtual bool loadFromCgnsFile(const int fn, int base, int zoneid) = 0;
	void saveToCgnsFile(const int fn) override;
	virtual bool saveToCgnsFile(const int fn, int base, const char* zonename) = 0;

	vtkPointSet* vtkGrid() const;

	vtkAlgorithm* vtkFilteredShapeAlgorithm() const;
	vtkAlgorithm* vtkFilteredPointsAlgorithm() const;
	vtkAlgorithm* vtkFilteredCellsAlgorithm() const;
	virtual vtkAlgorithm* vtkFilteredIndexGridAlgorithm() const;

	QList<GridAttributeContainer*>& gridRelatedConditions();
	GridAttributeContainer* gridRelatedCondition(const std::string& name) const;
	void addGridRelatedCondition(GridAttributeContainer* cond);

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
	virtual void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override {}
	bool loadGridRelatedConditions(int fn, int B, int Z);
	bool saveGridRelatedConditions(int fn, int B, int Z);
	static int zoneId(const std::string& zonename, int fn, int B, cgsize_t sizes[9]);
	vtkPointSet* m_vtkGrid;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredShapeAlgorithm;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredPointsAlgorithm;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredCellsAlgorithm;
	QList<GridAttributeContainer*> m_gridRelatedConditions;
	QMap<std::string, GridAttributeContainer*> m_gridRelatedConditionNameMap;
	std::string m_zoneName;
	SolverDefinitionGridType::GridType m_gridType;
	bool m_isModified;
	bool m_isMasked;

public:
	friend class GridInternalImporter;
	friend class GridInternalExporter;
};

#endif // GRID_H
