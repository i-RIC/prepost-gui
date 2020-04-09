#ifndef STRUCTURED2DGRIDGDALIMPORTER_H
#define STRUCTURED2DGRIDGDALIMPORTER_H

#include <guicore/pre/grid/gridimporterinterface.h>
#include <QObject>

class Structured2DGridGdalImporter : public QObject, public GridImporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterInterface)

public:
	Structured2DGridGdalImporter();
	~Structured2DGridGdalImporter(){}

	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // STRUCTURED2DGRIDGDALIMPORTER_H
