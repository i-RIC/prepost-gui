#include "dataitemview.h"
#include "dataitemviewhelperi.h"

DataItemViewHelperI::DataItemViewHelperI(DataItemView* dataItemView) :
	m_dataItemView {dataItemView}
{}

DataItemViewHelperI::~DataItemViewHelperI()
{}

bool DataItemViewHelperI::prepareDraw()
{
	return true;
}

DataItem* DataItemViewHelperI::dataItem() const
{
	return m_dataItemView->item();
}

Model* DataItemViewHelperI::model() const
{
	return m_dataItemView->model();
}

View* DataItemViewHelperI::view() const
{
	return m_dataItemView->view();
}
