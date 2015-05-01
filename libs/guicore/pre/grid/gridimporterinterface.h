#ifndef GRIDIMPORTERINTERFACE_H
#define GRIDIMPORTERINTERFACE_H

#include "../../solverdef/solverdefinitiongridtype.h"
#include <QtPlugin>

class Grid;
class QString;
class QStringList;
class QWidget;

class GridImporterInterface
{
public:
	virtual ~GridImporterInterface() {}
	virtual QString caption() const = 0;
	/// Can be used for importing the specified grid type.
	virtual bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const = 0;
	/// Return filter string for QFileDialog
	virtual QStringList fileDialogFilters() const = 0;
	/// Import grid data from external file.
	virtual bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) = 0;
};

#define GridImporterInterface_iid "org.iric.GUI.Plugin.GridImporterInterface"

Q_DECLARE_INTERFACE(GridImporterInterface, GridImporterInterface_iid)

#endif // GRIDIMPORTERINTERFACE_H
