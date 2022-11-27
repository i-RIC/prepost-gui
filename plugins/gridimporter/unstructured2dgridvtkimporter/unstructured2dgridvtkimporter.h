#ifndef UNSTRUCTURED2DGRIDVTKIMPORTER_H
#define UNSTRUCTURED2DGRIDVTKIMPORTER_H

#include <guicore/pre/grid/gridimporterinterface.h>
#include <QObject>

class Unstructured2dGridVtkImporter : public QObject, public GridImporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterInterface)
public:
	Unstructured2dGridVtkImporter();
	~Unstructured2dGridVtkImporter(){}

	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // UNSTRUCTURED2DGRIDVTKIMPORTER_H
