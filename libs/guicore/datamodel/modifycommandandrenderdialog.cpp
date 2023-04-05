#include "modifycommandandrenderdialog.h"

#include "graphicswindowdataitem.h"

ModifyCommandAndRenderDialog::ModifyCommandAndRenderDialog(GraphicsWindowDataItem* item, QWidget* parent) :
	ModifyCommandDialog(parent),
	m_item {item}
{}

void ModifyCommandAndRenderDialog::pushCommand(QUndoCommand* command)
{
	m_item->pushRenderCommand(command, m_item);
}
