#ifndef GEODATAIMPORTER_H
#define GEODATAIMPORTER_H

#include "../../guicore_global.h"

#include <QObject>
#include <QString>

#include <string>

class GeoData;
class GeoDataCreator;
class SolverDefinitionGridAttribute;
class PreProcessorGeoDataDataItemInterface;
class PreProcessorGeoDataGroupDataItemInterface;

class GUICOREDLL_EXPORT GeoDataImporter : public QObject
{

public:
	GeoDataImporter(const std::string& name, const QString& caption, GeoDataCreator* creator);

	std::string name() const;
	QString caption() const;

	PreProcessorGeoDataDataItemInterface* import(const QString& filename, const QString& selectedFilter, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	/// Initializes this class to prepare for importing the geodata from the specified file.
	bool importInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	GeoDataCreator* creator() const;

	/// Import the geodata into data.
	virtual bool importData(GeoData* data, int index, QWidget* w) = 0;
	virtual const QStringList fileDialogFilters() = 0;
	virtual const QStringList acceptableExtensions() = 0;

protected:
	virtual bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);

	QString m_filename;
	QString m_selectedFilter;

private:
	std::string m_name;
	QString m_caption;

};

#endif // GEODATAIMPORTER_H
