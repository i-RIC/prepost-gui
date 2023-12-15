#ifndef PREPROCESSORGRIDDATAITEM_IMPL_H
#define PREPROCESSORGRIDDATAITEM_IMPL_H

#include "../preprocessorgriddataitem.h"

#include <vtkDataSetMapper.h>
#include <vtkExtractCells.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

#include <QPixmap>
#include <QCursor>

#include <vector>

class PreProcessorGridCrosssectionWindow;
class GridBirdEyeWindowProjectDataItem;

class PreProcessorGridDataItem::Impl
{
public:
	Impl(PreProcessorGridDataItem* item);
	~Impl();

	PreProcessorGridCrosssectionWindow* buildCrosssectionWindow();

	v4InputGrid* m_grid;

	PreProcessorGridAttributeNodeDataItem* m_nodeDataItem;
	PreProcessorGridAttributeCellDataItem* m_cellDataItem;

	GridBirdEyeWindowProjectDataItem* m_birdEyeWindow;
	std::vector<PreProcessorGridCrosssectionWindow*> m_crosssectionWindows;

	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	SelectedNodesController* m_selectedNodesController;
	SelectedCellsController* m_selectedCellsController;
	SelectedIEdgesController* m_selectedIEdgesController;
	SelectedJEdgesController* m_selectedJEdgesController;
	SelectedEdgesController* m_selectedEdgesController;

	QPixmap m_addPixmap;
	QCursor m_addCursor;

	QMenu* m_menu;
	QMenu* m_generateAttMenu;

	QAction* m_importAction;
	QAction* m_exportAction;

	QAction* m_displaySettingAction;
	QAction* m_polygonSelectAction;
	QAction* m_deleteAction;

	QAction* m_nodeEditAction;
	QAction* m_nodeDisplaySettingAction;
	QAction* m_cellEditAction;
	QAction* m_cellDisplaySettingAction;
	QAction* m_birdEyeWindowAction;

	bool m_shiftPressed;
	bool m_gridIsDeleted;

private:
	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDDATAITEM_IMPL_H
