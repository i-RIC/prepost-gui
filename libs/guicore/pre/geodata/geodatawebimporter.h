#ifndef GEODATAWEBIMPORTER_H
#define GEODATAWEBIMPORTER_H

#include "../../guicore_global.h"

#include <QObject>
#include <QString>

#include <string>

class GeoData;
class GeoDataCreator;
class SolverDefinitionGridAttribute;
class PreProcessorGeoDataDataItemInterface;
class PreProcessorGeoDataGroupDataItemInterface;

class GUICOREDLL_EXPORT GeoDataWebImporter : public QObject
{
public:
	GeoDataWebImporter(const std::string& name, const QString& caption, GeoDataCreator* creator);

	std::string name() const;
	QString caption() const;
	virtual bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const;

	PreProcessorGeoDataDataItemInterface* import(SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	/// Initializes this class to prepare for importing the geodata from the specified file.
	bool importInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	GeoDataCreator* creator() const;

	/// Import the geodata into data.
	virtual bool importData(GeoData* data, int index, QWidget* w) = 0;

protected:
	virtual bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);

private:
	std::string m_name;
	QString m_caption;
};

#endif // GEODATAWEBIMPORTER_H
