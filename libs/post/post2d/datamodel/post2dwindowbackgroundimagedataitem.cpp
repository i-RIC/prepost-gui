#include "post2dwindowbackgroundimagedataitem.h"
#include "../post2dwindow.h"
#include "../post2dwindowgraphicsview.h"

#include <guibase/objectbrowserview.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/backgroundimageinfodialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QAction>
#include <QMenu>
#include <QStandardItem>

#include <vtkRenderer.h>

Post2dWindowBackgroundImageDataItem::Post2dWindowBackgroundImageDataItem(BackgroundImageInfo* image, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_imageInfo {image}
{
	setupStandardItem(NotChecked, Reorderable, NotDeletable, image->caption());
	m_actor = vtkSmartPointer<vtkActor>::New();

	image->setupActor(m_actor);
	renderer()->AddActor(m_actor);
	m_actor->VisibilityOff();
	actorCollection()->AddItem(m_actor);

	m_georeferenceAction = new QAction(Post2dWindowBackgroundImageDataItem::tr("&Georeference..."), this);

	connect(m_georeferenceAction, SIGNAL(triggered()), this, SLOT(showGeoreferenceDialog()));
	connect(image, SIGNAL(isChanged()), this, SLOT(applyImageChange()));
}

Post2dWindowBackgroundImageDataItem::~Post2dWindowBackgroundImageDataItem()
{
	renderer()->RemoveActor(m_actor);
}

void Post2dWindowBackgroundImageDataItem::addCustomMenuItems(QMenu *menu)
{
	menu->addAction(m_georeferenceAction);
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

void Post2dWindowBackgroundImageDataItem::assignActorZValues(const ZDepthRange& range)
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

void Post2dWindowBackgroundImageDataItem::showGeoreferenceDialog()
{
	m_imageInfo->showGeoreferenceDialog(m_actor, dataModel()->graphicsView(), m_zDepthRange.min(), m_zDepthRange.max(), postProcessorWindow());

	connect(m_imageInfo, SIGNAL(isGeoreferenceDialogClosed()), this, SLOT(enableObjectBrowserView()));
	setEnableObjectBrowserView(false);
}

void Post2dWindowBackgroundImageDataItem::enableObjectBrowserView()
{
	setEnableObjectBrowserView(true);
}

void Post2dWindowBackgroundImageDataItem::setEnableObjectBrowserView(bool enabled)
{
	m_georeferenceAction->setEnabled(enabled);

	ObjectBrowserView* oview = dataModel()->objectBrowserView();
	oview->deleteAction()->setEnabled(enabled);
	oview->propertyAction()->setEnabled(enabled);

	oview->setEnabled(enabled);
}

void Post2dWindowBackgroundImageDataItem::doApplyOffset(double /*x_diff*/, double /*y_diff*/)
{
	// offset already applied by PreProcessorBackgroundImageDataItem::doApplyOffset
	m_imageInfo->applySettingToActor(m_actor);
}
