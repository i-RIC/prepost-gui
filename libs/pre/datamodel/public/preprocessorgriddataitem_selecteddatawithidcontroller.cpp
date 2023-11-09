#include "preprocessorgriddataitem_selecteddatawithidcontroller.h"

PreProcessorGridDataItem::SelectedDataWithIdController::SelectedDataWithIdController(PreProcessorGridDataItem* item) :
	SelectedDataController(item)
{}

void PreProcessorGridDataItem::SelectedDataWithIdController::clearSelection()
{
	std::vector<vtkIdType> empty;
	setSelectedDataIds(empty);
}

const std::vector<vtkIdType>& PreProcessorGridDataItem::SelectedDataWithIdController::selectedDataIds() const
{
	return m_selectedDataIds;
}

void PreProcessorGridDataItem::SelectedDataWithIdController::setSelectedDataIds(const std::vector<vtkIdType>& ids)
{
	m_selectedDataIds = ids;
	updateVtkObjects();
}
