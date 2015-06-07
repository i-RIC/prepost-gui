#ifndef STRUCTURED15DGRIDWITHCROSSSECTION_H
#define STRUCTURED15DGRIDWITHCROSSSECTION_H

#include "../../guicore_global.h"
#include "grid2d.h"
#include <vtkUnstructuredGrid.h>

class Structured15DGridWithCrossSectionCrossSection;

class GUICOREDLL_EXPORT Structured15DGridWithCrossSection : public Grid2D
{

public:
	/// Constructor
	Structured15DGridWithCrossSection(ProjectDataItem* parent);
	/// Constructor
	Structured15DGridWithCrossSection(const QString& zonename, ProjectDataItem* parent);
	/// Return VTK container object to store the grid.
	vtkUnstructuredGrid* vtkGrid() const {return dynamic_cast<vtkUnstructuredGrid*>(m_vtkGrid);}
	vtkUnstructuredGrid* vertexGrid() const {return m_vertexGrid;}
	const QVector2D vertex(unsigned int index) const override;
	void setVertex(unsigned int index, const QVector2D& v) override;
	bool loadFromCgnsFile(const int fn, int B, int Z) override;
	bool saveToCgnsFile(const int fn, int B, char* zonename) override;
	unsigned int vertexCount() const override {return m_vertexGrid->GetNumberOfPoints();}
	unsigned int cellCount() const override {return nodeCount() - 1;}
	void setPoints(vtkPoints* points);
	QList<Structured15DGridWithCrossSectionCrossSection*>& crossSections() {return m_crossSections;}
	const QList<Structured15DGridWithCrossSectionCrossSection*>& crossSections() const {return m_crossSections;}
	Structured15DGridWithCrossSectionCrossSection* crossSections(int i) {
		return m_crossSections.at(i);
	}
	void setModified() override {
		Grid::setModified();
		m_vertexGrid->Modified();
	}

private:
	void init();
	vtkUnstructuredGrid* m_vertexGrid;
	QList<Structured15DGridWithCrossSectionCrossSection*> m_crossSections;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTION_H
