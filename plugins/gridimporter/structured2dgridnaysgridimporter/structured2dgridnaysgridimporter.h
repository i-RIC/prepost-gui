#ifndef STRUCTURED2DGRIDNAYSGRIDIMPORTER_H
#define STRUCTURED2DGRIDNAYSGRIDIMPORTER_H

#include <guicore/pre/grid/gridimporterinterface.h>
#include <QObject>

class Structured2DGridNaysGridImporter : public QObject, public GridImporterInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID GridImporterInterface_iid FILE "extrafilters.json")
	Q_INTERFACES(GridImporterInterface)

public:
	/// Constructor
	Structured2DGridNaysGridImporter();
	~Structured2DGridNaysGridImporter(){}
	QString caption() const;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt)
	{
		return gt == SolverDefinitionGridType::gtStructured2DGrid;
	}
	const QStringList fileDialogFilters();
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent);
};

#endif // STRUCTURED2DGRIDNAYSGRIDIMPORTER_H
