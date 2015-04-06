#ifndef STRUCTURED2DGRIDNAYSCSVEXPORTER_H
#define STRUCTURED2DGRIDNAYSCSVEXPORTER_H

#include <guicore/pre/grid/gridexporterinterface.h>
#include <QObject>

class Structured2DGridNaysCSVExporter : public QObject, public GridExporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridExporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridExporterInterface)
public:
	/// Constructor
	Structured2DGridNaysCSVExporter();
	~Structured2DGridNaysCSVExporter(){}
	QString caption() const;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt)
	{
		return gt = SolverDefinitionGridType::gtStructured2DGrid;
	}
	const QStringList fileDialogFilters();
	bool doExport(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent);
};

#endif // STRUCTURED2DGRIDNAYSCSVEXPORTER_H
