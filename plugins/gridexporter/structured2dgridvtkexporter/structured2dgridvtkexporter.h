#ifndef STRUCTURED2DGRIDVTKEXPORTER_H
#define STRUCTURED2DGRIDVTKEXPORTER_H

#include <guicore/pre/grid/gridexporteri.h>
#include <QObject>

class Structured2DGridVTKExporter : public QObject, public GridExporterI
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridExporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridExporterI)

public:
	Structured2DGridVTKExporter();
	~Structured2DGridVTKExporter();

	QString caption() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override;
	QStringList fileDialogFilters() const override;
	bool doExport(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, CoordinateSystem* cs, QWidget* parent) override;
};

#endif // STRUCTURED2DGRIDVTKEXPORTER_H
