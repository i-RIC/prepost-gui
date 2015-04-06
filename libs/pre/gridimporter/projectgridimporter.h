#ifndef PROJECTGRIDIMPORTER_H
#define PROJECTGRIDIMPORTER_H

#include <guicore/pre/grid/gridinternalimporter.h>
#include <QObject>

class CgnsGridImporter;

class ProjectGridImporter : public GridInternalImporter
{
	Q_OBJECT
	Q_INTERFACES(GridImporterInterface)

public:
	ProjectGridImporter();
	virtual ~ProjectGridImporter(){}
	QString caption() const;
	const QStringList fileDialogFilters();
	/// Import grid data from external file.
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent);

protected:
	virtual CgnsGridImporter* getCgnsImporter() = 0;
};

#endif // PROJECTGRIDIMPORTER_H
