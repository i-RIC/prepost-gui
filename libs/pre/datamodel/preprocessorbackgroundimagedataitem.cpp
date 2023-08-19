#include "preprocessorbackgroundimagedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/backgroundimageinfodialog.h>

#include <QAction>
#include <QMenu>
#include <QStandardItem>
#include <QToolBar>

#include <vtkRenderer.h>

PreProcessorBackgroundImageDataItem::PreProcessorBackgroundImageDataItem(BackgroundImageInfo* image, PreProcessorDataItem* parent) :
	PreProcessorDataItem {image->fileName(), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_actor {},
	m_imageInfo {image}
{
	setupStandardItem(Checked, Reorderable, Deletable);

	image->setupActor(m_actor.actor());
	image->setupMapper(m_actor.mapper());
	image->setPreProcessorActor(m_actor.actor());

	renderer()->AddActor(m_actor.actor());
	m_actorCollection->AddItem(m_actor.actor());

	updateVisibilityWithoutRendering();

	m_georeferenceAction = new QAction(PreProcessorBackgroundImageDataItem::tr("&Georeference..."), this);

	connect(m_georeferenceAction, SIGNAL(triggered()), this, SLOT(showGeoreferenceDialog()));
	connect(image, SIGNAL(isChanged()), this, SLOT(applyImageChange()));
}

PreProcessorBackgroundImageDataItem::~PreProcessorBackgroundImageDataItem()
{
	renderer()->RemoveActor(m_actor.actor());
}

void PreProcessorBackgroundImageDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_georeferenceAction);
}

void PreProcessorBackgroundImageDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_imageInfo->mousePressEvent(m_actor.actor(), event, v);
}

void PreProcessorBackgroundImageDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_imageInfo->mouseMoveEvent(m_actor.actor(), event, v);
}

void PreProcessorBackgroundImageDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_imageInfo->mouseReleaseEvent(m_actor.actor(), event, v);
}

void PreProcessorBackgroundImageDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	QStandardItem* parentItem = dynamic_cast<PreProcessorDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}

void PreProcessorBackgroundImageDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void PreProcessorBackgroundImageDataItem::doLoadFromProjectMainFile(const QDomNode&)
{
	updateVisibilityWithoutRendering();
}

void PreProcessorBackgroundImageDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("fileName", m_imageInfo->fileName());
}

void PreProcessorBackgroundImageDataItem::assignActorZValues(const ZDepthRange& range)
{
	double position[3];
	m_actor.actor()->GetPosition(position);
	m_actor.actor()->SetPosition(position[0], position[1], range.min());
}

BackgroundImageInfo* PreProcessorBackgroundImageDataItem::imageInfo() const
{
	return m_imageInfo;
}

QAction* PreProcessorBackgroundImageDataItem::fixAction()
{
	return imageInfo()->fixAction();
}

bool PreProcessorBackgroundImageDataItem::addToolBarButtons(QToolBar* toolbar)
{
	toolbar->addAction(imageInfo()->fixActionWithIcon());
	return true;
}

void PreProcessorBackgroundImageDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
}

void PreProcessorBackgroundImageDataItem::applyImageChange()
{
	m_imageInfo->applySettingToActor(m_actor.actor());
	updateVisibility();
}

QDialog* PreProcessorBackgroundImageDataItem::propertyDialog(QWidget* /*parent*/)
{
	return m_imageInfo->propertyDialog();
}

void PreProcessorBackgroundImageDataItem::handlePropertyDialogAccepted(QDialog* dialog)
{
	auto infoDialog = dynamic_cast<BackgroundImageInfoDialog*>(dialog);
	m_imageInfo->handlePropertyDialogAccepted(infoDialog);
}

void PreProcessorBackgroundImageDataItem::showGeoreferenceDialog()
{
	m_imageInfo->showGeoreferenceDialog(m_actor.actor(), dataModel()->graphicsView(), m_zDepthRange.min(), m_zDepthRange.max(), preProcessorWindow());

	connect(m_imageInfo, SIGNAL(isGeoreferenceDialogClosed()), this, SLOT(enableObjectBrowserView()));
	setEnableObjectBrowserView(false);
}

void PreProcessorBackgroundImageDataItem::enableObjectBrowserView()
{
	setEnableObjectBrowserView(true);
}

void PreProcessorBackgroundImageDataItem::setEnableObjectBrowserView(bool enabled)
{
	m_georeferenceAction->setEnabled(enabled);

	ObjectBrowserView* oview = dataModel()->objectBrowserView();
	oview->deleteAction()->setEnabled(enabled);
	oview->propertyAction()->setEnabled(enabled);

	oview->setEnabled(enabled);
}

void PreProcessorBackgroundImageDataItem::doApplyOffset(double x, double y)
{
	m_imageInfo->applySettingToActor(m_actor.actor());
}
