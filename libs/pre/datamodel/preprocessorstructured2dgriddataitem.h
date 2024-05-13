#ifndef PREPROCESSORSTRUCTURED2DGRIDDATAITEM_H
#define PREPROCESSORSTRUCTURED2DGRIDDATAITEM_H

#include "preprocessorgriddataitem.h"

class QMenu;
class QAction;

class PreProcessorStructured2dGridDataItem : public PreProcessorGridDataItem
{
	Q_OBJECT

public:
	PreProcessorStructured2dGridDataItem(PreProcessorDataItem* parent);
	~PreProcessorStructured2dGridDataItem();

	void setEdgeFocus(vtkIdType i, vtkIdType j);
	void clearEdgeFocus();

	void updateActionStatus() override;

private:
	void setupMenu() override;
	void assignActorZValues(const ZDepthRange& range) override;

	vtkPolyDataMapper* m_edgeMapper;
	vtkActor* m_edgeActor;

	QAction* m_regionSelectAction;
	QAction* m_showIndexAction;
	QMenu* m_selectMenu;
	QMenu* m_editMenu;
	QMenu* m_displayMenu;
};

#endif // PREPROCESSORSTRUCTURED2DGRIDDATAITEM_H
