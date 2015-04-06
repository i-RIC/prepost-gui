#ifndef GRAPHICSWINDOWROOTDATAITEM_H
#define GRAPHICSWINDOWROOTDATAITEM_H

#include "../guicore_global.h"
#include "graphicswindowdataitem.h"
#include <QMap>

class GraphicsWindowDataModel;
class QMainWindow;

class GUICOREDLL_EXPORT GraphicsWindowRootDataItem : public GraphicsWindowDataItem
{
public:
	/// Constructor
	GraphicsWindowRootDataItem(QMainWindow* window, ProjectDataItem* parent);
	virtual ~GraphicsWindowRootDataItem();

	void handleItemChange(QStandardItem* changedItem);
	void handleItemClick(QStandardItem* clickedItem);
	void handleItemDoubleClick(QStandardItem* clickedItem);
	virtual void deleteItem(QStandardItem* item);
	void moveUpItem(QStandardItem* item);
	void moveDownItem(QStandardItem* item);
	void updateItemMap();
	GraphicsWindowDataItem* modelItemFromItem(QStandardItem* item);
	void loadFromCgnsFile(const int fn);
	void closeCgnsFile();

protected:
	bool isAncientChecked(){return true;}
	virtual void doLoadFromProjectMainFile(const QDomNode& /*node*/){}
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/){}
	GraphicsWindowDataModel* dataModel(){return m_dataModel;}

protected:
	QMainWindow* m_mainWindow;
	GraphicsWindowDataModel* m_dataModel;

private:
	QMap<QStandardItem*, GraphicsWindowDataItem*> m_itemMap;

public:
	friend class GraphicsWindowDataModel;
};

#endif // GRAPHICSWINDOWROOTDATAITEM_H
