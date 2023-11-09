#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONHECRASEXPORTER_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONHECRASEXPORTER_H

#include <guicore/pre/grid/gridexporteri.h>
#include <QObject>

class Structured15DGridWithCrossSectionHecRasExporter : public QObject, public GridExporterI
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridExporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridExporterI)

public:
	Structured15DGridWithCrossSectionHecRasExporter();
	~Structured15DGridWithCrossSectionHecRasExporter();

	QString caption() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override;
	QStringList fileDialogFilters() const override;
	bool doExport(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, CoordinateSystem* cs, QWidget* parent) override;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONHECRASEXPORTER_H
