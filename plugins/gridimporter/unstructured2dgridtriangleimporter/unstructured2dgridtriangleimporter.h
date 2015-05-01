#ifndef UNSTRUCTURED2DGRIDTRIANGLEIMPORTER_H
#define UNSTRUCTURED2DGRIDTRIANGLEIMPORTER_H

#include <guicore/pre/grid/gridimporterinterface.h>
#include <QObject>

class Unstructured2dGridTriangleImporter : public QObject, public GridImporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterInterface)
public:
	Unstructured2dGridTriangleImporter();
	~Unstructured2dGridTriangleImporter(){}
	QString caption() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override
	{
		return gt == SolverDefinitionGridType::gtUnstructured2DGrid;
	}
	QStringList fileDialogFilters() const override;
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // UNSTRUCTURED2DGRIDTRIANGLEIMPORTER_H
