#ifndef UNSTRUCTURED2DGRIDSEWERIMPORTER_H
#define UNSTRUCTURED2DGRIDSEWERIMPORTER_H

#include <guicore/pre/grid/gridimporteri.h>
#include <QObject>

class Unstructured2dGridSewerImporter : public QObject, public GridImporterI
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterI)
public:
	Unstructured2dGridSewerImporter();
	~Unstructured2dGridSewerImporter(){}

	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	bool import(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // UNSTRUCTURED2DGRIDSEWERIMPORTER_H
