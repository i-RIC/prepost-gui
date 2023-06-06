#include "../bc/boundaryconditiondialog.h"
#include "preprocessorbcdataitem.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgraphicsview.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "private/preprocessorbcdataitem_impl.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/project/inputcond/inputconditionwidgetfilename.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QLocale>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <h5cgnsbase.h>
#include <h5cgnsbc.h>
#include <h5cgnsfile.h>
#include <h5cgnszone.h>
#include <h5cgnszonebc.h>
#include <iriclib_errorcodes.h>

#define TMPBCNAME "bc"

PreProcessorBCDataItem::Impl::Impl(PreProcessorBCDataItem* item) :
	m_projectNumber {1},
	m_cgnsNumber {1},
	m_opacityPercent {50},
	m_mapped {false},
	m_definingBoundingBox {false},
	m_isCustomModified {false},
	m_dialog {nullptr},
	m_data {vtkPolyData::New()},
	m_mapper {vtkPolyDataMapper::New()},
	m_actor {vtkActor::New()},
	m_nameActor {vtkActor2D::New()},
	m_nameMapper {vtkTextMapper::New()},
	m_editAction {new QAction(PreProcessorBCDataItem::tr("&Edit Condition..."), item)},
	m_assignAction {new QAction(PreProcessorBCDataItem::tr("&Assign Condition"), item)},
	m_releaseAction {new QAction(PreProcessorBCDataItem::tr("&Release Condition"), item)}
{
	m_mapper->SetScalarVisibility(false);
	auto prop = m_actor->GetProperty();
	prop->SetLighting(false);

	m_mapper->SetInputData(m_data);
	m_actor->SetMapper(m_mapper);
}

PreProcessorBCDataItem::Impl::~Impl()
{
	delete m_dialog;
	m_data->Delete();
	m_mapper->Delete();
	m_actor->Delete();
	m_nameActor->Delete();
	m_nameMapper->Delete();
}

PreProcessorBCDataItem::PreProcessorBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, GraphicsWindowDataItem* parent, bool hideSetting) :
	PreProcessorDataItem {tr("New Item"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, Deletable);
	setIsCommandExecuting(true);
	m_standardItem->setEditable(true);
	setIsCommandExecuting(false);

	impl->m_condition = cond;
	impl->m_hideSetting = hideSetting;

	try {
		impl->m_dialog = new BoundaryConditionDialog(this, iricMainWindow(), mainWindow());
		QLocale locale = iricMainWindow()->locale();
		impl->m_dialog->setup(def, cond->element(), locale);
		connect(impl->m_dialog, SIGNAL(accepted()), this, SLOT(setModified()));
	} catch (ErrorMessage&) {
		// m_dialog = nullptr;
	}
	impl->m_dialog->setType(cond->caption());

	impl->m_assignAction->setEnabled(false);
	impl->m_releaseAction->setEnabled(false);

	connect(impl->m_editAction, SIGNAL(triggered()), this, SLOT(showDialog()));
	connect(impl->m_assignAction, SIGNAL(triggered()), this, SLOT(assignSelectedElements()));
	connect(impl->m_releaseAction, SIGNAL(triggered()), this, SLOT(releaseSelectedElements()));

	setupActors();
	updateActorSettings();
}

PreProcessorBCDataItem::~PreProcessorBCDataItem()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->grid();
	if (g != 0) {g->setModified();}

	renderer()->RemoveActor(impl->m_actor);
	renderer()->RemoveActor2D(impl->m_nameActor);

	delete impl;
}

void PreProcessorBCDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_projectNumber = iRIC::getIntAttribute(node, "number");
	impl->m_cgnsNumber = iRIC::getIntAttribute(node, "cgnsNumber");
	if (buildNumber() < 3918) {
		impl->m_cgnsNumber = impl->m_projectNumber;
	}
	impl->m_isCustomModified = iRIC::getBooleanAttribute(node, "isCustomModified");
	impl->m_mapped = iRIC::getBooleanAttribute(node, "isMapped", true);
	impl->m_hideSetting = iRIC::getBooleanAttribute(node, "settingHidden", false);
	setFilename(QString("%1%2.dat").arg(impl->m_condition->name().c_str()).arg(impl->m_projectNumber));
}

void PreProcessorBCDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", impl->m_condition->name().c_str());
	iRIC::setIntAttribute(writer, "number", impl->m_projectNumber);
	iRIC::setIntAttribute(writer, "cgnsNumber", impl->m_cgnsNumber);
	iRIC::setBooleanAttribute(writer, "isCustomModified", impl->m_isCustomModified);
	iRIC::setBooleanAttribute(writer, "isMapped", impl->m_mapped);
	iRIC::setBooleanAttribute(writer, "settingHidden", impl->m_hideSetting);
	setFilename(QString("%1%2.dat").arg(impl->m_condition->name().c_str()).arg(impl->m_projectNumber));
}

void PreProcessorBCDataItem::loadExternalData(const QString& filename)
{
	if (buildNumber() <= 3507) {return;}

	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(filename), iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		auto bc = file.ccBase()->defaultZone()->zoneBc()->bc(TMPBCNAME, 1);
		if (bc == nullptr) {return;}

		auto d = impl->m_dialog;
		d->load(*bc);

		setName(d->caption());
		impl->m_opacityPercent = d->opacityPercent();
	} catch (...) {
		// do nothing
	}
}

void PreProcessorBCDataItem::saveExternalData(const QString& filename)
{
	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(filename), iRICLib::H5CgnsFile::Mode::Create);
		auto base = file.ccBase();

		std::vector<int> size {2, 2, 1, 1};
		auto zone = base->createDefaultZone(iRICLib::H5CgnsZone::Type::Structured, size);
		auto bc = zone->zoneBc()->bc(TMPBCNAME, 1);
		impl->m_dialog->save(bc);
	} catch (...) {
		return;
	}
}

void PreProcessorBCDataItem::setupActors()
{
	// node, edge, cell related settings
	QColor color = impl->m_dialog->color();
	auto prop = impl->m_actor->GetProperty();
	prop->SetColor(color.redF(), color.greenF(), color.blueF());

	renderer()->AddActor(impl->m_actor);
	actorCollection()->AddItem(impl->m_actor);

	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		auto prop = impl->m_actor->GetProperty();
		prop->SetPointSize(7);
		prop->SetRepresentationToPoints();
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		// nothing to do
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		auto prop = impl->m_actor->GetProperty();
		prop->SetLineWidth(7);
		prop->SetRepresentationToWireframe();
	}

	// name related settings
	vtkTextProperty* tprop = impl->m_nameMapper->GetTextProperty();

	tprop->SetJustificationToCentered();
	tprop->SetVerticalJustificationToCentered();

	impl->m_nameActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
	impl->m_nameActor->SetMapper(impl->m_nameMapper);

	renderer()->AddActor2D(impl->m_nameActor);
	actor2DCollection()->AddItem(impl->m_nameActor);
}

void PreProcessorBCDataItem::updateElements()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (tmpparent->grid() == nullptr) {return;}

	auto d = impl->m_data;
	auto grid = tmpparent->grid()->vtkGrid();
	d->Reset();
	d->SetPoints(grid->GetPoints());

	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		for (auto index : impl->m_indices) {
			ca->InsertNextCell(1, &index);
		}
		d->SetVerts(ca);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		for (auto index : impl->m_indices) {
			vtkCell* cell = grid->GetCell(index);
			ca->InsertNextCell(cell->GetPointIds());
		}
		d->SetPolys(ca);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		vtkIdType nodes[2];
		for (const auto& e : impl->m_edges) {
			nodes[0] = e.vertex1();
			nodes[1] = e.vertex2();
			ca->InsertNextCell(2, &(nodes[0]));
		}
		d->SetLines(ca);
	}
	d->Modified();
}

void PreProcessorBCDataItem::updateActorSettings()
{
	auto prop = impl->m_actor->GetProperty();

	QColor color = impl->m_dialog->color();
	prop->SetColor(color.redF(), color.greenF(), color.blueF());
	prop->SetOpacity(static_cast<double>(impl->m_opacityPercent) / 100);

	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		prop->SetPointSize(impl->m_dialog->pointSize());
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		// nothing to do
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		prop->SetLineWidth(impl->m_dialog->pointSize());
	}
	updateNameActorSettings();
}

void PreProcessorBCDataItem::updateNameActorSettings()
{
	actor2DCollection()->RemoveItem(impl->m_nameActor);
	impl->m_nameActor->VisibilityOff();

	if (! impl->m_dialog->showName()) {return;}

	double centerv[3] = {0, 0, 0};
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (tmpparent->grid() == 0) {return;}
	vtkPointSet* pset = tmpparent->grid()->vtkGrid();
	vtkPoints* points = pset->GetPoints();
	int pnum = 0;
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		for (auto index : impl->m_indices) {
			double tmpv[3];
			points->GetPoint(index, tmpv);
			for (int i = 0; i < 3; ++i) {
				centerv[i] += tmpv[i];
			}
			++ pnum;
		}
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		for (auto index : impl->m_indices) {
			vtkCell* cell = pset->GetCell(index);
			for (int i = 0; i < cell->GetNumberOfPoints(); ++i) {
				double tmpv[3];
				points->GetPoint(cell->GetPointId(i), tmpv);
				for (int i = 0; i < 3; ++i) {
					centerv[i] += tmpv[i];
				}
				++ pnum;
			}
		}
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		for (const auto& e : impl->m_edges) {
			double tmpv[3];
			points->GetPoint(e.vertex1(), tmpv);
			for (int i = 0; i < 3; ++i) {
				centerv[i] += tmpv[i];
			}
			points->GetPoint(e.vertex2(), tmpv);
			for (int i = 0; i < 3; ++i) {
				centerv[i] += tmpv[i];
			}
			pnum += 2;
		}
	}

	if (pnum == 0) {
		return;
	}

	for (int i = 0; i < 3; ++i) {
		centerv[i] /= pnum;
	}
	actor2DCollection()->AddItem(impl->m_nameActor);
	vtkCoordinate* coord = impl->m_nameActor->GetPositionCoordinate();
	coord->SetValue(centerv[0], centerv[1], centerv[2]);
	impl->m_nameMapper->SetInput(iRIC::toStr(impl->m_dialog->caption()).c_str());

	auto tprop = impl->m_nameMapper->GetTextProperty();
	const auto& nameSetting = dynamic_cast<PreProcessorBCGroupDataItem*> (parent())->nameSetting();
	nameSetting.applySetting(tprop);

	QColor color = impl->m_dialog->color();
	tprop->SetColor(color.redF(), color.greenF(), color.blueF());
	impl->m_nameMapper->Modified();

	updateVisibilityWithoutRendering();
}

void PreProcessorBCDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.max());
}

void PreProcessorBCDataItem::setModified(bool modified)
{
	PreProcessorDataItem::setModified(modified);
}

void PreProcessorBCDataItem::assignSelectedElements()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		const auto& selVertices = tmpparent->selectedVertices();
		for (int i = 0; i < selVertices.size(); ++i) {
			impl->m_indices.insert(selVertices[i]);
		}
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		const auto& selCells = tmpparent->selectedCells();
		for (int i = 0; i < selCells.size(); ++i) {
			impl->m_indices.insert(selCells[i]);
		}
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		const QVector<Edge>& selEdges = tmpparent->selectedEdges();
		for (int i = 0; i < selEdges.count(); ++i) {
			impl->m_edges.insert(selEdges[i]);
		}
	}
	updateElements();
	updateNameActorSettings();
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gItem->grid()->setModified();
	renderGraphicsView();
	impl->m_isCustomModified = true;
}

void PreProcessorBCDataItem::releaseSelectedElements()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		const auto& selVertices = tmpparent->selectedVertices();
		for (int i = 0; i < selVertices.size(); ++i) {
			impl->m_indices.remove(selVertices[i]);
		}
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		const auto& selCells = tmpparent->selectedCells();
		for (int i = 0; i < selCells.size(); ++i) {
			impl->m_indices.remove(selCells[i]);
		}
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		const QVector<Edge>& selEdges = tmpparent->selectedEdges();
		for (int i = 0; i < selEdges.count(); ++i) {
			impl->m_edges.remove(selEdges[i]);
		}
	}
	updateElements();
	updateNameActorSettings();
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gItem->grid()->setModified();
	renderGraphicsView();
	impl->m_isCustomModified = true;
}

void PreProcessorBCDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMouseMoveEvent(event, v);
		} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMouseMoveEvent(event, v);
		} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->edgeSelectingMouseMoveEvent(event, v);
		}
	}
}

void PreProcessorBCDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		impl->m_definingBoundingBox = true;
		if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMousePressEvent(event, v);
		} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMousePressEvent(event, v);
		} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->edgeSelectingMousePressEvent(event, v);
		}
	}
}

void PreProcessorBCDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (event->button() == Qt::LeftButton) {
		if (impl->m_definingBoundingBox) {
			bool selected = false;
			if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
				auto v2 = dynamic_cast<VTK2DGraphicsView*> (v);
				tmpparent->nodeSelectingMouseReleaseEvent(event, v2);
				selected = (tmpparent->selectedVertices().size() > 0);
			} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
				tmpparent->cellSelectingMouseReleaseEvent(event, v);
				selected = (tmpparent->selectedCells().size() > 0);
			} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
				tmpparent->edgeSelectingMouseReleaseEvent(event, v);
				selected = (tmpparent->selectedEdges().size() > 0);
			}
			impl->m_assignAction->setEnabled(selected);
			impl->m_releaseAction->setEnabled(selected);
		}
		impl->m_definingBoundingBox = false;
		v->setCursor(tmpparent->normalCursor());
	} else if (event->button() == Qt::RightButton) {
		if (menu != nullptr) {delete menu;}
		menu = new QMenu(projectData()->mainWindow());

		menu->addAction(impl->m_assignAction);
		menu->addAction(impl->m_releaseAction);
		menu->addSeparator();
		menu->addAction(impl->m_editAction);

		menu->move(event->globalPos());
		menu->show();
	}
}

void PreProcessorBCDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		gitem->nodeSelectingKeyPressEvent(event, v);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		gitem->cellSelectingKeyPressEvent(event, v);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		gitem->edgeSelectingKeyPressEvent(event, v);
	}
}

void PreProcessorBCDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		gitem->nodeSelectingKeyReleaseEvent(event, v);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		gitem->cellSelectingKeyReleaseEvent(event, v);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		gitem->cellSelectingKeyReleaseEvent(event, v);
	}
}

void PreProcessorBCDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_editAction);
}

void PreProcessorBCDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(true);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(true);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedEdgesVisibility(true);
	}
}

void PreProcessorBCDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(false);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(false);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedEdgesVisibility(false);
	}
}

void PreProcessorBCDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
	setName(m_standardItem->data(Qt::EditRole).toString());
	emit itemUpdated();
}

int PreProcessorBCDataItem::loadFromCgnsFile(const iRICLib::H5CgnsZone& zone)
{
	if (impl->m_cgnsNumber == 0) {
		// There is no need to load from CGNS file.
		return IRIC_NO_ERROR;
	}

	auto bc = zone.zoneBc()->bc(impl->m_condition->name(), impl->m_cgnsNumber);

	std::vector<int> indices;
	bc->readIndices(&indices);
	setupIndicesAndEdges(indices);

	impl->m_dialog->load(*bc);

	updateElements();
	updateActorSettings();

	return IRIC_NO_ERROR;
}

int PreProcessorBCDataItem::saveToCgnsFile(iRICLib::H5CgnsZone* zone)
{
	if (! isValid()) {return IRIC_NO_ERROR;}

	std::vector<int> indices;
	buildIndices(&indices);

	auto bc = zone->zoneBc()->bc(impl->m_condition->name(), impl->m_cgnsNumber);
	bc->writeIndices(indices);
	impl->m_dialog->save(bc);

	return IRIC_NO_ERROR;
}

void PreProcessorBCDataItem::handleStandardItemDoubleClicked()
{
	showDialog();
}

void PreProcessorBCDataItem::setName(const QString& name)
{
	m_standardItem->setText(name);
	impl->m_dialog->setCaption(name);
}

void PreProcessorBCDataItem::setProjectNumber(int num)
{
	impl->m_projectNumber = num;
}

void PreProcessorBCDataItem::setCgnsNumber(int num)
{
	impl->m_cgnsNumber = num;
}

int PreProcessorBCDataItem::projectNumber() const
{
	return impl->m_projectNumber;
}

int PreProcessorBCDataItem::cgnsNumber() const
{
	return impl->m_cgnsNumber;
}

void PreProcessorBCDataItem::setColor(const QColor& color)
{
	impl->m_dialog->setColor(color);
}

QColor PreProcessorBCDataItem::color() const
{
	return impl->m_dialog->color();
}

int PreProcessorBCDataItem::opacityPercent() const
{
	return impl->m_dialog->opacityPercent();
}

SolverDefinitionBoundaryCondition* PreProcessorBCDataItem::condition() const
{
	return impl->m_condition;
}

bool PreProcessorBCDataItem::showDialog()
{
	// set default folder for filename input conditions.
	InputConditionWidgetFilename::defaultFolder = LastIODirectory::get();
	// show dialog
	auto d = impl->m_dialog;
	d->setOpacityPercent(impl->m_opacityPercent);
	d->setEditMode();
	int ret = d->exec();
	if (ret == QDialog::Rejected) {return false;}

	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (gItem->grid() != nullptr) {
		gItem->grid()->setModified();
	}

	setName(d->caption());
	impl->m_opacityPercent = d->opacityPercent();
	updateActorSettings();

	// set the default folder back.
	LastIODirectory::set(InputConditionWidgetFilename::defaultFolder);
	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	emit itemUpdated();

	renderGraphicsView();
	return true;
}

void PreProcessorBCDataItem::clearPoints()
{
	impl->m_indices.clear();
	impl->m_edges.clear();
	updateElements();
	updateNameActorSettings();
}

void PreProcessorBCDataItem::assignIndices(const QSet<vtkIdType>& vertices)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (gitem->grid() == nullptr) {return;}
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		impl->m_indices = vertices;
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		impl->m_indices.clear();
		QVector<vtkIdType> cells = gitem->getCellsFromVertices(vertices);
		for (int i = 0; i < cells.count(); ++i) {
			impl->m_indices.insert(cells.at(i));
		}
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		impl->m_edges.clear();
		const QVector<Edge> edges = gitem->getEdgesFromVertices(vertices);
		for (int i = 0; i < edges.count(); ++i) {
			impl->m_edges.insert(edges.at(i));
		}
	}
	updateElements();
	updateNameActorSettings();
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gItem->grid()->setModified();
	impl->m_isCustomModified = false;
}

bool PreProcessorBCDataItem::isCustomModified() const
{
	return impl->m_isCustomModified;
}

bool PreProcessorBCDataItem::isValid() const
{
	bool valid = true;
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
			impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		valid = impl->m_indices.count() > 0;
	} else {
		valid = impl->m_edges.count() > 0;
	}
	return valid;
}


int PreProcessorBCDataItem::buildNumber() const
{
	PreProcessorBCGroupDataItem* gItem = dynamic_cast<PreProcessorBCGroupDataItem*>(parent());
	return gItem->m_projectBuildNumber;
}

void PreProcessorBCDataItem::setupIndicesAndEdges(const std::vector<int> indices)
{
	auto gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());

	Structured2DGrid* sgrid = dynamic_cast<Structured2DGrid*>(gitem->grid());
	if (sgrid == nullptr) {
		// unstructured grid.
		if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
				impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			impl->m_indices.clear();
			for (int i = 0; i < indices.size(); ++i) {
				impl->m_indices.insert(indices[i] - 1);
			}
		} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			for (int i = 0; i < indices.size() / 2; ++i) {
				Edge e(indices[i * 2] - 1, indices[i * 2 + 1] - 1);
				impl->m_edges.insert(e);
			}
		}
	} else {
		// structured grid.
		impl->m_indices.clear();
		if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			for (int idx = 0; idx < indices.size() / 2; ++idx) {
				int i = indices[idx * 2];
				int j = indices[idx * 2 + 1];
				impl->m_indices.insert(sgrid->vertexIndex(i - 1, j - 1));
			}
		} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			for (int idx = 0; idx < indices.size() / 2; ++idx) {
				int i = indices[idx * 2];
				int j = indices[idx * 2 + 1];
				impl->m_indices.insert(sgrid->cellIndex(i - 1, j - 1));
			}
		} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			for (int idx = 0; idx < indices.size() / 4; ++idx) {
				int i1 = indices[idx * 4];
				int j1 = indices[idx * 4 + 1];
				int i2 = indices[idx * 4 + 2];
				int j2 = indices[idx * 4 + 3];
				Edge e(sgrid->vertexIndex(i1 - 1, j1 - 1), sgrid->vertexIndex(i2 - 1, j2 - 1));
				impl->m_edges.insert(e);
			}
		}
	}
}

void PreProcessorBCDataItem::buildIndices(std::vector<int>* indices)
{
	auto gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	auto sgrid = dynamic_cast<Structured2DGrid*>(gitem->grid());
	QVector<int> tmplist;
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
			impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		for (int index : impl->m_indices) {
			tmplist.push_back(index);
		}
		qSort(tmplist);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		QVector<Edge> tmpedges;
		for (const auto& e : impl->m_edges) {
			tmpedges.append(e);
		}
		qSort(tmpedges);
		for (const auto& e : tmpedges) {
			tmplist.push_back(e.vertex1());
			tmplist.push_back(e.vertex2());
		}
	}
	if (sgrid == nullptr) {
		// unstructured grid
		indices->assign(tmplist.size(), 0);
		for (int idx = 0; idx < tmplist.size(); ++idx) {
			int index = tmplist[idx];
			(*indices)[idx] = index + 1;
		}
	} else {
		// structured grid
		indices->assign(tmplist.size() * 2, 0);
		for (int idx = 0; idx < tmplist.size(); ++idx) {
			int index = tmplist[idx];
			unsigned int i, j;
			if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
					impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
				sgrid->getIJIndex(index, &i, &j);
			} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
				sgrid->getCellIJIndex(index, &i, &j);
			}
			(*indices)[idx * 2] = i + 1;
			(*indices)[idx * 2 + 1] = j + 1;
		}
	}
}

QString PreProcessorBCDataItem::caption() const
{
	return impl->m_dialog->caption();
}

void PreProcessorBCDataItem::setMapped(bool mapped)
{
	impl->m_mapped = mapped;
}

bool PreProcessorBCDataItem::mapped() const
{
	return impl->m_mapped;
}

QAction* PreProcessorBCDataItem::editAction() const
{
	return impl->m_editAction;
}

QAction* PreProcessorBCDataItem::deleteAction() const
{
	return PreProcessorDataItem::deleteAction();
}

QAction* PreProcessorBCDataItem::assignAction() const
{
	return impl->m_assignAction;
}

QAction* PreProcessorBCDataItem::releaseAction() const
{
	return impl->m_releaseAction;
}

bool PreProcessorBCDataItem::hideSetting() const
{
	return impl->m_hideSetting;
}

std::string PreProcessorBCDataItem::uniqueName() const
{
	QString name("%1_%2");
	QString ret = name.arg(impl->m_condition->name().c_str()).arg(impl->m_projectNumber);
	return iRIC::toStr(ret);
}

void PreProcessorBCDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	this->updateActorSettings();
}
