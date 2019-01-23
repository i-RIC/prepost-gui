#ifndef POST2DWINDOWGRIDTYPEDATAITEM_H
#define POST2DWINDOWGRIDTYPEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <cgnslib.h>

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
	const QList<Post2dWindowZoneDataItem*>& zoneDatas() const;
	const std::string& name();
	Post2dWindowZoneDataItem* zoneData(const std::string& name) const;
	SolverDefinitionGridType* gridType() const override;
	Post2dWindowGeoDataTopDataItem* geoDataItem() const;

	LookupTableContainer* nodeLookupTable(const std::string& attName) override;
	LookupTableContainer* cellLookupTable(const std::string& attName) override;
	LookupTableContainer* particleLookupTable(const std::string& attName) override;
	void setupZoneDataItems();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateCellLookupTableRanges();
	void updateNodeLookupTableRanges();
	void updateParticleLookupTableRanges();

	void setupNodeScalarsToColors(const std::string& name);
	void setupCellScalarsToColors(const std::string& name);
	void setupParticleScalarsToColors(const std::string& name);

	SolverDefinitionGridType* m_gridType;
	QMap<std::string, LookupTableContainer*> m_cellLookupTables;
	QMap<std::string, LookupTableContainer*> m_nodeLookupTables;
	QMap<std::string, LookupTableContainer*> m_particleLookupTables;
	QMap<std::string, Post2dWindowZoneDataItem*> m_zoneDataNameMap;
	Post2dWindowGeoDataTopDataItem* m_geoDataItem;
	QList<Post2dWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_H
