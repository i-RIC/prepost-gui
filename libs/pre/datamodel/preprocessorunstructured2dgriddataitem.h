#ifndef PREPROCESSORUNSTRUCTURED2DGRIDDATAITEM_H
#define PREPROCESSORUNSTRUCTURED2DGRIDDATAITEM_H

#include "preprocessorgriddataitem.h"

class QMenu;
class QAction;

class PreProcessorUnstructured2dGridDataItem : public PreProcessorGridDataItem
{
	Q_OBJECT

public:
	PreProcessorUnstructured2dGridDataItem(PreProcessorDataItem* parent);
	~PreProcessorUnstructured2dGridDataItem();

protected:
	void updateActionStatus() override;
	void setupMenu() override;

private:
	QMenu* m_selectMenu;
	QMenu* m_editMenu;
	QMenu* m_displayMenu;
};

#endif // PREPROCESSORUNSTRUCTURED2DGRIDDATAITEM_H
