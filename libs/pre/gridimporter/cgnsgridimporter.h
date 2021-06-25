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
	Q_INTERFACES(GridImporterInterface)

public:
	CgnsGridImporter();

	QString caption() const override;
	QStringList fileDialogFilters() const override;
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;

	bool selectZoneForImporting(const iRICLib::H5CgnsFile& file, iRICLib::H5CgnsZone** selectedZone, QWidget* parent);
	void setGridDataItem(PreProcessorGridDataItem* gridDataItem);

protected:
	virtual bool isZoneCompatible(const iRICLib::H5CgnsZone& zone) = 0;

private:
	PreProcessorGridDataItem* m_gridDataItem;
};

#endif // CGNSGRIDIMPORTER_H
