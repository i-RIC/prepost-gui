#ifndef POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H
#define POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <postbase/postwindowgridtypedataiteminterface.h>
#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post2dBirdEyeWindowZoneDataItem;
class LookupTableContainer;

class Post2dBirdEyeWindowGridTypeDataItem : public Post2dBirdEyeWindowDataItem, public PostWindowGridTypeDataItemInterface
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	virtual ~Post2dBirdEyeWindowGridTypeDataItem();
	const QList<Post2dBirdEyeWindowZoneDataItem*>& zoneDatas() const {return m_zoneDatas;}

	const std::string& name() const;
	Post2dBirdEyeWindowZoneDataItem* zoneData(const std::string& name) const {return m_zoneDataNameMap.value(name);}
	SolverDefinitionGridType* gridType() const override {return m_gridType;}
	LookupTableContainer* nodeLookupTable(const std::string& attName) const override {return m_nodeLookupTables.value(attName, 0);}
	LookupTableContainer* particleLookupTable(const std::string&) const override {return nullptr;}

	void setupZoneDataItems();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateLookupTableRanges();
	void setupScalarsToColors(const std::string& name);

	SolverDefinitionGridType* m_gridType;
	QMap<std::string, LookupTableContainer*> m_nodeLookupTables;
	QMap<std::string, Post2dBirdEyeWindowZoneDataItem*> m_zoneDataNameMap;
	QList<Post2dBirdEyeWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H
