#ifndef PROJECTGRIDIMPORTER_H
#define PROJECTGRIDIMPORTER_H

#include <guicore/pre/grid/gridinternalimporter.h>
#include <QObject>

class CgnsGridImporter;
class PreProcessorGridDataItem;

class ProjectGridImporter : public GridInternalImporter
{
	Q_OBJECT
	Q_INTERFACES(GridImporterInterface)

public:
	ProjectGridImporter();
	virtual ~ProjectGridImporter();

	void setGridDataItem(PreProcessorGridDataItem* gridDataItem);

	QString caption() const override;
	QStringList fileDialogFilters() const override;

	/// Import grid data from external file.
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;

protected:
	virtual CgnsGridImporter* getCgnsImporter() = 0;

private:
	PreProcessorGridDataItem* m_gridDataItem;
};

#endif // PROJECTGRIDIMPORTER_H
