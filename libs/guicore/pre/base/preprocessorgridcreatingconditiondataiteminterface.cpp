#include "preprocessorgridcreatingconditiondataiteminterface.h"

#include <QIcon>

PreProcessorGridCreatingConditionDataItemInterface::PreProcessorGridCreatingConditionDataItemInterface(GraphicsWindowDataItem* parent) :
	PreProcessorDataItem(tr("Grid Creating Condition"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent)
{}

PreProcessorGridCreatingConditionDataItemInterface::~PreProcessorGridCreatingConditionDataItemInterface()
{}
