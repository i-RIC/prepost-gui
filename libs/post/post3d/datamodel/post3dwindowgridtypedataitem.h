#ifndef POST3DWINDOWGRIDTYPEDATAITEM_H
#define POST3DWINDOWGRIDTYPEDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post3dWindowZoneDataItem;
class LookupTableContainer;

class Post3dWindowGridTypeDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
public:
	Post3dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	~Post3dWindowGridTypeDataItem();
	const QList<Post3dWindowZoneDataItem*>& zoneDatas() const {return m_zoneDatas;}
	const QString& name() const;
	Post3dWindowZoneDataItem* zoneData(const QString& name) const {return m_zoneDataNameMap.value(name);}
	SolverDefinitionGridType* gridType() const {return m_gridType;}
	bool isChildCaptionAvailable(const QString& caption);
	LookupTableContainer* lookupTable(const QString& attName) const {return m_lookupTables.value(attName, 0);}
	void setValueRange(const QString& name);
	void setupZoneDataItems();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateLookupTableRanges();
	void setupScalarsToColors(const QString& name);
	const QString nextChildCaption();
	const QString nextChildZonename();
	SolverDefinitionGridType* m_gridType;
	QMap<QString, LookupTableContainer*> m_lookupTables;
	QMap<QString, Post3dWindowZoneDataItem*> m_zoneDataNameMap;
	QList<Post3dWindowZoneDataItem*> m_zoneDatas;
	/// Action to add new condition.
	QAction* m_addConditionAction;
	bool m_isZoneDataItemsSetup;
};

#endif // POST3DWINDOWGRIDTYPEDATAITEM_H
