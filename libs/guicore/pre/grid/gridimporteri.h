#ifndef GRIDIMPORTERI_H
#define GRIDIMPORTERI_H

#include "../../solverdef/solverdefinitiongridtype.h"
#include <QtPlugin>

class v4InputGrid;
class QString;
class QStringList;
class QWidget;

class GridImporterI
{

public:
	virtual ~GridImporterI() {}
	virtual QString caption() const = 0;
	/// Can be used for importing the specified grid type.
	virtual SolverDefinitionGridType::GridType supportedGridType() const = 0;
	/// Return filter string for QFileDialog
	virtual QStringList fileDialogFilters() const = 0;
	/// Import grid data from external file.
	virtual bool import(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) = 0;
};

#define GridImporterInterface_iid "org.iric.GUI.Plugin.GridImporterInterface"

Q_DECLARE_INTERFACE(GridImporterI, GridImporterInterface_iid)

#endif // GRIDIMPORTERI_H
