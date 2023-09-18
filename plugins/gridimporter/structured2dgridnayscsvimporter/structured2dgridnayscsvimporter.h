#ifndef STRUCTURED2DGRIDNAYSCSVIMPORTER_H
#define STRUCTURED2DGRIDNAYSCSVIMPORTER_H

#include <guicore/pre/grid/gridimporteri.h>
#include <QObject>

class Structured2DGridNaysCSVImporter : public QObject, public GridImporterI
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterI)

public:
	Structured2DGridNaysCSVImporter();
	~Structured2DGridNaysCSVImporter(){}

	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	bool import(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // STRUCTURED2DGRIDNAYSCSVIMPORTER_H
