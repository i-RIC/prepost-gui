#ifndef STRUCTURED2DGRIDNAYSCSVIMPORTER_H
#define STRUCTURED2DGRIDNAYSCSVIMPORTER_H

#include <guicore/pre/grid/gridimporterinterface.h>
#include <QObject>

class Structured2DGridNaysCSVImporter : public QObject, public GridImporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterInterface)

public:
	/// Constructor
	Structured2DGridNaysCSVImporter();
	~Structured2DGridNaysCSVImporter(){}
	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	/// Import grid data from external file.
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // STRUCTURED2DGRIDNAYSCSVIMPORTER_H
