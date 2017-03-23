#include "crosssectionspreprocessorcontroller.h"
#include "../crosssection/crosssection.h"
#include "../base/model.h"
#include "../base/view.h"
#include "../project/project.h"
#include "../../io/crosssections/crosssectionsimporter.h"
#include "../../io/crosssections/crosssectionsexporter.h"
#include "../../window/preprocessor/preprocessormodel.h"

#include "private/crosssectionspreprocessorcontroller_impl.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>

CrossSectionsPreProcessorController::Impl::Impl() :
	m_addAction {new QAction(tr("Add"), nullptr)},
	m_importAction {new QAction(QIcon(":/images/iconImport.png"), tr("&Import.."), nullptr)},
	m_exportAction {new QAction(QIcon(":/images/iconExport.png"), tr("&Export.."), nullptr)}
{}

CrossSectionsPreProcessorController::Impl::~Impl()
{
	delete m_addAction;
	delete m_importAction;
	delete m_exportAction;
}

CrossSectionsPreProcessorController::CrossSectionsPreProcessorController(Model* model, CrossSections* item) :
	DataItemController {model, item},
	impl {new Impl {}}
{
	connect(impl->m_addAction, SIGNAL(triggered()), this, SLOT(addCrossSection()));
	connect(impl->m_importAction, SIGNAL(triggered()), this, SLOT(importData()));
	connect(impl->m_exportAction, SIGNAL(triggered()), this, SLOT(exportData()));
}

CrossSectionsPreProcessorController::~CrossSectionsPreProcessorController()
{
	delete impl;
}

void CrossSectionsPreProcessorController::importData()
{
	auto crossSections = dynamic_cast<CrossSections*> (item());
	std::vector<CrossSection*> newCrossSections;
	QPointF offset = crossSections->project()->offset();

	bool ok = CrossSectionsImporter::importData(&newCrossSections, &offset, view());
	if (! ok) {return;}

	setCrossSections(newCrossSections);
	crossSections->project()->setOffset(offset);
}

void CrossSectionsPreProcessorController::exportData()
{
	if (item()->childItems().size() == 0) {
		QMessageBox::warning(view(), tr("Warning"), tr("Cross Section lines not defined yet."));
		return;
	}
	auto crossSections = dynamic_cast<CrossSections*> (item());
	CrossSectionsExporter::exportData(crossSections->crossSectionVector(), crossSections->project()->offset(), view());
}

void CrossSectionsPreProcessorController::addCrossSection()
{
	auto crossSections = dynamic_cast<CrossSections*> (item());
	auto cs = crossSections->addCrossSection();

	dynamic_cast<PreProcessorModel*> (model())->setupStandardItemAndViewAndController(cs, crossSections);

	model()->select(cs);
}

void CrossSectionsPreProcessorController::setCrossSections(const std::vector<CrossSection*>& crossSections)
{
	auto css = dynamic_cast<CrossSections*> (item());
	auto preModel = dynamic_cast<PreProcessorModel*> (model());
	for (CrossSection* cs : crossSections)	{
		cs->setParent(css);
		preModel->setupStandardItemAndViewAndController(cs, css);
	}
	preModel->updateCrossSections();
}

void CrossSectionsPreProcessorController::rebuildStandardItemsAndViews()
{
	auto m = model();
	auto itm = item();
	auto mySItem = m->standardItem(itm);
	auto myView = m->dataItemView(itm);

	auto selectedCs = dynamic_cast<CrossSection*> (m->selectedItem());

	for (auto cs : itm->childItems()) {
		auto sItem = m->standardItem(cs);
		mySItem->takeRow(sItem->row());
		m->removeStandardItem(cs);

		myView->removeChildItem(m->dataItemView(cs));
		m->removeDataItemView(cs);
	}

	int id = 0;
	for (auto cs : item()->childItems()) {
		auto cs2 = dynamic_cast<CrossSection*> (cs);
		cs2->setId(id++);
		auto sItem = cs2->buildPreProcessorStandardItem();
		mySItem->appendRow(sItem);
		m->addStandardItem(cs, sItem);

		auto v = cs2->buildPreProcessorDataItemView(m);
		myView->addChildItem(v);
		m->addDataItemView(cs, v);
	}

	if (selectedCs == nullptr) {return;}

	m->select(selectedCs);
}

void CrossSectionsPreProcessorController::setupObjectBrowserRightClickMenu(QMenu* menu)
{
	menu->addAction(impl->m_importAction);
	menu->addAction(impl->m_exportAction);
	menu->addSeparator();
	menu->addAction(impl->m_addAction);
}
