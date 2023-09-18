#ifndef STRUCTURED2DGRIDNAYSGRIDIMPORTER_H
#define STRUCTURED2DGRIDNAYSGRIDIMPORTER_H

#include <guicore/pre/grid/gridimporteri.h>
#include <QObject>

class Structured2DGridNaysGridImporter : public QObject, public GridImporterI
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterI)

public:
	Structured2DGridNaysGridImporter();
	~Structured2DGridNaysGridImporter(){}

	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	bool import(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // STRUCTURED2DGRIDNAYSGRIDIMPORTER_H
