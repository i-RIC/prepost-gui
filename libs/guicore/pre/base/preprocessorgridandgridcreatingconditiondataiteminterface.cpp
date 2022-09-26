#include "preprocessorgridandgridcreatingconditiondataiteminterface.h"

#include <QIcon>

PreProcessorGridAndGridCreatingConditionDataItemInterface::PreProcessorGridAndGridCreatingConditionDataItemInterface(const QString& caption, PreProcessorDataItem* parent) :
	PreProcessorDataItem(caption, QIcon(":/libs/guibase/images/iconFolder.svg"), parent)
{}

PreProcessorGridAndGridCreatingConditionDataItemInterface::~PreProcessorGridAndGridCreatingConditionDataItemInterface()
{}
