#ifndef CGNSGRIDIMPORTER_H
#define CGNSGRIDIMPORTER_H

#include <guicore/pre/grid/gridinternalimporter.h>
#include <QObject>

class CgnsGridImporter : public GridInternalImporter
{
	Q_OBJECT
	Q_INTERFACES(GridImporterInterface)

public:
	CgnsGridImporter();
	virtual ~CgnsGridImporter() {}
	QString caption() const override;
	QStringList fileDialogFilters() const override;
	bool import(Grid* grid, const QString& filename, const QString& selectedFilter, QWidget* parent) override;

	bool openCgnsFileForImporting(Grid* grid, const QString& filename, QString& tmpname, int& fn, int& base, int& zoneid, QWidget* parent);
	void closeAndRemoveTempCgnsFile(int fn, const QString& filename);

protected:
	virtual bool isZoneCompatible(int fn, int base, int Z) = 0;
};

#endif // CGNSGRIDIMPORTER_H
