#ifndef UNSTRUCTURED2DGRIDVTKEXPORTER_H
#define UNSTRUCTURED2DGRIDVTKEXPORTER_H

#include <guicore/pre/grid/gridexporterinterface.h>

class Unstructured2DGridVTKExporter : public QObject, public GridExporterInterface
{
	Q_OBJECT
public:
	/// Constructor
	Unstructured2DGridVTKExporter();
	~Unstructured2DGridVTKExporter(){}
	QString caption() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override
	{
		return gt = SolverDefinitionGridType::gtUnstructured2DGrid;
	}
	QStringList fileDialogFilters() const override;
	bool doExport(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // UNSTRUCTURED2DGRIDVTKEXPORTER_H
