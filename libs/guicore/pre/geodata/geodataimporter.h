#ifndef GEODATAIMPORTER_H
#define GEODATAIMPORTER_H

#include "../../guicore_global.h"

#include <QObject>
#include <QString>

class GeoData;
class GeoDataCreator;
class SolverDefinitionGridAttribute;
class PreProcessorGeoDataGroupDataItemInterface;

class GUICOREDLL_EXPORT GeoDataImporter : public QObject
{

public:
	/// Constructor
	GeoDataImporter(GeoDataCreator* creator);
	/// Initializes this class to prepare for importing the geodata from the specified file.
	bool importInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	/// Import the geodata into data.
	virtual bool importData(GeoData* data, int index, QWidget* w) = 0;
	virtual const QStringList fileDialogFilters() = 0;
	virtual const QStringList acceptableExtensions() = 0;
	GeoDataCreator* creator() const;
	const QString& caption() const {return m_caption;}

protected:
	virtual bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	QString m_caption;
	QString m_filename;
	QString m_selectedFilter;
};

#endif // GEODATAIMPORTER_H
