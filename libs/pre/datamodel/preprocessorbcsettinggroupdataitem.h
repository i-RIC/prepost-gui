#ifndef PREPROCESSORBCSETTINGGROUPDATAITEM_H
#define PREPROCESSORBCSETTINGGROUPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <QMap>

class PreProcessorBCDataItem;
class PreProcessorBCSettingDataItem;
class Grid;
class QAction;

class PreProcessorBCSettingGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorBCSettingGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorBCSettingGroupDataItem() override;

	void informGridUpdate();
	const QList<PreProcessorBCSettingDataItem*> conditions() const;
	PreProcessorBCSettingDataItem* condition(const QString& type, int num) const;
	void addCustomMenuItems(QMenu* menu) override;
	void updateZDepthRangeItemCount() override;
	void updateBCMenu(PreProcessorBCSettingDataItem* item);
	void setupAddActions();
	const QList<QAction*>& addActions() const;
	QAction* dummyEditAction() const;
	QAction* dummyDeleteAction() const;

public slots:
	void executeMapping(bool noDraw = false);
	void updateItems();
	void loadItems();
	void addCondition();

private slots:
	void deleteSelected();
	void deleteAll();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	QMap<PreProcessorBCDataItem*, PreProcessorBCSettingDataItem*> m_itemMap;
	QList<QAction*> m_addActions;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;

	QAction* m_dummyEditAction;
	QAction* m_dummyDeleteAction;
};

#endif // PREPROCESSORBCSETTINGGROUPDATAITEM_H
