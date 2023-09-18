#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDDATAWITHIDCONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDDATAWITHIDCONTROLLER_H

#include "../preprocessorgriddataitem.h"
#include "preprocessorgriddataitem_selecteddatacontroller.h"

class PreProcessorGridDataItem::SelectedDataWithIdController : public SelectedDataController
{
public:
	SelectedDataWithIdController(PreProcessorGridDataItem* item);

	void clearSelection() override;
	const std::vector<vtkIdType>& selectedDataIds() const;
	void setSelectedDataIds(const std::vector<vtkIdType>& ids);

protected:
	std::vector<vtkIdType> m_selectedDataIds;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDDATAWITHIDCONTROLLER_H
