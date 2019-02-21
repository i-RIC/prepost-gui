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
	const QList<Post2dBirdEyeWindowZoneDataItem*>& zoneDatas() const;

	const std::string& name() const;
	Post2dBirdEyeWindowZoneDataItem* zoneData(const std::string& name) const;
	SolverDefinitionGridType* gridType() const override;
	LookupTableContainer* nodeLookupTable(const std::string& attName) override;
	LookupTableContainer* cellLookupTable(const std::string& attName) override;
	LookupTableContainer* particleLookupTable(const std::string&) override;

	void setupZoneDataItems();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateLookupTableRanges();
	void setupNodeScalarsToColors(const std::string& name);
	void setupCellScalarsToColors(const std::string& name);

	SolverDefinitionGridType* m_gridType;
	QMap<std::string, LookupTableContainer*> m_cellLookupTables;
	QMap<std::string, LookupTableContainer*> m_nodeLookupTables;
	QMap<std::string, Post2dBirdEyeWindowZoneDataItem*> m_zoneDataNameMap;
	QList<Post2dBirdEyeWindowZoneDataItem*> m_zoneDatas;
	bool m_isZoneDataItemsSetup;
};

#endif // POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H
