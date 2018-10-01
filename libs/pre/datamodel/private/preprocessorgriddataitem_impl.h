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

class GridBirdEyeWindow;

class PreProcessorGridDataItem::Impl
{
public:
	Impl();
	~Impl();

	Grid* m_grid;

	QVector<vtkIdType> m_selectedVertices;
	QVector<vtkIdType> m_selectedCells;
	QVector<Edge> m_selectedEdges;

	PreProcessorGridAttributeNodeDataItem* m_nodeDataItem;
	PreProcessorGridAttributeCellDataItem* m_cellDataItem;

	GridBirdEyeWindow* m_birdEyeWindow;

	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	vtkSmartPointer<vtkPolyData> m_selectedVerticesPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_selectedVerticesMapper;
	vtkSmartPointer<vtkActor> m_selectedVerticesActor;

	vtkSmartPointer<vtkExtractCells> m_selectedCellsGrid;
	vtkSmartPointer<vtkDataSetMapper> m_selectedCellsMapper;
	vtkSmartPointer<vtkActor> m_selectedCellsActor;

	vtkSmartPointer<vtkDataSetMapper> m_selectedCellsLinesMapper;
	vtkSmartPointer<vtkActor> m_selectedCellsLinesActor;

	vtkSmartPointer<vtkPolyData> m_selectedEdgesPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_selectedEdgesMapper;
	vtkSmartPointer<vtkActor> m_selectedEdgesActor;

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

	QAction* m_setupScalarBarAction;


	bool m_shiftPressed;
};

#endif // PREPROCESSORGRIDDATAITEM_IMPL_H
