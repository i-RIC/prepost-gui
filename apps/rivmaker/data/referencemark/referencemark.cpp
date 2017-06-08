#include "referencemark.h"
#include "referencemarkpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

ReferenceMark::ReferenceMark(DataItem* parent) :
	Points {parent}
{}

ReferenceMark::~ReferenceMark()
{}

QString ReferenceMark::caption() const
{
	return tr("Reference mark");
}

QStandardItem* ReferenceMark::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconReferenceMark.png"), tr("Reference mark"));
	setupStandardItem(item);
	return item;
}

DataItemView* ReferenceMark::buildPreProcessorDataItemView(Model* model)
{
	return new ReferenceMarkPreProcessorView(model, this);
}

QString ReferenceMark::relativeFilename() const
{
	return "referencemark.dat";
}
