#include "preprocessorgridcreatingconditiondataiteminterface.h"

#include <QIcon>

PreProcessorGridCreatingConditionDataItemInterface::PreProcessorGridCreatingConditionDataItemInterface(GraphicsWindowDataItem* parent) :
	PreProcessorDataItem(tr("Grid Creating Condition"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{}

PreProcessorGridCreatingConditionDataItemInterface::~PreProcessorGridCreatingConditionDataItemInterface()
{}
