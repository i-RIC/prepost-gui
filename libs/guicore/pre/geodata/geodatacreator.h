#ifndef GEODATACREATOR_H
#define GEODATACREATOR_H

#include "../../guicore_global.h"
#include <QObject>

#include <vector>

class GeoData;
class GeoDataImporter;
class GeoDataExporter;
class GeoDataMapper;
class GeoDataWebImporter;
class GraphicsWindowDataItem;
class ProjectDataItem;
class SolverDefinitionGridAttribute;

class QDomNode;
class QString;

class GUICOREDLL_EXPORT GeoDataCreator : public QObject
{

public:
	GeoDataCreator(const QString& typeName, const QString& caption);
	virtual ~GeoDataCreator();

	virtual QString name(unsigned int index) const;
	const QString& typeName() const;

	virtual QString defaultCaption(unsigned int index) const = 0;
	const QString& caption() const;

	void setNameAndDefaultCaption(const std::vector<GraphicsWindowDataItem*>& list, GeoData* data);

	/// Return true if the corresponding geo data can be used　for this grid related condition.
	virtual bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const = 0;

	virtual GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) = 0;
	virtual GeoData* restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition);

	const std::vector<GeoDataMapper*>& nodeMappers() const;
	std::vector<GeoDataMapper*>& nodeMappers();

	const std::vector<GeoDataMapper*>& cellMappers() const;
	std::vector<GeoDataMapper*>& cellMappers();

	const std::vector<GeoDataImporter*>& importers() const;
	std::vector<GeoDataImporter*>& importers();

	const std::vector<GeoDataWebImporter*>& webImporters() const;
	std::vector<GeoDataWebImporter*>& webImporters();

	const std::vector<GeoDataExporter*>& exporters() const;
	std::vector<GeoDataExporter*>& exporters();

	virtual bool isCreatable() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/geodatacreator_impl.h"
#endif // _DEBUG

#endif // GEODATACREATOR_H
