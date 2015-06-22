#ifndef GEODATACREATOR_H
#define GEODATACREATOR_H

#include "../../guicore_global.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QIcon>

class QDomNode;
class QToolBar;
class GeoData;
class GeoDataFactory;
class GraphicsWindowDataItem;
class SolverDefinitionGridAttribute;
class PreProcessorGeoDataGroupDataItem;
class ProjectDataItem;
class GeoDataMapper;
class GeoDataImporter;
class GeoDataExporter;

class GUICOREDLL_EXPORT GeoDataCreator : public QObject
{

public:
	/// Constructor
	GeoDataCreator();
	virtual ~GeoDataCreator();
	/// Return true if the corresponding raw data can be used
	/// for this grid related condition.
	virtual bool isCompatibleWith(SolverDefinitionGridAttribute* condition) = 0;
	virtual GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) = 0;
	virtual GeoData* restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition);
	const QList<GeoDataMapper*>& nodeMappers() const {return m_nodeMappers;}
	const QList<GeoDataMapper*>& cellMappers() const {return m_cellMappers;}
	const QList<GeoDataImporter*>& importers() const {return m_importers;}
	const QList<GeoDataExporter*>& exporters() const {return m_exporters;}
	const QIcon& icon() const {return m_icon;}
	const QString& caption() const {return m_caption;}
	const QString& typeName() const {return m_typeName;}
	virtual QString name(unsigned int /*index*/) {return "";}
	virtual bool isCreatable() const {return false;}
	virtual QString defaultCaption(unsigned int index) = 0;
	void setNameAndDefaultCaption(const QList<GraphicsWindowDataItem*>& list, GeoData* data);

protected:
	QList<GeoDataMapper*> m_nodeMappers;
	QList<GeoDataMapper*> m_cellMappers;
	QList<GeoDataImporter*> m_importers;
	QList<GeoDataExporter*> m_exporters;
	QString m_caption;
	QString m_typeName;
	QIcon m_icon;

	GeoData* m_emptyData;
};

#endif // GEODATACREATOR_H
