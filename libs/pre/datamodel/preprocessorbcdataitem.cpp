#include "../bc/boundaryconditiondialog.h"
#include "preprocessorbcdataitem.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgriddataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/project/inputcond/inputconditionwidgetfilename.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
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

#include <vtkActor2DCollection.h>
#include <vtkCellArray.h>
#include <vtkExtractEdges.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkVertex.h>

#include <cgnslib.h>
#include <iriclib.h>

#define TMPBCNAME "bc"

PreProcessorBCDataItem::PreProcessorBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, GraphicsWindowDataItem* parent, bool hideSetting) :
	PreProcessorDataItem {tr("New Item"), QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_condition {cond},
	m_projectNumber {1},
	m_cgnsNumber {1},
	m_opacityPercent {50},
	m_definingBoundingBox {false},
	m_isCustomModified {false},
	m_hideSetting {hideSetting}
{
	if (hideSetting) {
		setupStandardItem(Checked, NotReorderable, Deletable);
	} else {
		setupStandardItem(Checked, NotReorderable, NotDeletable);
	}
	setIsCommandExecuting(true);
	m_standardItem->setEditable(true);
	setIsCommandExecuting(false);

	try {
		m_dialog = new BoundaryConditionDialog(this, iricMainWindow(), mainWindow());
		QLocale locale = iricMainWindow()->locale();
		m_dialog->setup(def, cond->element(), locale);
		connect(m_dialog, SIGNAL(accepted()), this, SLOT(setModified()));
	} catch (ErrorMessage&) {
		m_dialog = nullptr;
	}
	m_dialog->setType(m_condition->caption());

	m_editAction = new QAction(PreProcessorBCDataItem::tr("&Edit Condition..."), this);
	connect(m_editAction, SIGNAL(triggered()), this, SLOT(showDialog()));
	m_assignAction = new QAction(PreProcessorBCDataItem::tr("&Assign Condition"), this);
	m_assignAction->setEnabled(false);
	connect(m_assignAction, SIGNAL(triggered()), this, SLOT(assignSelectedElements()));
	m_releaseAction = new QAction(PreProcessorBCDataItem::tr("&Release Condition"), this);
	m_releaseAction->setEnabled(false);
	connect(m_releaseAction, SIGNAL(triggered()), this, SLOT(releaseSelectedElements()));

	setupActors();
	updateActorSettings();
}

PreProcessorBCDataItem::~PreProcessorBCDataItem()
{
	delete m_dialog;

	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->grid();
	if (g != 0) {g->setModified();}

	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		renderer()->RemoveActor(m_verticesActor);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		renderer()->RemoveActor(m_cellsActor);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		renderer()->RemoveActor(m_edgesActor);
	}
	renderer()->RemoveActor2D(m_nameActor);
}

void PreProcessorBCDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_projectNumber = iRIC::getIntAttribute(node, "number");
	m_cgnsNumber = iRIC::getIntAttribute(node, "cgnsNumber");
	if (buildNumber() < 3918) {
		m_cgnsNumber = m_projectNumber;
	}
	m_isCustomModified = iRIC::getBooleanAttribute(node, "isCustomModified");
	m_mapped = iRIC::getBooleanAttribute(node, "isMapped", true);
	m_hideSetting = iRIC::getBooleanAttribute(node, "settingHidden", false);
	setFilename(QString("%1%2.dat").arg(m_condition->name().c_str()).arg(m_projectNumber));
}

void PreProcessorBCDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", m_condition->name().c_str());
	iRIC::setIntAttribute(writer, "number", m_projectNumber);
	iRIC::setIntAttribute(writer, "cgnsNumber", m_cgnsNumber);
	iRIC::setBooleanAttribute(writer, "isCustomModified", m_isCustomModified);
	iRIC::setBooleanAttribute(writer, "isMapped", m_mapped);
	iRIC::setBooleanAttribute(writer, "settingHidden", m_hideSetting);
	setFilename(QString("%1%2.dat").arg(m_condition->name().c_str()).arg(m_projectNumber));
}

void PreProcessorBCDataItem::loadExternalData(const QString& filename)
{
	if (buildNumber() <= 3507) {return;}
	int fn;
	int ier = cg_open(iRIC::toStr(filename).c_str(), CG_MODE_MODIFY, &fn);
	if (ier != 0) {
		// Opening CGNS file failed.
		return;
	}
	cg_iRIC_Init(fn);
	// when loading, use 1 for number.
	m_dialog->setNameAndNumber(TMPBCNAME, 1);
	m_dialog->load(fn);
	cg_close(fn);

	setName(m_dialog->caption());
	m_opacityPercent = m_dialog->opacityPercent();
}

void PreProcessorBCDataItem::saveExternalData(const QString& filename)
{
	ProjectCgnsFile::createNewFile(filename, 2, 2);
	int fn;
	cg_open(iRIC::toStr(filename).c_str(), CG_MODE_MODIFY, &fn);
	cg_iRIC_Init(fn);
	// add dummy grid.
	cgsize_t size[6];
	int Z;
	size[0] = size[1] = 2;
	size[2] = size[3] = 1;
	size[4] = size[5] = 0;
	cg_zone_write(fn, 1, "iRICZone", size, Structured, &Z);
	cg_iRIC_Set_ZoneId(Z);
	// dummy indices.
	cgsize_t indices[1];
	indices[0] = 1;
	cg_iRIC_Write_BC_Indices(const_cast<char*>(TMPBCNAME), 1, 1, indices);

	m_dialog->setNameAndNumber(TMPBCNAME, 1);
	m_dialog->save(fn);
	cg_close(fn);
}

void PreProcessorBCDataItem::setupActors()
{
	QColor color = m_dialog->color();
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		m_verticesGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
		m_verticesMapper = vtkSmartPointer<vtkDataSetMapper>::New();
		m_verticesMapper->SetScalarVisibility(false);
		m_verticesMapper->SetInputData(m_verticesGrid);
		m_verticesActor = vtkSmartPointer<vtkActor>::New();
		vtkProperty* prop = m_verticesActor->GetProperty();
		prop->SetPointSize(7);
		prop->SetLighting(false);
		prop->SetColor(color.redF(), color.greenF(), color.blueF());
		prop->SetRepresentationToPoints();
		m_verticesActor->SetMapper(m_verticesMapper);
		m_verticesActor->VisibilityOff();
		renderer()->AddActor(m_verticesActor);
		actorCollection()->AddItem(m_verticesActor);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		m_cellsGrid = vtkSmartPointer<vtkExtractCells>::New();
		// put m_verticesGrid as dummy.
		m_verticesGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
		m_cellsGrid->SetInputData(m_verticesGrid);
//		m_cellsGrid->SetInput(tmpparent->grid()->vtkGrid());
		m_cellsMapper = vtkSmartPointer<vtkDataSetMapper>::New();
		m_cellsMapper->SetScalarVisibility(false);
		m_cellsMapper->SetInputData(m_cellsGrid->GetOutput());
		m_cellsActor = vtkSmartPointer<vtkActor>::New();
		m_cellsActor->SetMapper(m_cellsMapper);
		vtkProperty* prop = m_cellsActor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(color.redF(), color.greenF(), color.blueF());
		m_cellsActor->VisibilityOff();
		renderer()->AddActor(m_cellsActor);
		actorCollection()->AddItem(m_cellsActor);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		m_edgesPolyData = vtkSmartPointer<vtkPolyData>::New();
		m_edgesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		m_edgesMapper->SetScalarVisibility(false);
		m_edgesMapper->SetInputData(m_edgesPolyData);
		m_edgesActor = vtkSmartPointer<vtkActor>::New();
		vtkProperty* prop = m_edgesActor->GetProperty();
		prop->SetLineWidth(7);
		prop->SetLighting(false);
		prop->SetColor(color.redF(), color.greenF(), color.blueF());
		prop->SetRepresentationToWireframe();
		m_edgesActor->SetMapper(m_edgesMapper);
		m_edgesActor->VisibilityOff();
		renderer()->AddActor(m_edgesActor);
		actorCollection()->AddItem(m_edgesActor);
	}
	m_nameMapper = vtkSmartPointer<vtkTextMapper>::New();
	vtkTextProperty* tprop = m_nameMapper->GetTextProperty();
	tprop->SetFontFamilyToArial();
	tprop->SetFontSize(14);
	tprop->SetJustificationToCentered();
	tprop->SetVerticalJustificationToCentered();
	m_nameActor = vtkSmartPointer<vtkActor2D>::New();
	m_nameActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
	m_nameActor->SetMapper(m_nameMapper);

	renderer()->AddActor2D(m_nameActor);
	actor2DCollection()->AddItem(m_nameActor);
}

void PreProcessorBCDataItem::updateElements()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (tmpparent->grid() == nullptr) {return;}
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		m_verticesGrid->Reset();
		m_verticesGrid->SetPoints(tmpparent->grid()->vtkGrid()->GetPoints());
		for (auto it = m_indices.begin(); it != m_indices.end(); ++it) {
			vtkIdType index = *it;
			vtkSmartPointer<vtkVertex> v = vtkSmartPointer<vtkVertex>::New();
			v->GetPointIds()->SetId(0, index);
			m_verticesGrid->InsertNextCell(v->GetCellType(), v->GetPointIds());
		}
		m_verticesGrid->BuildLinks();
		m_verticesGrid->Modified();
		m_verticesActor->VisibilityOn();
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		vtkSmartPointer<vtkIdList> cellids = vtkSmartPointer<vtkIdList>::New();
		for (auto it = m_indices.begin(); it != m_indices.end(); ++it) {
			int index = *it;
			cellids->InsertNextId(index);
		}
		m_cellsGrid->SetInputData(tmpparent->grid()->vtkGrid());
		m_cellsGrid->SetCellList(cellids);
		m_cellsGrid->Modified();
		m_cellsActor->VisibilityOn();
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		vtkPointSet* ps = tmpparent->grid()->vtkGrid();
		m_edgesPolyData->Reset();
		m_edgesPolyData->SetPoints(ps->GetPoints());
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		vtkIdType nodes[2];
		for (auto it = m_edges.begin(); it != m_edges.end(); ++it) {
			Edge e = *it;
			nodes[0] = e.vertex1();
			nodes[1] = e.vertex2();
			ca->InsertNextCell(2, &(nodes[0]));
		}
		m_edgesPolyData->SetLines(ca);
		m_edgesActor->VisibilityOn();
	}
}

void PreProcessorBCDataItem::updateActorSettings()
{
	QColor color = m_dialog->color();
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		vtkProperty* prop = m_verticesActor->GetProperty();

		prop->SetColor(color.redF(), color.greenF(), color.blueF());
		prop->SetOpacity(static_cast<double>(m_opacityPercent) / 100);
		prop->SetPointSize(m_dialog->pointSize());
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		vtkProperty* prop = m_cellsActor->GetProperty();

		prop->SetColor(color.redF(), color.greenF(), color.blueF());
		prop->SetOpacity(static_cast<double>(m_opacityPercent) / 100);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		vtkProperty* prop = m_edgesActor->GetProperty();

		prop->SetColor(color.redF(), color.greenF(), color.blueF());
		prop->SetOpacity(static_cast<double>(m_opacityPercent) / 100);
		prop->SetLineWidth(m_dialog->pointSize());
	}
	updateNameActorSettings();
}

void PreProcessorBCDataItem::updateNameActorSettings()
{
	actor2DCollection()->RemoveItem(m_nameActor);
	m_nameActor->VisibilityOff();
	if (m_dialog->showName()) {
		double centerv[3] = {0, 0, 0};
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		if (tmpparent->grid() == 0) {return;}
		vtkPointSet* pset = tmpparent->grid()->vtkGrid();
		vtkPoints* points = pset->GetPoints();
		int pnum = 0;
		if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			for (auto it = m_indices.begin(); it != m_indices.end(); ++it) {
				double tmpv[3];
				points->GetPoint(*it, tmpv);
				for (int i = 0; i < 3; ++i) {
					centerv[i] += tmpv[i];
				}
				++ pnum;
			}
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			for (auto it = m_indices.begin(); it != m_indices.end(); ++it) {
				vtkCell* cell = pset->GetCell(*it);
				for (int i = 0; i < cell->GetNumberOfPoints(); ++i) {
					double tmpv[3];
					points->GetPoint(cell->GetPointId(i), tmpv);
					for (int i = 0; i < 3; ++i) {
						centerv[i] += tmpv[i];
					}
					++ pnum;
				}
			}
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			for (auto it = m_edges.begin(); it != m_edges.end(); ++it) {
				double tmpv[3];
				points->GetPoint(it->vertex1(), tmpv);
				for (int i = 0; i < 3; ++i) {
					centerv[i] += tmpv[i];
				}
				points->GetPoint(it->vertex2(), tmpv);
				for (int i = 0; i < 3; ++i) {
					centerv[i] += tmpv[i];
				}
				pnum += 2;
			}
		}
		if (pnum == 0) {
			m_nameActor->VisibilityOff();
		} else {
			for (int i = 0; i < 3; ++i) {
				centerv[i] /= pnum;
			}
			actor2DCollection()->AddItem(m_nameActor);
			vtkCoordinate* coord = m_nameActor->GetPositionCoordinate();
			coord->SetValue(centerv[0], centerv[1], centerv[2]);
			m_nameMapper->SetInput(iRIC::toStr(m_dialog->caption()).c_str());
			vtkTextProperty* tprop = m_nameMapper->GetTextProperty();
			QColor color = m_dialog->color();
			tprop->SetColor(color.redF(), color.greenF(), color.blueF());
			m_nameMapper->Modified();
		}
	}
	updateVisibilityWithoutRendering();
}


void PreProcessorBCDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		m_verticesActor->SetPosition(0, 0, range.max());
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		m_cellsActor->SetPosition(0, 0, range.max());
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		m_edgesActor->SetPosition(0, 0, range.max());
	}
}

void PreProcessorBCDataItem::assignSelectedElements()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		const QVector<vtkIdType>& selVertices = tmpparent->selectedVertices();
		for (int i = 0; i < selVertices.count(); ++i) {
			m_indices.insert(selVertices[i]);
		}
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		const QVector<vtkIdType>& selCells = tmpparent->selectedCells();
		for (int i = 0; i < selCells.count(); ++i) {
			m_indices.insert(selCells[i]);
		}
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		const QVector<Edge>& selEdges = tmpparent->selectedEdges();
		for (int i = 0; i < selEdges.count(); ++i) {
			m_edges.insert(selEdges[i]);
		}
	}
	updateElements();
	updateNameActorSettings();
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gItem->grid()->setModified();
	renderGraphicsView();
	m_isCustomModified = true;
}

void PreProcessorBCDataItem::releaseSelectedElements()
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		const QVector<vtkIdType>& selVertices = tmpparent->selectedVertices();
		for (int i = 0; i < selVertices.count(); ++i) {
			m_indices.remove(selVertices[i]);
		}
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		const QVector<vtkIdType>& selCells = tmpparent->selectedCells();
		for (int i = 0; i < selCells.count(); ++i) {
			m_indices.remove(selCells[i]);
		}
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		const QVector<Edge>& selEdges = tmpparent->selectedEdges();
		for (int i = 0; i < selEdges.count(); ++i) {
			m_edges.remove(selEdges[i]);
		}
	}
	updateElements();
	updateNameActorSettings();
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gItem->grid()->setModified();
	renderGraphicsView();
	m_isCustomModified = true;
}

void PreProcessorBCDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMouseMoveEvent(event, v);
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMouseMoveEvent(event, v);
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->edgeSelectingMouseMoveEvent(event, v);
		}
	}
	/*
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
		if (tmpparent->grid()->isMasked()){
			v->setCursor(Qt::ForbiddenCursor);
		} else {
			v->setCursor(Qt::ArrowCursor);
		}
	*/
}

void PreProcessorBCDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (event->button() == Qt::LeftButton) {
		// start drawing the mouse bounding box.
		m_definingBoundingBox = true;
		if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->nodeSelectingMousePressEvent(event, v);
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->cellSelectingMousePressEvent(event, v);
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->edgeSelectingMousePressEvent(event, v);
		}
	}
}

void PreProcessorBCDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			bool selected = false;
			if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
				tmpparent->nodeSelectingMouseReleaseEvent(event, v);
				selected = (tmpparent->selectedVertices().count() > 0);
			} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
				tmpparent->cellSelectingMouseReleaseEvent(event, v);
				selected = (tmpparent->selectedCells().count() > 0);
			} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
				tmpparent->edgeSelectingMouseReleaseEvent(event, v);
				selected = (tmpparent->selectedEdges().count() > 0);
			}
			m_assignAction->setEnabled(selected);
			m_releaseAction->setEnabled(selected);
		}
		m_definingBoundingBox = false;
		v->setCursor(tmpparent->normalCursor());
	} else if (event->button() == Qt::RightButton) {
		if (menu != nullptr) {delete menu;}
		menu = new QMenu(projectData()->mainWindow());

		menu->addAction(m_assignAction);
		menu->addAction(m_releaseAction);
		menu->addSeparator();
		menu->addAction(m_editAction);

		menu->move(event->globalPos());
		menu->show();
	}
}

void PreProcessorBCDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		gitem->nodeSelectingKeyPressEvent(event, v);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		gitem->cellSelectingKeyPressEvent(event, v);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		gitem->edgeSelectingKeyPressEvent(event, v);
	}
}

void PreProcessorBCDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		gitem->nodeSelectingKeyReleaseEvent(event, v);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		gitem->cellSelectingKeyReleaseEvent(event, v);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		gitem->cellSelectingKeyReleaseEvent(event, v);
	}
}


void PreProcessorBCDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editAction);
}

void PreProcessorBCDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(true);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(true);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedEdgesVisibility(true);
	}
}

void PreProcessorBCDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedPointsVisibility(false);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedCellsVisibility(false);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		dynamic_cast<PreProcessorGridDataItem*>(parent()->parent())->setSelectedEdgesVisibility(false);
	}
}

void PreProcessorBCDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
	setName(m_standardItem->data(Qt::EditRole).toString());
	emit itemUpdated();
}

void PreProcessorBCDataItem::loadFromCgnsFile(const int fn)
{
	if (m_cgnsNumber == 0) {
		// There is no need to load from CGNS file.
		return;
	}
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Structured2DGrid* sgrid = dynamic_cast<Structured2DGrid*>(gitem->grid());
	cgsize_t size;
	int err = cg_iRIC_Read_BC_IndicesSize(const_cast<char*>(m_condition->name().c_str()), m_cgnsNumber, &size);
	if (err != 0) {return;}
	if (sgrid == nullptr) {
		// this is an unstructured grid.
		std::vector<cgsize_t> indices (size, 0);
		cg_iRIC_Read_BC_Indices(const_cast<char*>(m_condition->name().c_str()), m_cgnsNumber, indices.data());

		if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
				m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			m_indices.clear();
			for (int i = 0; i < size; ++i) {
				m_indices.insert(indices[i] - 1);
			}
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			for (int i = 0; i < size / 2; ++i) {
				Edge e(indices[i * 2] - 1, indices[i * 2 + 1] - 1);
				m_edges.insert(e);
			}
		}
	} else {
		// this is a structured grid.
		std::vector<cgsize_t> indices(size * 2, 0);
		cg_iRIC_Read_BC_Indices(const_cast<char*>(m_condition->name().c_str()), m_cgnsNumber, indices.data());
		m_indices.clear();
		if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			for (int idx = 0; idx < size; ++idx) {
				int i = indices[idx * 2];
				int j = indices[idx * 2 + 1];
				m_indices.insert(sgrid->vertexIndex(i - 1, j - 1));
			}

		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			for (int idx = 0; idx < size; ++idx) {
				int i = indices[idx * 2];
				int j = indices[idx * 2 + 1];
				m_indices.insert(sgrid->cellIndex(i - 1, j - 1));
			}
		} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			for (int idx = 0; idx < size / 2; ++idx) {
				int i1 = indices[idx * 4];
				int j1 = indices[idx * 4 + 1];
				int i2 = indices[idx * 4 + 2];
				int j2 = indices[idx * 4 + 3];
				Edge e(sgrid->vertexIndex(i1 - 1, j1 - 1), sgrid->vertexIndex(i2 - 1, j2 - 1));
				m_edges.insert(e);
			}
		}
	}
	if (buildNumber() <= 3507) {
		m_dialog->setNameAndNumber(m_condition->name(), m_cgnsNumber);
		m_dialog->load(fn);
		setName(m_dialog->caption());
		m_opacityPercent = m_dialog->opacityPercent();
	}

	updateElements();
	updateActorSettings();
}

void PreProcessorBCDataItem::saveToCgnsFile(const int fn)
{
	if (! isValid()) {
		return;
	}
	// first, save indices at which BC is defined.
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	Structured2DGrid* sgrid = dynamic_cast<Structured2DGrid*>(gitem->grid());
	QList<unsigned int> tmplist;
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
			m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		for (int index : m_indices) {
			tmplist.append(index);
		}
		qSort(tmplist);
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		QVector<Edge> tmpedges;
		for (Edge e : m_edges) {
			tmpedges.append(e);
		}
		qSort(tmpedges);
		for (Edge e : tmpedges) {
			tmplist.append(e.vertex1());
			tmplist.append(e.vertex2());
		}
	}
	std::vector<cgsize_t> indices;
	if (sgrid == nullptr) {
		// this is an unstructured grid.
		indices.assign(tmplist.count(), 0);
		for (int idx = 0; idx < tmplist.count(); ++idx) {
			int index = tmplist[idx];
			indices[idx] = index + 1;
		}
	} else {
		// this is a structured grid.
		indices.assign(tmplist.count() * 2, 0);
		for (int idx = 0; idx < tmplist.count(); ++idx) {
			int index = tmplist[idx];
			unsigned int i, j;
			if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
					m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
				sgrid->getIJIndex(index, &i, &j);
			} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
				sgrid->getCellIJIndex(index, &i, &j);
			}
			indices[idx * 2] = i + 1;
			indices[idx * 2 + 1] = j + 1;
		}
	}
	gitem->grid();
	cg_iRIC_Write_BC_Indices(const_cast<char*>(m_condition->name().c_str()), m_cgnsNumber, tmplist.count(), indices.data());

	m_dialog->setNameAndNumber(m_condition->name(), m_cgnsNumber);
	m_dialog->save(fn);
}

void PreProcessorBCDataItem::setName(const QString& name)
{
	m_standardItem->setText(name);
	m_dialog->setCaption(name);
}

void PreProcessorBCDataItem::setProjectNumber(int num)
{
	m_projectNumber = num;
}

void PreProcessorBCDataItem::setCgnsNumber(int num)
{
	m_cgnsNumber = num;
}

void PreProcessorBCDataItem::setColor(const QColor& color)
{
	m_dialog->setColor(color);
}

QColor PreProcessorBCDataItem::color() const
{
	return m_dialog->color();
}

int PreProcessorBCDataItem::opacityPercent() const
{
	return m_dialog->opacityPercent();
}

bool PreProcessorBCDataItem::showDialog()
{
	// set default folder for filename input conditions.
	InputConditionWidgetFilename::defaultFolder = LastIODirectory::get();
	// show dialog
	m_dialog->setOpacityPercent(m_opacityPercent);
	m_dialog->setEditMode();
	int ret = m_dialog->exec();
	if (ret == QDialog::Rejected) {return false;}

	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (gItem->grid() != nullptr) {
		gItem->grid()->setModified();
	}

	setName(m_dialog->caption());
	m_opacityPercent = m_dialog->opacityPercent();
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
	m_indices.clear();
	m_edges.clear();
	updateElements();
	updateNameActorSettings();
}

void PreProcessorBCDataItem::setGrid(Grid* grid)
{
	if (grid == nullptr) {return;}
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		m_cellsGrid->SetInputData(grid->vtkGrid());
	}
}

void PreProcessorBCDataItem::assignIndices(const QSet<vtkIdType>& vertices)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	if (gitem->grid() == nullptr) {return;}
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		m_indices = vertices;
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		m_indices.clear();
		QVector<vtkIdType> cells = gitem->getCellsFromVertices(vertices);
		for (int i = 0; i < cells.count(); ++i) {
			m_indices.insert(cells.at(i));
		}
	} else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		m_edges.clear();
		const QVector<Edge> edges = gitem->getEdgesFromVertices(vertices);
		for (int i = 0; i < edges.count(); ++i) {
			m_edges.insert(edges.at(i));
		}
	}
	updateElements();
	updateNameActorSettings();
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent()->parent());
	gItem->grid()->setModified();
	m_isCustomModified = false;
}

bool PreProcessorBCDataItem::isValid() const
{
	bool valid = true;
	if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
			m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		valid = m_indices.count() > 0;
	} else {
		valid = m_edges.count() > 0;
	}
	return valid;
}


int PreProcessorBCDataItem::buildNumber() const
{
	PreProcessorBCGroupDataItem* gItem = dynamic_cast<PreProcessorBCGroupDataItem*>(parent());
	return gItem->m_projectBuildNumber;
}

QString PreProcessorBCDataItem::caption() const
{
	return m_dialog->caption();
}

std::string PreProcessorBCDataItem::uniqueName() const
{
	QString name("%1_%2");
	QString ret = name.arg(m_condition->name().c_str()).arg(m_projectNumber);
	return iRIC::toStr(ret);
}

void PreProcessorBCDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	this->updateActorSettings();
}
