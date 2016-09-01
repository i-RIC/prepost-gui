#ifndef GEODATACREATOR_H
#define GEODATACREATOR_H

#include "../../guicore_global.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QIcon>

#include <vector>

class QDomNode;
class GeoData;
class GraphicsWindowDataItem;
class SolverDefinitionGridAttribute;
class ProjectDataItem;
class GeoDataMapper;
class GeoDataImporter;
class GeoDataExporter;

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

	const QList<GeoDataMapper*>& nodeMappers() const;
	QList<GeoDataMapper*>& nodeMappers();

	const QList<GeoDataMapper*>& cellMappers() const;
	QList<GeoDataMapper*>& cellMappers();

	const QList<GeoDataImporter*>& importers() const;
	QList<GeoDataImporter*>& importers();

	const QList<GeoDataExporter*>& exporters() const;
	QList<GeoDataExporter*>& exporters();

	virtual bool isCreatable() const;

private:
	QString m_caption;
	QString m_typeName;

	QList<GeoDataMapper*> m_nodeMappers;
	QList<GeoDataMapper*> m_cellMappers;

	QList<GeoDataImporter*> m_importers;
	QList<GeoDataExporter*> m_exporters;
};

#endif // GEODATACREATOR_H
