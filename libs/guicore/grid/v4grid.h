#ifndef V4GRID_H
#define V4GRID_H

#include "../guicore_global.h"

#include <vtkIdList.h>

#include <QObject>

#include <memory>

class v4GridStructureCheckerI;

class vtkPointSet;
class vtkPointSetExtended;

class GUICOREDLL_EXPORT v4Grid : public QObject
{
	Q_OBJECT

public:
	class AttributeDataProvider;

	v4Grid(vtkPointSetExtended* data);
	virtual ~v4Grid();

	vtkPointSetExtended* vtkData() const;

	vtkIdType nodeCount() const;
	vtkIdType cellCount() const;

	AttributeDataProvider* attributeDataProvider() const;
	void setAttributeDataProvider(AttributeDataProvider* provider);

	void applyOffset(const QPointF& point);
	void pointsModified();

	virtual void updateCellIndex();

	virtual v4GridStructureCheckerI* structureChecker() const;

signals:
	void changed();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // V4GRID_H
