#include "streamgage.h"
#include "streamgagepreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

StreamGage::StreamGage(DataItem* parent) :
	Points {parent}
{}

StreamGage::~StreamGage()
{}

QString StreamGage::caption() const
{
	return tr("Stream gage");
}

QStandardItem* StreamGage::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconStreamGage.png"), tr("Stream gage"));
	setupStandardItem(item);
	return item;
}

DataItemView* StreamGage::buildPreProcessorDataItemView(Model* model)
{
	return new StreamGagePreProcessorView(model, this);
}

QString StreamGage::relativeFilename() const
{
	return "streamgage.dat";
}
