#define _USE_MATH_DEFINES
#include <cmath>

#include "gridcreatingconditiongridcombine.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercellcontainer.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <misc/stringtool.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>

#include <QMenu>
#include <QXmlStreamWriter>
#include <QMouseEvent>

GridCreatingConditionGridCombine::GridCreatingConditionGridCombine(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: GridCreatingCondition(parent, creator)
{
	m_rightClickingMenu = nullptr;
	clear();
	graphicsView()->ResetCameraClippingRange();
}

GridCreatingConditionGridCombine::~GridCreatingConditionGridCombine()
{
	delete m_rightClickingMenu;
}

bool GridCreatingConditionGridCombine::create(QWidget* parent)
{
	showDialog(parent);
	if (! m_isAccepted) {return false;}

	Structured2DGrid* grid = new Structured2DGrid(0);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridRelatedConditions(grid);

	grid->setDimensions(m_iMax, m_jMax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();

	for (int j = 0; j < m_jMax; j++) {
		for (int i = 0; i < m_iMax; i++) {
			points->InsertPoint(m_iMax * j + i, x8[i + 1][j + 1], y8[i + 1][j + 1], 0.0);
		}
	}
	grid->vtkGrid()->SetPoints(points);

	// allocate memory for all grid related conditions.
	QList<GridAttributeContainer*>& clist = grid->gridRelatedConditions();
	for (auto it = clist.begin(); it != clist.end(); ++it) {
		(*it)->allocate();
	}
	GridAttributeContainer* c;
	c = grid->gridRelatedCondition("Elevation");
	GridAttributeRealNodeContainer* rnContainer = dynamic_cast<GridAttributeRealNodeContainer*>(c);
	if (rnContainer != nullptr) {
		for (int j = 0; j < m_jMax; j++) {
			for (int i = 0; i < m_iMax; i++) {
				rnContainer->setValue(m_iMax * j + i, z8[i + 1][j + 1]);
			}
		}
		rnContainer->dataArray()->Modified();
		rnContainer->setMapped(true);
	}
	c = grid->gridRelatedCondition("CellCondition");
	GridAttributeIntegerCellContainer* icContainer = dynamic_cast<GridAttributeIntegerCellContainer*>(c);
	if (icContainer != nullptr) {
		for (int j = 0; j < m_jMax - 1; j++) {
			for (int i = 0; i < m_iMax - 1; i++) {
				icContainer->setValue((m_iMax - 1) * j + i, obst[i + 1][j + 1]);
			}
		}
		icContainer->dataArray()->Modified();
		icContainer->setMapped(true);
	}
	grid->setModified();

	for (int i = 0; i < m_iMax + 1; i++) {
		delete[] x8[i];
		delete[] y8[i];
		delete[] z8[i];
	}
	delete[] x8;
	delete[] y8;
	delete[] z8;

	for (int i = 0; i < m_iMax; i++) {
		delete[] obst[i];
	}
	delete[] obst;

	emit gridCreated(grid);
	return true;
}

void GridCreatingConditionGridCombine::setupParameters()
{
	int jx[4];
	int ni4_1, nj4_1;
	int ni4_2, nj4_2;

	GridAttributeRealNodeContainer* rncont;
	GridAttributeIntegerCellContainer* iccont;
	// read mainstream grid
	Structured2DGrid* g1 = dynamic_cast<Structured2DGrid*>(mainstreamGrid);
	ni4_1 = g1->dimensionI();
	nj4_1 = g1->dimensionJ();

	double** x8_1 = new double*[ni4_1 + 1];
	double** y8_1 = new double*[ni4_1 + 1];
	double** z8_1 = new double*[ni4_1 + 1];
	for (int i = 0; i < ni4_1 + 1; i++) {
		x8_1[i] = new double[nj4_1 + 1];
		y8_1[i] = new double[nj4_1 + 1];
		z8_1[i] = new double[nj4_1 + 1];
	}
	rncont = dynamic_cast<GridAttributeRealNodeContainer*>(g1->gridRelatedCondition("Elevation"));
	for (int i = 0; i < ni4_1 * nj4_1; i++) {
		double point[3];
		g1->vtkGrid()->GetPoint(i, point);
		int ii = i % ni4_1 + 1;
		int jj = i / ni4_1 + 1;
		x8_1[ii][jj] = point[0];
		y8_1[ii][jj] = point[1];
		if (rncont != 0) {
			z8_1[ii][jj] = rncont->value(i);
		} else {
			z8_1[ii][jj] = 0;
		}
	}

	// initialize obst4_1
	int** obst4_1 = new int*[ni4_1];
	for (int i = 0; i < ni4_1; i++) {
		obst4_1[i] = new int[nj4_1];
	}
	iccont = dynamic_cast<GridAttributeIntegerCellContainer*>(g1->gridRelatedCondition("CellCondition"));
	for (int i = 0; i < (ni4_1 - 1) * (nj4_1 - 1); i++) {
		int ii = i % (ni4_1 - 1) + 1;
		int jj = i / (ni4_1 - 1) + 1;
		if (iccont != 0) {
			obst4_1[ii][jj] = iccont->value(i);
		} else {
			obst4_1[ii][jj] = 0;
		}
	}

	// read tributary grid
	Structured2DGrid* g2 = dynamic_cast<Structured2DGrid*>(tributaryGrid);
	ni4_2 = g2->dimensionI();
	nj4_2 = g2->dimensionJ();

	double** x8_2 = new double*[ni4_2 + 1];
	double** y8_2 = new double*[ni4_2 + 1];
	double** z8_2 = new double*[ni4_2 + 1];
	for (int i = 0; i < ni4_2 + 1; i++) {
		x8_2[i] = new double[nj4_2 + 1];
		y8_2[i] = new double[nj4_2 + 1];
		z8_2[i] = new double[nj4_2 + 1];
	}
	rncont = dynamic_cast<GridAttributeRealNodeContainer*>(g2->gridRelatedCondition("Elevation"));
	for (int i = 0; i < ni4_2 * nj4_2; i++) {
		double point[3];
		g2->vtkGrid()->GetPoint(i, point);
		int ii = i % ni4_2 + 1;
		int jj = i / ni4_2 + 1;
		x8_2[ii][jj] = point[0];
		y8_2[ii][jj] = point[1];
		z8_2[ii][jj] = rncont->value(i);
	}

	// initialize obst4_2
	int** obst4_2 = new int*[ni4_2];
	for (int i = 0; i < ni4_2; i++) {
		obst4_2[i] = new int[nj4_2];
	}
	iccont = dynamic_cast<GridAttributeIntegerCellContainer*>(g2->gridRelatedCondition("CellCondition"));
	for (int i = 0; i < (ni4_2 - 1) * (nj4_2 - 1); i++) {
		int ii = i % (ni4_2 - 1) + 1;
		int jj = i / (ni4_2 - 1) + 1;
		obst4_2[ii][jj] = iccont->value(i);
	}

	// setup parameters
	jx[0] = j_m1;
	jx[1] = j_m2;
	jx[2] = j_t1;
	jx[3] = j_t2;
	int ni = ni4_1;
	int nj = nj4_1 + nj4_2;
	int jxd = 1;
	if (j_conf >= GridCreatingConditionGridCombineSettingDialog::Right) { jxd = - 1; }
	if (j_conf >= GridCreatingConditionGridCombineSettingDialog::Left) { nj = nj4_1 + ni4_2 - 1; }
	m_iMax = ni;
	m_jMax = nj;
	if (j_conf > GridCreatingConditionGridCombineSettingDialog::Right &&
	    jx[3] != nj - 1){
		return;
	} else if (j_conf == GridCreatingConditionGridCombineSettingDialog::Right &&
	           jx[1] != nj - 1){
		return;
	}

//	int nobst = 1;
	x8 = new double*[m_iMax + 1];
	y8 = new double*[m_iMax + 1];
	z8 = new double*[m_iMax + 1];
	obst = new int*[m_iMax];
	for (int i = 0; i < m_iMax + 1; i++) {
		x8[i] = new double[m_jMax + 1];
		y8[i] = new double[m_jMax + 1];
		z8[i] = new double[m_jMax + 1];
	}
	for (int i = 0; i < m_iMax; i++) {
		obst[i] = new int[m_jMax];
	}
	for (int i = 1; i <= ni4_1; i++) {
		for (int j = j_m1 + 1; j <= j_m2 + 1; j++) {
			x8[i][j] = x8_1[i][j - j_m1];
			y8[i][j] = y8_1[i][j - j_m1];
			z8[i][j] = z8_1[i][j - j_m1];
		}
	}
	for (int i = 1; i <= ni4_1 - 1; i++) {
		for (int j = j_m1 + 1; j <= j_m2; j++) {
			obst[i][j] = obst4_1[i][j - j_m1];
		}
	}

	if (j_conf == GridCreatingConditionGridCombineSettingDialog::Bind) {
		for (int i = 1; i <= ni4_1; i++) {
			for (int j = j_t1 + 1; j <= j_t2 + 1; j++) {
				x8[i][j] = x8_2[i][j - j_t1];
				y8[i][j] = y8_2[i][j - j_t1];
				z8[i][j] = z8_2[i][j - j_t1];
			}
		}
		for (int i = 1; i <= ni4_1 - 1; i++) {
			for (int j = j_t1 + 1; j <= j_t2; j++) {
				obst[i][j] = obst4_2[i][j - j_t1];
			}
		}
		// obstacle cells outside computational domain
		for (int i = 1; i <= i_t1; i++) {
			obst[i][j_t1] = 1;
		}
	} else if (j_conf >= GridCreatingConditionGridCombineSettingDialog::Left) {
		int js1 = (1 + jxd) / 2;
		int js2 = (1 - jxd) / 2;
		int j_nj = 0;
		double sl1_x = 0;
		double sl1_y = 0;
		double sl2_x = 0;
		double sl2_y = 0;
		int j_p = 0;
		if (j_conf == GridCreatingConditionGridCombineSettingDialog::Left) {
			j_nj = 1;
			sl1_x = x8_1[1][nj4_1] - p1_x;
			sl1_y = y8_1[1][nj4_1] - p1_y;
			sl2_x = x8_1[ni4_1][nj4_1] - p2_x;
			sl2_y = y8_1[ni4_1][nj4_1] - p2_y;
			j_p = nj;
		} else if (j_conf == GridCreatingConditionGridCombineSettingDialog::Right) {
			j_nj = nj4_2;
			sl1_x = x8_1[1][1] - p1_x;
			sl1_y = y8_1[1][1] - p1_y;
			sl2_x = x8_1[ni4_1][1] - p2_x;
			sl2_y = y8_1[ni4_1][1] - p2_y;
			j_p = 1;
		}

		for (int i = i_t1 + 1; i <= i_t2 + 1; i++) {
			for (int j = j_t1 + 1; j != j_t2 + 1 + jxd; j += jxd) {
				x8[i][j] = x8_2[ni4_2 - jxd * (j - j_t1) + jxd][j_nj + jxd * (i - i_t1) - jxd];
				y8[i][j] = y8_2[ni4_2 - jxd * (j - j_t1) + jxd][j_nj + jxd * (i - i_t1) - jxd];
				z8[i][j] = z8_2[ni4_2 - jxd * (j - j_t1) + jxd][j_nj + jxd * (i - i_t1) - jxd];
			}
		}
		for (int i = i_t1 + 1; i <= i_t2; i++) {
			for (int j = j_t1 + js1; j != j_t2 + js2 + jxd; j += jxd) {
				obst[i][j] = obst4_2[ni4_2 - jxd * (j - j_t1) - js2][j_nj + jxd * (i - i_t1) - jxd - js2];
			}
		}
		// obstacle cells outside computational domain
		x8[1][j_p] = p1_x;
		y8[1][j_p] = p1_y;
		z8[1][j_p] = beta;
		for (int j = j_t2 + 1 - jxd; j != j_t1 + 1; j -= jxd) {
			x8[1][j] = x8[1][j + jxd] + sl1_x / (ni4_2 - 1.) * alpha;
			y8[1][j] = y8[1][j + jxd] + sl1_y / (ni4_2 - 1.) * alpha;
			z8[1][j] = beta;
		}
		for (int j = j_t2 + 1; j != j_t1 + 1; j -= jxd) {
			for (int i = 2; i <= i_t1; i++) {
				x8[i][j] = x8[i - 1][j] + (x8[i_t1 + 1][j] - x8[1][j]) / i_t1 * alpha;
				y8[i][j] = y8[i - 1][j] + (y8[i_t1 + 1][j] - y8[1][j]) / i_t1 * alpha;
				z8[i][j] = beta;
			}
		}

		x8[ni][j_p] = p2_x;
		y8[ni][j_p] = p2_y;
		z8[ni][j_p] = beta;
		for (int j = j_t2 + 1 - jxd; j != j_t1 + 1; j -= jxd) {
			x8[ni][j] = x8[ni][j + jxd] + sl2_x / (ni4_2 - 1.) * alpha;
			y8[ni][j] = y8[ni][j + jxd] + sl2_y / (ni4_2 - 1.) * alpha;
			z8[ni][j] = beta;
		}
		for (int j = j_t2 + 1; j != j_t1 + 1; j -= jxd) {
			for (int i = ni - 1; i >= i_t2 + 2; i--) {
				x8[i][j] = x8[i + 1][j] + (x8[i_t2 + 1][j] - x8[ni][j]) / (ni - 1. - i_t2) * alpha;
				y8[i][j] = y8[i + 1][j] + (y8[i_t2 + 1][j] - y8[ni][j]) / (ni - 1. - i_t2) * alpha;
				z8[i][j] = beta;
			}
		}
		for (int j = j_t1 + js1; j != j_t2 + js2 + jxd; j += jxd) {
			for (int i = ni - 1; i >= i_t2 + 1; i--) {
				obst[i][j] = 1;
			}
			for (int i = 1; i <= i_t1; i++) {
				obst[i][j] = 1;
			}
		}
	}
	for (int i = 0; i < ni4_1 + 1; i++) {
		delete[] x8_1[i];
		delete[] y8_1[i];
		delete[] z8_1[i];
	}
	delete[] x8_1;
	delete[] y8_1;
	delete[] z8_1;

	for (int i = 0; i < ni4_1; i++) {
		delete[] obst4_1[i];
	}
	delete[] obst4_1;

	for (int i = 0; i < ni4_2 + 1; i++) {
		delete[] x8_2[i];
		delete[] y8_2[i];
		delete[] z8_2[i];
	}
	delete[] x8_2;
	delete[] y8_2;
	delete[] z8_2;
	for (int i = 0; i < ni4_2; i++) {
		delete[] obst4_2[i];
	}
	delete[] obst4_2;
}

bool GridCreatingConditionGridCombine::ready() const
{
	return true;
}

void GridCreatingConditionGridCombine::setupMenu()
{
	QAction* dummyAction = m_menu->addAction(tr("(No additional operation available)"));
	dummyAction->setDisabled(true);

	if (m_rightClickingMenu == 0) {
		PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
		m_rightClickingMenu = new QMenu();
		m_rightClickingMenu->addAction(p->createAction());
		m_rightClickingMenu->addSeparator();
		m_menu->addAction(p->switchAction());
	}
}

void GridCreatingConditionGridCombine::showDialog(QWidget* parent)
{
	GridCreatingConditionGridCombineSettingDialog* dialog = new GridCreatingConditionGridCombineSettingDialog(parent);

	dialog->setType(j_conf);
	dialog->setObstacleUpstreamX(p1_x);
	dialog->setObstacleUpstreamY(p1_y);
	dialog->setObstacleDownstreamX(p2_x);
	dialog->setObstacleDownstreamY(p2_y);
	dialog->setCorrectionValue(alpha);
	dialog->setObstacleElevation(beta);
	dialog->setupComboBox(m_conditionDataItem);
	dialog->setMainstreamGridName(mainstreamGridName);
	dialog->setTributaryGridName(tributaryGridName);

	dialog->setupConnections();
	dialog->setupDisability();
	dialog->setupGridIndex();

	dialog->setMainstreamFlowBeginningIndex(i_t1);

	int result = dialog->exec();
	if (result == QDialog::Accepted) {
		m_isAccepted = true;
		handleDialogAccepted(dialog);
	} else {
		m_isAccepted = false;
	}

	delete dialog;
}

void GridCreatingConditionGridCombine::handleDialogAccepted(QDialog* d)
{
	GridCreatingConditionGridCombineSettingDialog* dialog = dynamic_cast<GridCreatingConditionGridCombineSettingDialog*>(d);

	j_conf = dialog->type();
	mainstreamGrid = dialog->mainstreamGrid();
	tributaryGrid = dialog->tributaryGrid();
	mainstreamGridName = dialog->mainstreamGridName();
	tributaryGridName = dialog->tributaryGridName();
	j_m1 = dialog->mainstreamCrossingBeginningIndex();
	j_m2 = dialog->mainstreamCrossingEndingIndex();
	j_t1 = dialog->tributaryCrossingBeginningIndex();
	j_t2 = dialog->tributaryCrossingEndingIndex();
	i_t1 = dialog->mainstreamFlowBeginningIndex();
	i_t2 = dialog->mainstreamFlowEndingIndex();
	p1_x = dialog->obstacleUpstreamX();
	p1_y = dialog->obstacleUpstreamY();
	p2_x = dialog->obstacleDownstreamX();
	p2_y = dialog->obstacleDownstreamY();
	alpha = dialog->correctionValue();
	beta = dialog->obstacleElevation();

	setupParameters();
}

void GridCreatingConditionGridCombine::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode combineNode = iRIC::getChildNode(node, "GridCombine");
	if (! combineNode.isNull()) {
		loadGridCombineFromProjectMainFile(combineNode);
	}
}

void GridCreatingConditionGridCombine::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("GridCombine");
	saveGridCombineToProjectMainFile(writer);
	writer.writeEndElement();
}

void GridCreatingConditionGridCombine::loadGridCombineFromProjectMainFile(const QDomNode& node)
{
	j_conf = (GridCreatingConditionGridCombineSettingDialog::Type) node.toElement().attribute("j_conf", "0").toInt();
	j_m1 = node.toElement().attribute("j_m1", "0").toInt();
	j_m2 = node.toElement().attribute("j_m2", "0").toInt();
	j_t1 = node.toElement().attribute("j_t1", "0").toInt();
	j_t2 = node.toElement().attribute("j_t2", "0").toInt();
	i_t1 = node.toElement().attribute("i_t1", "0").toInt();
	i_t2 = node.toElement().attribute("i_t2", "0").toInt();
	p1_x = node.toElement().attribute("p1_x", "0").toDouble();
	p1_y = node.toElement().attribute("p1_y", "1000").toDouble();
	p2_x = node.toElement().attribute("p2_x", "1000").toDouble();
	p2_y = node.toElement().attribute("p2_y", "1000").toDouble();
	alpha = node.toElement().attribute("alpha", "0.95").toDouble();
	beta = node.toElement().attribute("beta", "999").toDouble();
	mainstreamGridName = node.toElement().attribute("mainstreamGridName", "");
	tributaryGridName = node.toElement().attribute("tributaryGridName", "");

	/*
	   GridCreatingConditionGridCombineSettingDialog* dialog = new GridCreatingConditionGridCombineSettingDialog(parent);
	   dialog->setupComboBox(m_conditionDataItem);
	   mainstreamGrid = dialog->mainstreamGrid();
	   tributaryGrid = dialog->tributaryGrid();
	   delete dialog;
	 */

	m_iMax = node.toElement().attribute("iMax", "0").toInt();
	m_jMax = node.toElement().attribute("jMax", "0").toInt();
}

void GridCreatingConditionGridCombine::saveGridCombineToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("j_conf", qstr.setNum(j_conf));
	writer.writeAttribute("j_m1", qstr.setNum(j_m1));
	writer.writeAttribute("j_m2", qstr.setNum(j_m2));
	writer.writeAttribute("j_t1", qstr.setNum(j_t1));
	writer.writeAttribute("j_t2", qstr.setNum(j_t2));
	writer.writeAttribute("i_t1", qstr.setNum(i_t1));
	writer.writeAttribute("i_t2", qstr.setNum(i_t2));
	writer.writeAttribute("p1_x", qstr.setNum(p1_x));
	writer.writeAttribute("p1_y", qstr.setNum(p1_y));
	writer.writeAttribute("p2_x", qstr.setNum(p2_x));
	writer.writeAttribute("p2_y", qstr.setNum(p2_y));
	writer.writeAttribute("alpha", qstr.setNum(alpha));
	writer.writeAttribute("beta", qstr.setNum(beta));

	writer.writeAttribute("mainstreamGridName", mainstreamGridName);
	writer.writeAttribute("tributaryGridName", tributaryGridName);
	writer.writeAttribute("iMax", qstr.setNum(m_iMax));
	writer.writeAttribute("jMax", qstr.setNum(m_jMax));
}

void GridCreatingConditionGridCombine::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionGridCombine::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionGridCombine::clear()
{
	j_conf = GridCreatingConditionGridCombineSettingDialog::Left;
	j_m1 = 0;
	j_m2 = 0;
	j_t1 = 0;
	j_t2 = 0;
	i_t1 = 0;
	i_t2 = 0;
	p1_x = 0;
	p1_y = 1000;
	p2_x = 1000;
	p2_y = 1000;
	alpha = 0.95;
	beta = 999;

	mainstreamGrid = nullptr;
	tributaryGrid = nullptr;
	m_iMax = 0;
	m_jMax = 0;
}
