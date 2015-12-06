#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOWPROJECTDATAITEM_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOWPROJECTDATAITEM_H

#include <guicore/project/projectdataitem.h>

class Structured2DGrid;
class PreProcessorGridCrosssectionWindow;
class PreProcessorGridAttributeNodeDataItem;

class PreProcessorGridCrosssectionWindowProjectDataItem : public ProjectDataItem
{

public:
	PreProcessorGridCrosssectionWindowProjectDataItem(PreProcessorGridAttributeNodeDataItem* item, const std::string& cond, QWidget* parentWindow);
	~PreProcessorGridCrosssectionWindowProjectDataItem();

	void informWindowClose();
	void requestWindowClose();
	PreProcessorGridCrosssectionWindow* window() const;
	PreProcessorGridAttributeNodeDataItem* conditionNodeDataItem() const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	PreProcessorGridAttributeNodeDataItem* m_item;
	PreProcessorGridCrosssectionWindow* m_window;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOWPROJECTDATAITEM_H
