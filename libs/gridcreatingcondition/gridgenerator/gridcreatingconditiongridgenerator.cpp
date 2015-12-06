#define _USE_MATH_DEFINES
#include <cmath>

#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>
#include "gridcreatingconditiongridgenerator.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercellcontainer.h>

#include <QMenu>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMainWindow>

GridCreatingConditionGridGenerator::GridCreatingConditionGridGenerator(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: GridCreatingCondition(parent, creator)
{
	m_rightClickingMenu = 0;
	clear();
}

GridCreatingConditionGridGenerator::~GridCreatingConditionGridGenerator()
{
	delete m_rightClickingMenu;
}

bool GridCreatingConditionGridGenerator::create(QWidget* parent)
{
	GridCreatingConditionGridGeneratorSettingDialog* dialog = new GridCreatingConditionGridGeneratorSettingDialog(parent);

	dialog->setChannelShape(j_chn);
	dialog->setFloodChannelProperty(j_fix);
	dialog->setBedShape(j_bar);
	dialog->setAmplitude(amp);
	dialog->setLag(delta);
	dialog->setWaveLength(sl);
	dialog->setWaveNumber(irns);
	dialog->setAngle(phi0);
	dialog->setSlope(slope);
	dialog->setLongitudinalGridNumber(nt);
	dialog->setSimpleChannelWidth(width);
	dialog->setSimpleChannelGridNumber(ny);
	dialog->setSimpleCompoundLeftWidth(wid[5]);
	dialog->setSimpleCompoundRightWidth(wid[1]);
	dialog->setSimpleCompoundLowWidth(wid[3]);
	dialog->setComplexCompoundTotalWidth(width_t);
	dialog->setComplexCompoundLowWidth(width_ct);
	dialog->setLeftGridNumber(ndy[5]);
	dialog->setRightGridNumber(ndy[1]);
	dialog->setLowGridNumber(ndy[3]);
	dialog->setLowDepth(height_l);
	dialog->setLowSlope(alpha_l);
	dialog->setBankGridNumber(ny_m);
	dialog->setWidthChange(j_width);
	dialog->setWidthChangeType(j_width_type);
	dialog->setWidthVariation(delta_b);

	dialog->setupConnections();
	dialog->setupDisability();

	int result = dialog->exec();
	if (result == QDialog::Rejected) {
		return false;
	}
	j_chn = dialog->channelShape();
	j_fix = dialog->floodChannel();
	j_bar = dialog->bedShape();
	amp = dialog->amplitude();
	delta = dialog->lag();
	nt = dialog->longitudinalGridNumber();
	irns = dialog->waveNumber();
	sl = dialog->waveLength();
	phi0 = dialog->angle();
	slope = dialog->slope();
	width = dialog->simpleChannelWidth();
	ny = dialog->simpleChannelGridNumber();
	wid[5] = dialog->simpleCompoundLeftWidth();
	wid[3] = dialog->simpleCompoundLowWidth();
	wid[1] = dialog->simpleCompoundRightWidth();
	ndy[5] = dialog->leftGridNumber();
	ndy[3] = dialog->lowGridNumber();
	ndy[1] = dialog->rightGridNumber();
	height_l = dialog->lowDepth();
	alpha_l = dialog->lowSlope();
	ny_m = dialog->bankGridNumber();
	width_t = dialog->complexCompoundTotalWidth();
	width_ct = dialog->complexCompoundLowWidth();
	j_width = dialog->widthChange();
	j_width_type = dialog->widthChangeType();
	delta_b = dialog->widthVariation();

	delete dialog;
	if (! setupParameters()) {
		// error occured.
		return false;
	}
	Structured2DGrid* grid = new Structured2DGrid(0);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridRelatedConditions(grid);

	grid->setDimensions(m_iMax, m_jMax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();

	for (int j = 0; j < m_jMax; j++) {
		for (int i = 0; i < m_iMax; i++) {
			points->InsertPoint(m_iMax * j + i, xp[i][j], yp[i][j], 0.0);
		}
	}
	grid->vtkGrid()->SetPoints(points);

	// allocate memory for all grid related conditions.
	for (GridAttributeContainer* c : grid->gridAttributes()) {
		c->allocate();
	}
	GridAttributeContainer* c;
	c = grid->gridAttribute("Elevation");
	GridAttributeRealNodeContainer* rnContainer = dynamic_cast<GridAttributeRealNodeContainer*>(c);
	if (rnContainer != 0) {
		for (int j = 0; j < m_jMax; j++) {
			for (int i = 0; i < m_iMax; i++) {
				rnContainer->setValue(m_iMax * j + i, zp[i][j]);
			}
		}
		rnContainer->dataArray()->Modified();
		rnContainer->setMapped(true);
	}
	c = grid->gridAttribute("CellCondition");
	GridAttributeIntegerCellContainer* icContainer = dynamic_cast<GridAttributeIntegerCellContainer*>(c);
	if (icContainer != 0) {
		for (int j = 0; j < m_jMax - 1; j++) {
			for (int i = 0; i < m_iMax - 1; i++) {
				icContainer->setValue((m_iMax - 1) * j + i, obst[i][j]);
			}
		}
		icContainer->dataArray()->Modified();
		icContainer->setMapped(true);
	}
	grid->setModified();
	for (int i = 0; i < m_iMax; i++) {
		delete[] xp[i];
		delete[] yp[i];
		delete[] zp[i];
	}
	delete[] xp;
	delete[] yp;
	delete[] zp;

	for (int i = 0; i < m_iMax - 1; i++) {
		delete[] obst[i];
	}
	delete[] obst;

	emit gridCreated(grid);
	dataModel()->graphicsView()->cameraFit();
	return true;
}

bool GridCreatingConditionGridGenerator::setupParameters()
{
	int jp[6];
	double ddy[6];
	double yyp[6];

	if (j_chn == GridCreatingConditionGridGeneratorSettingDialog::ComplexCompound) {
		wid[3] = width_ct;
	}
	ndy[4] = ny_m;
	ndy[2] = ndy[4];
	wid[2] = alpha_l * height_l;
	wid[4] = wid[2];
	if (height_l < 1e-4) {
		ndy[2] = 0;
		ndy[4] = 0;
	}

	double originalPhi = phi0;
	phi0 = phi0 * M_PI / 180;
	int nx = nt * irns;
//	double chl = sl * irns;

	double dy0 = width / ny;

	double* y0;
	if (j_chn == GridCreatingConditionGridGeneratorSettingDialog::SingleSection) {
		y0 = new double[ny + 1];
		for (int j = 0; j <= ny; j++) {
			y0[j] = - width / 2. + j * dy0;
		}
	} else if (j_chn >= GridCreatingConditionGridGeneratorSettingDialog::SimpleCompound) {
		jp[0] = 0;
		yyp[0] = 0;
		for (int m = 1; m <= 5; m++) {
			yyp[m] = yyp[m - 1] + wid[m];
			jp[m] = jp[m - 1] + ndy[m];
			ddy[m] = wid[m] / ndy[m];
		}
		width = yyp[5];
		ny = jp[5];
		y0 = new double[ny + 1];
		for (int m = 1; m <= 5; m++) {
			for (int j = jp[m - 1] + 1; j <= jp[m]; j++) {
				y0[j] = - width / 2. + yyp[m - 1] + ddy[m] * (j - jp[m - 1]);
			}
		}
		y0[0] = - width / 2.;
	}

	m_iMax = nx + 1;
	m_jMax = ny + 1;

	if (m_iMax * m_jMax > MAXGRIDSIZE) {
		QMessageBox::warning(dataModel()->mainWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return false;
	}
	double* xcenter = new double[m_iMax];
	double* ycenter = new double[m_iMax];
	double* s = new double[m_iMax];
	double* pp = new double[m_iMax];

	double ds = sl / nt;
	double dds = ds / 1000;
	pp[0] = 0.;
	s[0] = 0.;
	xcenter[0] = 0.;
	ycenter[0] = 0.;
	double ss = 0.;
	double xx = 0.;
	double yy = 0.;
	double ymax = - 9999.;
	double ymin = 9999.;
	for (int i = 1; i <= nx; i++) {
		double phi;
		for (int ii = 1; ii <= 1000; ii++) {
			ss = ss + dds;
			phi = phi0 * sin((2 * M_PI / sl) * ss);
			xx = xx + dds * cos(phi);
			yy = yy + dds * sin(phi);
			ymax = std::max(ymax, yy);
			ymin = std::min(ymin, yy);
		}
		xcenter[i] = xx;
		ycenter[i] = yy;
		s[i] = ss;
		pp[i] = phi;
	}
	phi0 = originalPhi;

	double yycc = (ymin + ymax) * .5;
	ymax = yycc + width_t * .5;
	ymin = yycc - width_t * .5;

	xp = new double*[m_iMax];
	yp = new double*[m_iMax];
	zp = new double*[m_iMax];
	for (int i = 0; i < m_iMax; i++) {
		xp[i] = new double[m_jMax];
		yp[i] = new double[m_jMax];
		zp[i] = new double[m_jMax];
	}
	for (int i = 0; i <= nx; i++) {
		double w_l;
		double w_r;
		if (j_chn >= GridCreatingConditionGridGeneratorSettingDialog::SimpleCompound ||
				(j_chn == GridCreatingConditionGridGeneratorSettingDialog::SingleSection && j_width == GridCreatingConditionGridGeneratorSettingDialog::Constant)) {
			w_l = width / 2.;
			w_r = width / 2.;
		} else if (j_width_type == GridCreatingConditionGridGeneratorSettingDialog::Both) {
			w_l = width / 2. + (delta_b / 2.) * (1. - cos(2. * M_PI * s[i] / sl));
			w_r = width / 2. + (delta_b / 2.) * (1. - cos(2. * M_PI * s[i] / sl));
		} else if (j_width_type == GridCreatingConditionGridGeneratorSettingDialog::Left) {
			w_l = width / 2. + (delta_b / 2.) * (1. - cos(2. * M_PI * s[i] / sl));
			w_r = width / 2.;
		} else {
			w_r = width / 2. + (delta_b / 2.) * (1. - cos(2. * M_PI * s[i] / sl));
			w_l = width / 2.;
		}
		double xp_r = xcenter[i] + w_r * sin(pp[i]);
		double yp_r = ycenter[i] - w_r * cos(pp[i]);
		double xp_l = xcenter[i] - w_l * sin(pp[i]);
		double yp_l = ycenter[i] + w_l * cos(pp[i]);
		for (int j = 0; j <= ny; j++) {
			xp[i][j] = xp_r + (xp_l - xp_r) * j / ny;
			yp[i][j] = yp_r + (yp_l - yp_r) * j / ny;
			zp[i][j] = - slope * s[i];
			if (j_chn >= GridCreatingConditionGridGeneratorSettingDialog::SimpleCompound &&
					j >= jp[2] &&
					j <= jp[3]) {
				zp[i][j] = zp[i][j] - height_l;
			}
		}
	}

	// Straight Bank
	if (j_chn == GridCreatingConditionGridGeneratorSettingDialog::ComplexCompound) {
		double dyy;
		for (int i = 0; i <= nx; i++) {
			yy = ymax - yp[i][jp[4]];
			dyy = yy / (jp[5] - jp[4]);
			for (int j = jp[4] + 1; j <= jp[5]; j++) {
				xp[i][j] = xp[i][jp[4]];
				yp[i][j] = yp[i][jp[4]] + dyy * (j - jp[4]);
			}
			yy = yp[i][jp[1]] - ymin;
			dyy = yy / jp[1];
			for (int j = 0; j <= jp[1]; j++) {
				xp[i][j] = xp[i][jp[1]];
				yp[i][j] = ymin + dyy * j;
			}
		}
	}

	// bed shape
	if (j_bar == GridCreatingConditionGridGeneratorSettingDialog::AlternateBar ||
			j_bar == GridCreatingConditionGridGeneratorSettingDialog::Parabolic) {
		for (int i = 0; i <= nx; i++) {
			for (int j = 0; j <= ny; j++) {
				if (j_chn == GridCreatingConditionGridGeneratorSettingDialog::SingleSection) {
					if (j_bar == GridCreatingConditionGridGeneratorSettingDialog::AlternateBar) {
						zp[i][j] = zp[i][j] - amp * cos(2. * M_PI * (s[i] - delta) / sl) * cos(M_PI * (y0[j] - y0[0]) / (2. * y0[ny]));
					} else {
						zp[i][j] = zp[i][j] + 4. * amp / pow(width, 2) * pow(y0[j] - y0[0] - width / 2., 2) - amp;
					}
				} else {
					if (j >= jp[2] && j <= jp[3]) {
						if (j_bar == GridCreatingConditionGridGeneratorSettingDialog::AlternateBar) {
							zp[i][j] = zp[i][j] - amp * cos(2. * M_PI * (s[i] - delta) / sl) * cos(M_PI * (y0[j] - y0[jp[2]]) / wid[3]);
						} else {
							zp[i][j] = zp[i][j] + 4. * amp / pow(wid[3], 2) * pow(y0[j] - y0[jp[2]], 2) - amp;
						}
					}
				}
			}
		}
	}

	delete[] xcenter;
	delete[] ycenter;
	delete[] s;
	delete[] pp;
	delete[] y0;

	obst = new int*[m_iMax - 1];
	for (int i = 0; i < m_iMax - 1; i++) {
		obst[i] = new int[m_jMax - 1];
	}
	for (int i = 0; i < m_iMax - 1; i++) {
		for (int j = 0; j < m_jMax - 1; j++) {
			obst[i][j] = 0;
		}
	}

	int is[2][2];
	int js[2][2];
	if (j_fix == GridCreatingConditionGridGeneratorSettingDialog::Fixed &&
			j_chn >= GridCreatingConditionGridGeneratorSettingDialog::SimpleCompound) {
		is[0][0] = 0;
		is[1][0] = nx;
		js[0][0] = 0;
		js[1][0] = jp[1];
		is[0][1] = 0;
		is[1][1] = nx;
		js[0][1] = jp[4];
		js[1][1] = ny;
		for (int n = 0; n <= 1; n++) {
			for (int i = is[0][n]; i < is[1][n]; i++) {
				for (int j = js[0][n]; j < js[1][n]; j++) {
					obst[i][j] = 1;
				}
			}
		}
	}
	return true;
}

bool GridCreatingConditionGridGenerator::ready() const
{
	return true;
}

void GridCreatingConditionGridGenerator::setupMenu()
{
	if (m_rightClickingMenu == nullptr) {
		PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
		m_rightClickingMenu = new QMenu();
		m_rightClickingMenu->addAction(p->createAction());
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(p->clearAction());
	}
}

void GridCreatingConditionGridGenerator::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode generatorNode = iRIC::getChildNode(node, "GridGenerator");
	if (! generatorNode.isNull()) {
		loadGridGeneratorFromProjectMainFile(generatorNode);
	}
}

void GridCreatingConditionGridGenerator::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("GridGenerator");
	saveGridGeneratorToProjectMainFile(writer);
	writer.writeEndElement();
}

void GridCreatingConditionGridGenerator::loadGridGeneratorFromProjectMainFile(const QDomNode& node)
{
	j_chn = (GridCreatingConditionGridGeneratorSettingDialog::ChannelShape) node.toElement().attribute("j_chn", "0").toInt();
	j_fix = (GridCreatingConditionGridGeneratorSettingDialog::FloodChannel) node.toElement().attribute("j_fix", "0").toInt();
	j_bar = (GridCreatingConditionGridGeneratorSettingDialog::BedShape) node.toElement().attribute("j_bar", "0").toInt();
	amp = node.toElement().attribute("amp", "0.01").toDouble();
	delta = node.toElement().attribute("delta", "0.3").toDouble();
	nt = node.toElement().attribute("nt", "20").toInt();
	irns = node.toElement().attribute("irns", "1").toInt();
	sl = node.toElement().attribute("sl", "500").toDouble();
	phi0 = node.toElement().attribute("phi0", "0").toDouble();
	slope = node.toElement().attribute("slope", "0.001").toDouble();
	width = node.toElement().attribute("width", "20").toDouble();
	ny = node.toElement().attribute("ny", "10").toInt();
	wid[5] = node.toElement().attribute("width_l", "10").toDouble();
	wid[3] = node.toElement().attribute("width_c", "20").toDouble();
	wid[1] = node.toElement().attribute("width_r", "10").toDouble();
	ndy[5] = node.toElement().attribute("ny_l", "4").toInt();
	ndy[3] = node.toElement().attribute("ny_c", "4").toInt();
	ndy[1] = node.toElement().attribute("ny_r", "4").toInt();
	height_l = node.toElement().attribute("height_l", "1").toDouble();
	alpha_l = node.toElement().attribute("alpha_l", "2").toDouble();
	ny_m = node.toElement().attribute("ny_m", "1").toInt();
	width_t = node.toElement().attribute("width_t", "50").toDouble();
	width_ct = node.toElement().attribute("width_ct", "20").toDouble();
	j_width = (GridCreatingConditionGridGeneratorSettingDialog::WidthChange) node.toElement().attribute("j_width", "0").toInt();
	j_width_type = (GridCreatingConditionGridGeneratorSettingDialog::WidthChangeType) node.toElement().attribute("j_width_type", "0").toInt();
	delta_b = node.toElement().attribute("delta_b", "10").toDouble();

	m_iMax = node.toElement().attribute("m_iMax", "0").toInt();
	m_jMax = node.toElement().attribute("m_jMax", "0").toInt();
}

void GridCreatingConditionGridGenerator::saveGridGeneratorToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("j_chn", qstr.setNum(j_chn));
	writer.writeAttribute("j_fix", qstr.setNum(j_fix));
	writer.writeAttribute("j_bar", qstr.setNum(j_bar));
	writer.writeAttribute("amp", qstr.setNum(amp));
	writer.writeAttribute("delta", qstr.setNum(delta));
	writer.writeAttribute("nt", qstr.setNum(nt));
	writer.writeAttribute("irns", qstr.setNum(irns));
	writer.writeAttribute("sl", qstr.setNum(sl));
	writer.writeAttribute("phi0", qstr.setNum(phi0));
	writer.writeAttribute("slope", qstr.setNum(slope));
	writer.writeAttribute("width", qstr.setNum(width));
	writer.writeAttribute("ny", qstr.setNum(ny));
	writer.writeAttribute("width_l", qstr.setNum(wid[5]));
	writer.writeAttribute("width_c", qstr.setNum(wid[3]));
	writer.writeAttribute("width_r", qstr.setNum(wid[1]));
	writer.writeAttribute("ny_l", qstr.setNum(ndy[5]));
	writer.writeAttribute("ny_c", qstr.setNum(ndy[3]));
	writer.writeAttribute("ny_r", qstr.setNum(ndy[1]));
	writer.writeAttribute("height_l", qstr.setNum(height_l));
	writer.writeAttribute("alpha_l", qstr.setNum(alpha_l));
	writer.writeAttribute("ny_m", qstr.setNum(ny_m));
	writer.writeAttribute("width_t", qstr.setNum(width_t));
	writer.writeAttribute("width_ct", qstr.setNum(width_ct));
	writer.writeAttribute("j_width", qstr.setNum(j_width));
	writer.writeAttribute("j_width_type", qstr.setNum(j_width_type));
	writer.writeAttribute("delta_b", qstr.setNum(delta_b));

	writer.writeAttribute("m_iMax", qstr.setNum(m_iMax));
	writer.writeAttribute("m_jMax", qstr.setNum(m_jMax));
}

void GridCreatingConditionGridGenerator::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionGridGenerator::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionGridGenerator::showInitialDialog()
{
	create(preProcessorWindow());
}

void GridCreatingConditionGridGenerator::clear()
{
	j_chn = GridCreatingConditionGridGeneratorSettingDialog::SingleSection;
	j_fix = GridCreatingConditionGridGeneratorSettingDialog::Fixed;
	j_bar = GridCreatingConditionGridGeneratorSettingDialog::Flat;
	amp = 0.01;
	delta = 0.3;
	nt = 20;
	irns = 1;
	sl = 500;
	phi0 = 0;
	slope = 0.001;
	width = 20;
	ny = 10;
	wid[5] = 10;
	wid[3] = 20;
	wid[1] = 10;
	ndy[5] = 4;
	ndy[3] = 4;
	ndy[1] = 4;
	height_l = 1;
	alpha_l = 2;
	ny_m = 1;
	width_t = 50;
	width_ct = 20;
	j_width = GridCreatingConditionGridGeneratorSettingDialog::Constant;
	j_width_type = GridCreatingConditionGridGeneratorSettingDialog::Both;
	delta_b = 10;

	m_iMax = 0;
	m_jMax = 0;
}
