#ifndef CGNSGRIDEXPORTER_H
#define CGNSGRIDEXPORTER_H

#include <guicore/pre/grid/gridinternalexporter.h>
#include <QObject>

class CgnsGridExporter : public GridInternalExporter
{
	Q_OBJECT

public:
	CgnsGridExporter();
	~CgnsGridExporter() override;

	QString caption() const override;
	QStringList fileDialogFilters() const override;
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override;
	bool doExport(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, CoordinateSystem* cs, QWidget* parent) override;
};

#endif // CGNSGRIDEXPORTER_H
