#ifndef GRIDLANDXMLEXPORTER_H
#define GRIDLANDXMLEXPORTER_H

#include <guicore/pre/grid/gridexporterinterface.h>
#include <QObject>

class GridLandXmlExporter : public QObject, public GridExporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridExporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridExporterInterface)
public:
	GridLandXmlExporter();
	~GridLandXmlExporter();

	QString caption() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override;
	QStringList fileDialogFilters() const override;
	bool doExport(Grid* grid, const QString& filename, const QString& selectedFilter, CoordinateSystem* cs, QWidget* parent) override;
};

#endif // GRIDLANDXMLEXPORTER_H
