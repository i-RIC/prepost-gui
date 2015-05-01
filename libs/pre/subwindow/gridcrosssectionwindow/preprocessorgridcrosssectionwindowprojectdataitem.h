#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOWPROJECTDATAITEM_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOWPROJECTDATAITEM_H

#include <guicore/project/projectdataitem.h>

class Structured2DGrid;
class PreProcessorGridCrosssectionWindow;
class PreProcessorGridRelatedConditionNodeDataItem;

class PreProcessorGridCrosssectionWindowProjectDataItem : public ProjectDataItem
{
public:
	PreProcessorGridCrosssectionWindowProjectDataItem(PreProcessorGridRelatedConditionNodeDataItem* item, const QString& cond, QWidget* parentWindow);
	~PreProcessorGridCrosssectionWindowProjectDataItem();
	void informWindowClose();
	void requestWindowClose();
	PreProcessorGridCrosssectionWindow* window() {
		return m_window;
	}
	PreProcessorGridRelatedConditionNodeDataItem* conditionNodeDataItem() {return m_item;}
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	PreProcessorGridRelatedConditionNodeDataItem* m_item;
	PreProcessorGridCrosssectionWindow* m_window;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOWPROJECTDATAITEM_H
