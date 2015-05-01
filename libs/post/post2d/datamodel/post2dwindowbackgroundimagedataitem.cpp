#include "post2dwindowbackgroundimagedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/backgroundimageinfodialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QAction>
#include <QStandardItem>

#include <vtkRenderer.h>

Post2dWindowBackgroundImageDataItem::Post2dWindowBackgroundImageDataItem(BackgroundImageInfo* image, Post2dWindowDataItem* parent)
	: Post2dWindowDataItem("", QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_imageInfo = image;
	m_standardItem->setText(image->caption());
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);
	m_actor = vtkSmartPointer<vtkActor>::New();
	image->setupActor(m_actor);
	renderer()->AddActor(m_actor);

	m_actor->VisibilityOff();
	actorCollection()->AddItem(m_actor);

	m_isReorderable = true;
	m_standardItemCopy = m_standardItem->clone();

	connect(image, SIGNAL(isChanged()), this, SLOT(applyImageChange()));
}

Post2dWindowBackgroundImageDataItem::~Post2dWindowBackgroundImageDataItem()
{
	renderer()->RemoveActor(m_actor);
}

void Post2dWindowBackgroundImageDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_imageInfo->mousePressEvent(m_actor, event, v);
}

void Post2dWindowBackgroundImageDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_imageInfo->mouseMoveEvent(m_actor, event, v);
}

void Post2dWindowBackgroundImageDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_imageInfo->mouseReleaseEvent(m_actor, event, v);
}

void Post2dWindowBackgroundImageDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	QStandardItem* parentItem = dynamic_cast<Post2dWindowDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}

void Post2dWindowBackgroundImageDataItem::assignActionZValues(const ZDepthRange& range)
{
	double position[3];
	m_actor->GetPosition(position);
	m_actor->SetPosition(position[0], position[1], range.min());
}

void Post2dWindowBackgroundImageDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
//	projectData()->mainfile()->updateActorVisibility(m_standardItem->index().row(), m_standardItem->checkState() == Qt::Checked);
}

void Post2dWindowBackgroundImageDataItem::applyImageChange()
{
	m_imageInfo->applySettingToActor(m_actor);
	updateVisibility();
}


QDialog* Post2dWindowBackgroundImageDataItem::propertyDialog(QWidget* /*parent*/)
{
	QDialog* dialog = m_imageInfo->propertyDialog();
	return dialog;
}

void Post2dWindowBackgroundImageDataItem::handlePropertyDialogAccepted(QDialog* dialog)
{
	BackgroundImageInfoDialog* infoDialog = dynamic_cast<BackgroundImageInfoDialog*>(dialog);
	m_imageInfo->handlePropertyDialogAccepted(infoDialog);
}
