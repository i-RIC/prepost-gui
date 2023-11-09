#ifndef UNSTRUCTURED2DGRIDTRIANGLEIMPORTER_H
#define UNSTRUCTURED2DGRIDTRIANGLEIMPORTER_H

#include <guicore/pre/grid/gridimporteri.h>
#include <QObject>

class Unstructured2dGridTriangleImporter : public QObject, public GridImporterI
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterI)
public:
	Unstructured2dGridTriangleImporter();
	~Unstructured2dGridTriangleImporter(){}

	QString caption() const override;
	SolverDefinitionGridType::GridType supportedGridType() const override;
	QStringList fileDialogFilters() const override;
	bool import(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;
};

#endif // UNSTRUCTURED2DGRIDTRIANGLEIMPORTER_H
