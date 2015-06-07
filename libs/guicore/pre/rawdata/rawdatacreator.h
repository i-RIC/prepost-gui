#ifndef RAWDATACREATOR_H
#define RAWDATACREATOR_H

#include "../../guicore_global.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QIcon>

class QDomNode;
class QToolBar;
class RawData;
class RawDataFactory;
class GraphicsWindowDataItem;
class SolverDefinitionGridAttribute;
class PreProcessorRawDataGroupDataItem;
class ProjectDataItem;
class RawDataMapper;
class RawDataImporter;
class RawDataExporter;

class GUICOREDLL_EXPORT RawDataCreator : public QObject
{

public:
	/// Constructor
	RawDataCreator();
	virtual ~RawDataCreator();
	/// Return true if the corresponding raw data can be used
	/// for this grid related condition.
	virtual bool isCompatibleWith(SolverDefinitionGridAttribute* condition) = 0;
	virtual RawData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) = 0;
	virtual RawData* restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition);
	const QList<RawDataMapper*>& nodeMappers() const {return m_nodeMappers;}
	const QList<RawDataMapper*>& cellMappers() const {return m_cellMappers;}
	const QList<RawDataImporter*>& importers() const {return m_importers;}
	const QList<RawDataExporter*>& exporters() const {return m_exporters;}
	const QIcon& icon() const {return m_icon;}
	const QString& caption() const {return m_caption;}
	const QString& typeName() const {return m_typeName;}
	virtual QString name(unsigned int /*index*/) {return "";}
	virtual bool isCreatable() const {return false;}
	virtual QString defaultCaption(unsigned int index) = 0;
	void setNameAndDefaultCaption(const QList<GraphicsWindowDataItem*>& list, RawData* data);

protected:
	QList<RawDataMapper*> m_nodeMappers;
	QList<RawDataMapper*> m_cellMappers;
	QList<RawDataImporter*> m_importers;
	QList<RawDataExporter*> m_exporters;
	QString m_caption;
	QString m_typeName;
	QIcon m_icon;

	RawData* m_emptyData;
};

#endif // RAWDATACREATOR_H
