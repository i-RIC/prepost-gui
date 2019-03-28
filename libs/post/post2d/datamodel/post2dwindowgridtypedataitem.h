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
	~Post2dWindowGridTypeDataItem() override;
	const std::string& name() const;
	const QList<Post2dWindowZoneDataItem*>& zoneDatas() const;
	Post2dWindowZoneDataItem* zoneData(const std::string& name) const;
	SolverDefinitionGridType* gridType() const override;
	Post2dWindowGeoDataTopDataItem* geoDataItem() const;

	LookupTableContainer* nodeLookupTable(const std::string& attName);
	LookupTableContainer* cellLookupTable(const std::string& attName);
	LookupTableContainer* particleLookupTable(const std::string& attName);
	LookupTableContainer* polyDataLookupTable(const std::string& attName);

	void setupZoneDataItems();
	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void updateCellLookupTableRanges();
	void updateNodeLookupTableRanges();
	void updateParticleLookupTableRanges();
	void updatePolyDataLookupTableRanges();

	void setupNodeScalarsToColors(const std::string& name);
	void setupCellScalarsToColors(const std::string& name);
	void setupParticleScalarsToColors(const std::string& name);
	void setupPolyDataScalarsToColors(const std::string& name);

	SolverDefinitionGridType* m_gridType;
	QMap<std::string, LookupTableContainer*> m_cellLookupTables;
	QMap<std::string, LookupTableContainer*> m_nodeLookupTables;
	QMap<std::string, LookupTableContainer*> m_particleLookupTables;
	QMap<std::string, LookupTableContainer*> m_polyDataLookupTables;
	QMap<std::string, Post2dWindowZoneDataItem*> m_zoneDataNameMap;
	Post2dWindowGeoDataTopDataItem* m_geoDataItem;
	QList<Post2dWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_H
