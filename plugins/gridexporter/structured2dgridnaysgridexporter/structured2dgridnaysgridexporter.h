#ifndef STRUCTURED2DGRIDNAYSGRIDEXPORTER_H
#define STRUCTURED2DGRIDNAYSGRIDEXPORTER_H

#include <guicore/pre/grid/gridexporterinterface.h>
#include <QObject>

class Structured2DGridNaysGridExporter : public QObject, public GridExporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridExporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridExporterInterface)
public:
	/// Constructor
	Structured2DGridNaysGridExporter();
	~Structured2DGridNaysGridExporter(){}
	QString caption() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override
	{
		return gt == SolverDefinitionGridType::gtStructured2DGrid;
	}
	QStringList fileDialogFilters() const override;
	bool doExport(Grid* grid, const QString& filename, const QString& selectedFilter, CoordinateSystem* cs, QWidget* parent) override;
};

#endif // STRUCTURED2DGRIDNAYSGRIDEXPORTER_H
