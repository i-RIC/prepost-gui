#ifndef GRID_H
#define GRID_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridtype.h"

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
class vtkAlgorithm;
class vtkPointSet;

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
	bool loadGridAttributes(int fn, int B, int Z);
	bool saveGridAttributes(int fn, int B, int Z);
	static int zoneId(const std::string& zonename, int fn, int B, cgsize_t sizes[9]);

	void setMasked(bool masked);
	void setFilteredShapeAlgorithm(vtkAlgorithm* algo);
	void setFilteredPointsAlgorithm(vtkAlgorithm* algo);
	void setFilteredCellsAlgorithm(vtkAlgorithm* algo);

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
