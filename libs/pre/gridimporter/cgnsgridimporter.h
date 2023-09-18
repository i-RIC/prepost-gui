#ifndef CGNSGRIDIMPORTER_H
#define CGNSGRIDIMPORTER_H

#include <guicore/pre/grid/gridinternalimporter.h>
#include <QObject>

namespace iRICLib {
	class H5CgnsFile;
	class H5CgnsZone;
} // namespase iRICLib

class PreProcessorGridDataItem;

class CgnsGridImporter : public GridInternalImporter
{
	Q_OBJECT
	Q_INTERFACES(GridImporterI)

public:
	CgnsGridImporter();

	QString caption() const override;
	QStringList fileDialogFilters() const override;
	bool import(v4InputGrid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;

	virtual bool isZoneCompatible(const iRICLib::H5CgnsZone& zone) = 0;

private:
	PreProcessorGridDataItem* m_gridDataItem;
	bool m_disableWarning;
};

#endif // CGNSGRIDIMPORTER_H
