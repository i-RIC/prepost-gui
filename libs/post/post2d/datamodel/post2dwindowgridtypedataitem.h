#ifndef POST2DWINDOWGRIDTYPEDATAITEM_H
#define POST2DWINDOWGRIDTYPEDATAITEM_H

#include "post2dwindowgridtypedataiteminterface.h"
#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post2dWindowRawDataTopDataItem;
class Post2dWindowZoneDataItem;
class LookupTableContainer;

class Post2dWindowGridTypeDataItem : public Post2dWindowDataItem, public Post2dWindowGridTypeDataItemInterface
{
	Q_OBJECT
public:
	Post2dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	virtual ~Post2dWindowGridTypeDataItem();
	const QList<Post2dWindowZoneDataItem*>& zoneDatas() const {return m_zoneDatas;}
	const QString& name();
	Post2dWindowZoneDataItem* zoneData(const QString& name) const {return m_zoneDataNameMap.value(name);}
	SolverDefinitionGridType* gridType() const override {return m_gridType;}
	Post2dWindowRawDataTopDataItem* rawDataItem() const {return m_rawDataItem;}
	LookupTableContainer* lookupTable(const QString& attName) const override {return m_lookupTables.value(attName, 0);}
	void setupZoneDataItems();
	void update();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	void updateLookupTableRanges();
	void setupScalarsToColors(const QString& name);
	SolverDefinitionGridType* m_gridType;
	QMap<QString, LookupTableContainer*> m_lookupTables;
	QMap<QString, Post2dWindowZoneDataItem*> m_zoneDataNameMap;
	Post2dWindowRawDataTopDataItem* m_rawDataItem;
	QList<Post2dWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_H
