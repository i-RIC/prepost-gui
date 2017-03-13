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

View* DataItemViewHelperI::view() const
{
	return m_dataItemView->view();
}
