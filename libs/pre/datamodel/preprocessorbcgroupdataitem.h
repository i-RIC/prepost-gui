#ifndef PREPROCESSORBCGROUPDATAITEM_H
#define PREPROCESSORBCGROUPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>

class PreProcessorBCDataItem;
class ColorSource;
class Grid;

class PreProcessorBCGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorBCGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorBCGroupDataItem();
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void informGridUpdate();
	const QList<PreProcessorBCDataItem*> conditions() const;
	void addCustomMenuItems(QMenu* menu) override;
	void updateZDepthRangeItemCount() override;
	QMenu* bcMenu() {return m_bcMenu;}
	void updateBCMenu(PreProcessorBCDataItem* item);
	void clear();
	void clearPoints();
	PreProcessorBCDataItem* addCondition(int index, bool hideSetting = false);
	void renumberItemsForProject();
	const QList<QAction*> addActions() const {return m_addActions;}

public slots:
	void addCondition();

signals:
	void itemsUpdated();
	void itemsLoaded();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void renumberItemsForCgns();

	QList<QAction*> m_addActions;
	int m_projectBuildNumber;

	QAction* m_dummyEditAction;
	QAction* m_dummyDeleteAction;
	QAction* m_dummyAssignAction;
	QAction* m_dummyReleaseAction;
	QMenu* m_bcMenu;

	ColorSource* m_colorSource;

public:
	friend class PreProcessorBCDataItem;
};

#endif // PREPROCESSORBCGROUPDATAITEM_H
