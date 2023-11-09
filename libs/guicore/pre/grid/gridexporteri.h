#ifndef GRIDEXPORTERI_H
#define GRIDEXPORTERI_H

#include "../../solverdef/solverdefinitiongridtype.h"
#include <QtPlugin>

class CoordinateSystem;
class v4InputGrid;

class QString;
class QStringList;
class QWidget;

/// Base class for Grid Exporter
class GridExporterI
{

public:
	virtual ~GridExporterI() {}
	virtual QString caption() const = 0;
	/// Can be used for exporting the specified grid type.
	virtual bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const = 0;
	/// Return filter string for QFileDialog
	virtual QStringList fileDialogFilters() const = 0;
	/// Export grid data into external file.
	virtual bool doExport(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, CoordinateSystem* cs, QWidget* parent) = 0;
};

#define GridExporterInterface_iid "org.iric.GUI.Plugin.GridExporterInterface"

Q_DECLARE_INTERFACE(GridExporterI, GridExporterInterface_iid)

#endif // GRIDEXPORTERI_H
