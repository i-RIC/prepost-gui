#ifndef GRID_H
#define GRID_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridtype.h"
#include <vtkPointSet.h>
#include <vtkSmartPointer.h>
#include <vtkAlgorithm.h>
#include <QList>
#include <QMap>
#include <cgnslib.h>

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

/// Abstract class to store Grid
class GUICOREDLL_EXPORT Grid : public ProjectDataItem
{
	Q_OBJECT

public:
	static const int MAX_DRAWCELLCOUNT;
	static const int MAX_DRAWINDEXCOUNT;
	static const char LABEL_NAME[];
	/// Constructor
	Grid(const QString& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	Grid(SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	/// Destructor
	virtual ~Grid();
	const QString& zoneName() {return m_zoneName;}
	void setZoneName(const QString& name) {m_zoneName = name;}
	void loadFromCgnsFile(const int fn) override;
	virtual bool loadFromCgnsFile(const int fn, int base, int zoneid) = 0;
	virtual void saveToCgnsFile(const int fn) override;
	virtual bool saveToCgnsFile(const int fn, int base, char* zonename) = 0;
	/// The pointer to VTK container that holds the data in deed.
	vtkPointSet* vtkGrid() const {return m_vtkGrid;}

	vtkAlgorithm* vtkFilteredShapeAlgorithm() const {return m_vtkFilteredShapeAlgorithm;}
	vtkAlgorithm* vtkFilteredPointsAlgorithm() const {return m_vtkFilteredPointsAlgorithm;}
	vtkAlgorithm* vtkFilteredCellsAlgorithm() const {return m_vtkFilteredCellsAlgorithm;}
	virtual vtkAlgorithm* vtkFilteredIndexGridAlgorithm() const {return 0;}

	/// The list of grid related conditions
	QList<GridAttributeContainer*>& gridRelatedConditions() {
		return m_gridRelatedConditions;
	}
	/// Get grid related condition by name.
	GridAttributeContainer* gridRelatedCondition(const QString& name) {
		return m_gridRelatedConditionNameMap.value(name);
	}
	void addGridRelatedCondition(GridAttributeContainer* cond);
	/// Returns the number of nodes
	unsigned int nodeCount() const {
		return m_vtkGrid->GetNumberOfPoints();
	}
	/// Returns the number of cells
	virtual unsigned int cellCount() const = 0;
	virtual void setModified() override {
		m_isModified = true;
		m_vtkGrid->Modified();
	}
	bool isModified() const {return m_isModified;}
	virtual const QStringList checkShape(QTextStream& stream);
	virtual bool isValid(QTextStream& /*stream*/) const {return true;}
	bool isCustomModified();
	void setCustomModified(bool modified);
	virtual void updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax);
	bool isMasked() const {return m_isMasked;}
	SolverDefinitionGridType::GridType gridType() const {return m_gridType;}
	void setParent(QObject* parent);
	static void getCullSetting(bool* enable, int* cellLimit, int* indexLimit);

protected:
	virtual void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override {}
	bool loadGridRelatedConditions(int fn, int B, int Z);
	bool saveGridRelatedConditions(int fn, int B, int Z);
	static int zoneId(const QString& zonename, int fn, int B, cgsize_t sizes[9]);
	vtkPointSet* m_vtkGrid;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredShapeAlgorithm;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredPointsAlgorithm;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredCellsAlgorithm;
	QList<GridAttributeContainer*> m_gridRelatedConditions;
	QMap<QString, GridAttributeContainer*> m_gridRelatedConditionNameMap;
	QString m_zoneName;
	SolverDefinitionGridType::GridType m_gridType;
	bool m_isModified;
	bool m_isMasked;

private:
	void initPointers();

public:
	friend class GridInternalImporter;
	friend class GridInternalExporter;
};

#endif // GRID_H
