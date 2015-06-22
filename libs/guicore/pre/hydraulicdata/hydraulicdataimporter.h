#ifndef HYDRAULICDATAIMPORTER_H
#define HYDRAULICDATAIMPORTER_H

#include <QObject>
#include <QString>

class GeoData;
class HydraulicData;
class HydraulicDataCreator;

class HydraulicDataImporter : public QObject
{

public:
	/// Constructor
	HydraulicDataImporter() {}
	/// Import hydraulicdata from the specified file.
	virtual bool import(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w) = 0;
	/// Returns true if the hydraulic data can be imported to the specified geodata.
	virtual bool canImportTo(GeoData* data) = 0;
	virtual const QStringList fileDialogFilters() = 0;
	const QString& caption() const {return m_caption;}

protected:
	QString m_caption;
};

#endif // HYDRAULICDATAIMPORTER_H
