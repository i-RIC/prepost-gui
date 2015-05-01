#ifndef CGNSGRIDEXPORTER_H
#define CGNSGRIDEXPORTER_H

#include <guicore/pre/grid/gridinternalexporter.h>
#include <QObject>

class CgnsGridExporter : public GridInternalExporter
{
	Q_OBJECT
public:
	CgnsGridExporter();
	virtual ~CgnsGridExporter() {}
	virtual QString caption() const;
	const QStringList fileDialogFilters();
	bool isGridTypeSupported(SolverDefinitionGridType::GridType /*gt*/) {
		return true;
	}
	bool doExport(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent);
	bool createTempCgns(Grid* grid, QString& tmpname, int& fn, int& B);
	bool closeAndMoveCgns(const QString& tmpname, int fn, const QString& cgnsname);
};

#endif // CGNSGRIDEXPORTER_H
