#ifndef POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H
#define POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H

#include <post/post2d/datamodel/post2dwindowgridtypedataiteminterface.h>
#include "../post2dbirdeyewindowdataitem.h"
#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post2dBirdEyeWindowZoneDataItem;
class LookupTableContainer;

class Post2dBirdEyeWindowGridTypeDataItem : public Post2dBirdEyeWindowDataItem, public Post2dWindowGridTypeDataItemInterface
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	virtual ~Post2dBirdEyeWindowGridTypeDataItem();
	const QList<Post2dBirdEyeWindowZoneDataItem*>& zoneDatas() const {return m_zoneDatas;}
	const QString& name() const;
	Post2dBirdEyeWindowZoneDataItem* zoneData(const QString& name) const {return m_zoneDataNameMap.value(name);}
	SolverDefinitionGridType* gridType() const override {return m_gridType;}
	LookupTableContainer* lookupTable(const QString& attName) const override {return m_lookupTables.value(attName, 0);}
	void setupZoneDataItems();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateLookupTableRanges();
	void setupScalarsToColors(const QString& name);
	SolverDefinitionGridType* m_gridType;
	QMap<QString, LookupTableContainer*> m_lookupTables;
	QMap<QString, Post2dBirdEyeWindowZoneDataItem*> m_zoneDataNameMap;
	QList<Post2dBirdEyeWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H
