#ifndef GEODATAIMPORTER_H
#define GEODATAIMPORTER_H

#include "../../guicore_global.h"

#include <QObject>
#include <QString>

#include <string>

class GeoData;
class GeoDataCreator;
class SolverDefinitionGridAttribute;
class PreProcessorGeoDataDataItemI;
class PreProcessorGeoDataGroupDataItemI;

class GUICOREDLL_EXPORT GeoDataImporter : public QObject
{

public:
	GeoDataImporter(const std::string& name, const QString& caption, GeoDataCreator* creator);
	virtual ~GeoDataImporter();

	std::string name() const;
	QString caption() const;

	PreProcessorGeoDataDataItemI* import(const QString& filename, const QString& selectedFilter, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);

	GeoDataCreator* creator() const;

	virtual const QStringList fileDialogFilters() = 0;
	virtual const QStringList acceptableExtensions() = 0;

	bool importInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);
	virtual bool importData(GeoData* data, int index, QWidget* w) = 0;

protected:
	QString filename() const;
	QString selectedFilter() const;

private:
	virtual bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/geodataimporter_impl.h"
#endif // _DEBUG

#endif // GEODATAIMPORTER_H
