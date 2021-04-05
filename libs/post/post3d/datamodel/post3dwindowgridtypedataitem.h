#ifndef POST3DWINDOWGRIDTYPEDATAITEM_H
#define POST3DWINDOWGRIDTYPEDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <postbase/postwindowgridtypedataiteminterface.h>

#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post3dWindowZoneDataItem;
class LookupTableContainer;

class Post3dWindowGridTypeDataItem : public Post3dWindowDataItem, public PostWindowGridTypeDataItemInterface
{
	Q_OBJECT

public:
	Post3dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	~Post3dWindowGridTypeDataItem();
	const QList<Post3dWindowZoneDataItem*>& zoneDatas() const;
	const std::string& name() const;
	Post3dWindowZoneDataItem* zoneData(const std::string& name) const;
	SolverDefinitionGridType* gridType() const;
	bool isChildCaptionAvailable(const QString& caption);
	LookupTableContainer* nodeLookupTable(const std::string& attName) override;
	LookupTableContainer* cellLookupTable(const std::string& attName) override;
	LookupTableContainer* particleLookupTable(const std::string& attName) override;
	LookupTableContainer* polyDataLookupTable(const std::string&) override {return nullptr;}
	void setValueRange(const std::string& name);
	void setupZoneDataItems();
	void update();

private:
	void updateNodeLookupTableRanges();
	void updateCellLookupTableRanges();
	void updateParticleLookupTableRanges();

	void setupNodeScalarsToColors(const std::string& name);
	void setupCellScalarsToColors(const std::string& name);
	void setupParticleScalarsToColors(const std::string& name);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	SolverDefinitionGridType* m_gridType;
	QMap<std::string, LookupTableContainer*> m_nodeLookupTables;
	QMap<std::string, LookupTableContainer*> m_cellLookupTables;
	QMap<std::string, LookupTableContainer*> m_particleLookupTables;
	QMap<std::string, Post3dWindowZoneDataItem*> m_zoneDataNameMap;
	QList<Post3dWindowZoneDataItem*> m_zoneDatas;
	/// Action to add new condition.
	QAction* m_addConditionAction;
	bool m_isZoneDataItemsSetup;
};

#endif // POST3DWINDOWGRIDTYPEDATAITEM_H
