#include "../preprocessorgraphicsview.h"
#include "preprocessorbackgroundimagedataitem.h"
#include "preprocessorbackgroundimagesdataitem.h"
#include "preprocessorrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemmultiselectingdialog.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/qttool.h>

PreProcessorBackgroundImagesDataItem::PreProcessorBackgroundImagesDataItem(GraphicsWindowDataItem* parent) :
	PreProcessorDataItem {tr("Background Images"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_addAction {new QAction(PreProcessorBackgroundImagesDataItem::tr("&Add Image..."), this)},
	m_deleteSelectedAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), PreProcessorBackgroundImagesDataItem::tr("Delete &Selected..."), this)},
	m_deleteAllAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), PreProcessorBackgroundImagesDataItem::tr("Delete &All..."), this)}
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

	projectData()->mainfile()->addRenderer(renderer());
}

void PreProcessorBackgroundImagesDataItem::addChildItem()
{
	BackgroundImageInfo* image = *(projectData()->mainfile()->backgroundImages().begin());
	auto iItem = new PreProcessorBackgroundImageDataItem(image, this);
	// make the standard item top.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(iItem->standardItem()->row());
	m_standardItem->insertRows(0, takenItems);
	m_childItems.insert(m_childItems.begin(), iItem);

	updateItemMap();
	updateZDepthRange();

	dataModel()->objectBrowserView()->select(iItem->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectBackgroundImage(iItem->standardItem()->index());
}

void PreProcessorBackgroundImagesDataItem::deleteChildItem(int i)
{
	auto it = m_childItems.begin();
	delete *(it + i);
	dynamic_cast<PreProcessorRootDataItem*>(parent())->updateItemMap();
	for (auto child : m_childItems) {
		auto bgItem = dynamic_cast<PreProcessorBackgroundImageDataItem*>(child);
		QModelIndex idx = bgItem->standardItem()->index();
		if (idx != dataModel()->objectBrowserView()->currentIndex()) {continue;}
		bgItem->updateMoveUpDownActions(dataModel()->objectBrowserView());
	}
	renderGraphicsView();
}

void PreProcessorBackgroundImagesDataItem::moveUpChildItem(int i)
{
	// reorder the standard item.
	auto items = m_standardItem->takeRow(i);
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

void PreProcessorBackgroundImagesDataItem::moveDownChildItem(int i)
{
	// reorder the standard item.
	auto items = m_standardItem->takeRow(i);
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

void PreProcessorBackgroundImagesDataItem::visiblePropBounds(double bounds[6])
{
	renderer()->ComputeVisiblePropBounds(bounds);
}

void PreProcessorBackgroundImagesDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_addAction);
	menu->addSeparator();
	menu->addAction(m_deleteSelectedAction);
	menu->addAction(m_deleteAllAction);
}

void PreProcessorBackgroundImagesDataItem::deleteSelected()
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

void PreProcessorBackgroundImagesDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all background images?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	while (m_childItems.size() > 0) {
		GraphicsWindowDataItem* item = *(m_childItems.begin());
		projectData()->mainfile()->deleteImage(item->standardItem()->index());
	}
}

void PreProcessorBackgroundImagesDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto nodeList = node.childNodes();
	for (int i = 0; i < nodeList.count(); ++i) {
		auto childElem = nodeList.at(i).toElement();
		if (i < static_cast<int> (m_childItems.size())) {
			auto iItem = dynamic_cast<PreProcessorBackgroundImageDataItem*> (m_childItems.at(i));
			iItem->loadFromProjectMainFile(childElem);
		}
	}
}

void PreProcessorBackgroundImagesDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto child : m_childItems) {
		auto iItem = dynamic_cast<PreProcessorBackgroundImageDataItem*> (child);
		writer.writeStartElement("BackgroundImage");
		iItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
