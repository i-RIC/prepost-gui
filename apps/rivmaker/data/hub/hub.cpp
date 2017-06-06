#include "hub.h"
#include "hubpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

Hub::Hub(DataItem* parent) :
	Points {parent}
{}

Hub::~Hub()
{}

QStandardItem* Hub::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconHub.png"), tr("Surveying or turning point"));
	setupStandardItem(item);
	return item;
}

DataItemView* Hub::buildPreProcessorDataItemView(Model* model)
{
	return new HubPreProcessorView(model, this);
}
