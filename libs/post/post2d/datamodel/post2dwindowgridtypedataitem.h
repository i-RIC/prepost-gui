#ifndef POST2DWINDOWGRIDTYPEDATAITEM_H
#define POST2DWINDOWGRIDTYPEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <postbase/postwindowgridtypedataiteminterface.h>

#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post2dWindowGeoDataTopDataItem;
class Post2dWindowZoneDataItem;
class LookupTableContainer;

class Post2dWindowGridTypeDataItem : public Post2dWindowDataItem, public PostWindowGridTypeDataItemInterface
{
	Q_OBJECT

public:
	Post2dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	virtual ~Post2dWindowGridTypeDataItem();
	const QList<Post2dWindowZoneDataItem*>& zoneDatas() const {return m_zoneDatas;}
	const std::string& name();
	Post2dWindowZoneDataItem* zoneData(const std::string& name) const {return m_zoneDataNameMap.value(name);}
	SolverDefinitionGridType* gridType() const override {return m_gridType;}
	Post2dWindowGeoDataTopDataItem* geoDataItem() const {return m_geoDataItem;}
	LookupTableContainer* nodeLookupTable(const std::string& attName) const {return m_nodeLookupTables.value(attName, nullptr);}
	LookupTableContainer* particleLookupTable(const std::string& attName) const {return m_particleLookupTables.value(attName, nullptr);}
	void setupZoneDataItems();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateNodeLookupTableRanges();
	void updateParticleLookupTableRanges();

	void setupNodeScalarsToColors(const std::string& name);
	void setupParticleScalarsToColors(const std::string& name);

	SolverDefinitionGridType* m_gridType;
	QMap<std::string, LookupTableContainer*> m_nodeLookupTables;
	QMap<std::string, LookupTableContainer*> m_particleLookupTables;
	QMap<std::string, Post2dWindowZoneDataItem*> m_zoneDataNameMap;
	Post2dWindowGeoDataTopDataItem* m_geoDataItem;
	QList<Post2dWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_H
