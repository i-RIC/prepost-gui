#ifndef STRUCTURED2DGRIDVTKIMPORTER_H
#define STRUCTURED2DGRIDVTKIMPORTER_H

#include <guicore/pre/grid/gridimporteri.h>
#include <QObject>

class Structured2dGridVtkImporter : public QObject, public GridImporterI
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterI)
public:
	Structured2dGridVtkImporter();
	~Structured2dGridVtkImporter(){}

	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	bool import(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // STRUCTURED2DGRIDVTKIMPORTER_H
