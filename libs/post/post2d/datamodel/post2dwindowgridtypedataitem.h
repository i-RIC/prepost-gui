#ifndef POST2DWINDOWGRIDTYPEDATAITEM_H
#define POST2DWINDOWGRIDTYPEDATAITEM_H

#include "post2dwindowgridtypedataiteminterface.h"
#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post2dWindowGeoDataTopDataItem;
class Post2dWindowZoneDataItem;
class LookupTableContainer;

class Post2dWindowGridTypeDataItem : public Post2dWindowDataItem, public Post2dWindowGridTypeDataItemInterface
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
	LookupTableContainer* lookupTable(const std::string& attName) const override {return m_lookupTables.value(attName, 0);}
	void setupZoneDataItems();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateLookupTableRanges();
	void setupScalarsToColors(const std::string& name);
	SolverDefinitionGridType* m_gridType;
	QMap<std::string, LookupTableContainer*> m_lookupTables;
	QMap<std::string, Post2dWindowZoneDataItem*> m_zoneDataNameMap;
	Post2dWindowGeoDataTopDataItem* m_geoDataItem;
	QList<Post2dWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_H
