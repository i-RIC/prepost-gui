#ifndef UNSTRUCTURED2DGRIDVTKEXPORTER_H
#define UNSTRUCTURED2DGRIDVTKEXPORTER_H

#include <guicore/pre/grid/gridexporterinterface.h>

class Unstructured2DGridVTKExporter : public QObject, public GridExporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridExporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridExporterInterface)
public:
	Unstructured2DGridVTKExporter();
	~Unstructured2DGridVTKExporter(){}

	QString caption() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override;
	QStringList fileDialogFilters() const override;
	bool doExport(Grid* grid, const QString& filename, const QString& selectedFilter, CoordinateSystem* cs, QWidget* parent) override;
};

#endif // UNSTRUCTURED2DGRIDVTKEXPORTER_H
