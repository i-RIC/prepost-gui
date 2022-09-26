#include "../post2dwindowgraphicsview.h"
#include "post2dwindowbackgroundimagedataitem.h"
#include "post2dwindowbackgroundimagesdataitem.h"
#include "post2dwindowrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemmultiselectingdialog.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

Post2dWindowBackgroundImagesDataItem::Post2dWindowBackgroundImagesDataItem(GraphicsWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Background Images"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_addAction {new QAction(Post2dWindowBackgroundImagesDataItem::tr("&Add Image"), this)},
	m_deleteSelectedAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), Post2dWindowBackgroundImagesDataItem::tr("Delete &Selected..."), this)},
	m_deleteAllAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), Post2dWindowBackgroundImagesDataItem::tr("Delete &All..."), this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	m_standardItem->setData(QVariant("BACKGROUNDIMAGES"), Qt::UserRole + 10);

	connect(m_addAction, SIGNAL(triggered()), projectData()->mainfile(), SLOT(addBackgroundImage()));
	connect(m_deleteSelectedAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));
	connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageAdded()), this, SLOT(addChildItem()));
	connect(this, SIGNAL(selectBackgroundImage(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageDeleted(int)), this, SLOT(deleteChildItem(int)));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageMovedUp(int)), this, SLOT(moveUpChildItem(int)));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageMovedDown(int)), this, SLOT(moveDownChildItem(int)));
	connect(dynamic_cast<Post2dWindowRootDataItem*>(this->parent()), SIGNAL(standardModelSetuped()), this, SLOT(setupChildItem()));
	connect(this, SIGNAL(requestRemoveRenderer(vtkRenderer*)), projectData()->mainfile(), SLOT(removeRenderer(vtkRenderer*)));
	connect(projectData()->mainfile(), SIGNAL(backgroundActorVisibilityChanged(int,bool)), this, SLOT(updateChildCheckState(int,bool)));

	projectData()->mainfile()->addRenderer(renderer());
}

Post2dWindowBackgroundImagesDataItem::~Post2dWindowBackgroundImagesDataItem()
{
	emit requestRemoveRenderer(renderer());
}

void Post2dWindowBackgroundImagesDataItem::setupChildItem()
{
	if (projectData()->mainfile()->backgroundImages().size() == 0) { return; }

	const auto& images = projectData()->mainfile()->backgroundImages();
	for (auto it = images.begin(); it != images.end(); ++it) {
		Post2dWindowBackgroundImageDataItem* iItem = new Post2dWindowBackgroundImageDataItem(*it, this);
		// there is no need to make the standard item top.
		m_childItems.push_back(iItem);
	}
	updateItemMap();
	updateZDepthRange();

	dataModel()->graphicsView()->ResetCameraClippingRange();
	setModified();
}

void Post2dWindowBackgroundImagesDataItem::addChildItem()
{
	BackgroundImageInfo* image = *(projectData()->mainfile()->backgroundImages().begin());
	Post2dWindowBackgroundImageDataItem* iItem = new Post2dWindowBackgroundImageDataItem(image, this);
	// make the standard item top.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(iItem->standardItem()->row());
	m_standardItem->insertRows(0, takenItems);
	m_childItems.insert(m_childItems.begin(), iItem);

	updateItemMap();
	updateZDepthRange();

	dataModel()->objectBrowserView()->select(iItem->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectBackgroundImage(iItem->standardItem()->index());
	setModified();
}

void Post2dWindowBackgroundImagesDataItem::deleteChildItem(int i)
{
	auto it = m_childItems.begin();
	delete *(it + i);
	dynamic_cast<Post2dWindowRootDataItem*>(parent())->updateItemMap();
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowBackgroundImageDataItem* bgItem = dynamic_cast<Post2dWindowBackgroundImageDataItem*>(*it);
		QModelIndex idx = bgItem->standardItem()->index();
		if (idx != dataModel()->objectBrowserView()->currentIndex()) { continue; }
		bgItem->updateMoveUpDownActions(dataModel()->objectBrowserView());
	}
	renderGraphicsView();
}

void Post2dWindowBackgroundImagesDataItem::moveUpChildItem(int i)
{
	// reorder the standard item.
	QList<QStandardItem*> items = m_standardItem->takeRow(i);
	m_standardItem->insertRows(i - 1, items);

	// reorder the m_childItems.
	auto it = m_childItems.begin() + i;
	auto it2 = it - 1;
	GraphicsWindowDataItem* item = *it;
	*it = *it2;
	*it2 = item;

	// update the ZDepthRange.
	updateZDepthRange();
	renderGraphicsView();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
}

void Post2dWindowBackgroundImagesDataItem::moveDownChildItem(int i)
{
	// reorder the standard item.
	QList<QStandardItem*> items = m_standardItem->takeRow(i);
	m_standardItem->insertRows(i + 1, items);

	// reorder the m_childList.
	auto it = m_childItems.begin() + i;
	auto it2 = it + 1;
	GraphicsWindowDataItem* item = *it;
	*it = *it2;
	*it2 = item;

	// update the ZDepthRange.
	updateZDepthRange();
	renderGraphicsView();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
}

void Post2dWindowBackgroundImagesDataItem::visiblePropBounds(double bounds[6])
{
	renderer()->ComputeVisiblePropBounds(bounds);
}

void Post2dWindowBackgroundImagesDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_addAction);
	menu->addSeparator();
	menu->addAction(m_deleteSelectedAction);
	menu->addAction(m_deleteAllAction);
}

void Post2dWindowBackgroundImagesDataItem::updateChildCheckState(int idx, bool vis)
{
	dataModel()->itemModel()->blockSignals(true);
	if (idx < 0) {
		if (vis) {
			m_standardItem->setCheckState(Qt::Checked);
		} else {
			m_standardItem->setCheckState(Qt::Unchecked);
		}
	} else {
		auto it = m_childItems.begin() + idx;
		Post2dWindowBackgroundImageDataItem* item = dynamic_cast<Post2dWindowBackgroundImageDataItem*>(*it);
		if (vis) {
			item->standardItem()->setCheckState(Qt::Checked);
		} else {
			item->standardItem()->setCheckState(Qt::Unchecked);
		}
	}
	dataModel()->itemModel()->blockSignals(false);
	updateVisibility();
	renderGraphicsView();
}

void Post2dWindowBackgroundImagesDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	projectData()->mainfile()->updateActorVisibility(-1, m_standardItem->checkState() == Qt::Checked);
}

void Post2dWindowBackgroundImagesDataItem::deleteSelected()
{
	if (m_childItems.size() == 0) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("There is no background image."), QMessageBox::Ok);
		return;
	}

	auto items = m_childItems;

	std::vector<QString> names;
	for (auto item : items) {
		names.push_back(item->standardItem()->text());
	}

	ItemMultiSelectingDialog dialog(mainWindow());
	dialog.setWindowTitle(tr("Delete selected background images"));
	dialog.setItems(names);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto settings = dialog.selectSettings();
	auto mainfile = projectData()->mainfile();
	for (int i = 0; i < settings.size(); ++i) {
		if (settings.at(i)) {
			// delete the item
			mainfile->deleteImage(items.at(i)->standardItem()->index());
		}
	}
}

void Post2dWindowBackgroundImagesDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all background images?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	while (m_childItems.size() > 0) {
		GraphicsWindowDataItem* item = *(m_childItems.begin());
		projectData()->mainfile()->deleteImage(item->standardItem()->index());
	}
}
