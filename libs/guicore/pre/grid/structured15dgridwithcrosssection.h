#ifndef STRUCTURED15DGRIDWITHCROSSSECTION_H
#define STRUCTURED15DGRIDWITHCROSSSECTION_H

#include "../../guicore_global.h"
#include "grid2d.h"
#include <vtkUnstructuredGrid.h>

class Structured15DGridWithCrossSectionCrossSection;

class GUICOREDLL_EXPORT Structured15DGridWithCrossSection : public Grid2D
{
public:
	Structured15DGridWithCrossSection(ProjectDataItem* parent);
	Structured15DGridWithCrossSection(const std::string& zonename, ProjectDataItem* parent);
	~Structured15DGridWithCrossSection();

	vtkUnstructuredGrid* vtkGrid() const;
	vtkUnstructuredGrid* vertexGrid() const;
	unsigned int vertexCount() const override;
	unsigned int cellCount() const override;

	QPointF vertex(unsigned int index) const override;
	void setVertex(unsigned int index, const QPointF& v) override;

	int loadFromCgnsFile(const iRICLib::H5CgnsZone& zone) override;
	int saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName) override;

	void setPoints(vtkPoints* points);
	QList<Structured15DGridWithCrossSectionCrossSection*>& crossSections();
	const QList<Structured15DGridWithCrossSectionCrossSection*>& crossSections() const;
	Structured15DGridWithCrossSectionCrossSection* crossSections(int i);
	void setModified(bool modified = true) override;

private:
	void init();

	vtkUnstructuredGrid* m_vertexGrid;
	QList<Structured15DGridWithCrossSectionCrossSection*> m_crossSections;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTION_H
