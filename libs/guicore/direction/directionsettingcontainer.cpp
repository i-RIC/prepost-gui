#include "directionsettingcontainer.h"

#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>

#include <QSettings>

const int DirectionSettingContainer::DEFAULT_LINEWIDTH = 1;
const int DirectionSettingContainer::DEFAULT_ARROWSIZE = 8;

DirectionSettingContainer::DirectionSettingContainer() :
	CompositeContainer({&mode, &color, &opacity, &lineWidth, &arrowSize}),
	mode {"mode", Mode::Mode1to8},
	color {"color", Qt::gray},
	lineWidth {"lineWidth", DEFAULT_LINEWIDTH},
	arrowSize {"arrowSize", DEFAULT_ARROWSIZE}
{
	QSettings settings;

	arrowSize = settings.value("graphics/arrow_size", DEFAULT_ARROWSIZE).toInt();
}

DirectionSettingContainer::DirectionSettingContainer(const DirectionSettingContainer& c) :
	DirectionSettingContainer {}
{
	copyValue(c);
}

DirectionSettingContainer& DirectionSettingContainer::operator=(const DirectionSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& DirectionSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const DirectionSettingContainer&> (c));
}

vtkPolyData* DirectionSettingContainer::buildDirectionPolygonData(vtkPointSet* data, SolverDefinitionGridAttribute* att, VTKGraphicsView* view) const
{
	auto linesPolyData = vtkSmartPointer<vtkPolyData>::New();
	auto linesPoints = vtkSmartPointer<vtkPoints>::New();
	linesPoints->SetDataTypeToDouble();
	linesPolyData->SetPoints(linesPoints);
	int dims[3];
	auto grid = vtkStructuredGrid::SafeDownCast(data);
	if (grid == nullptr || dims[0] < 2 || dims[1] < 2) {
		return vtkPolyData::New();
	}

	grid->GetDimensions(dims);
	for (vtkIdType cellId = 0; cellId < data->GetNumberOfCells(); ++cellId) {
		auto cell = data->GetCell(cellId);
		auto center = vtkPointsUtil::getCenter(cell);
		linesPoints->InsertNextPoint(center.x(), center.y(), 0);
	}

	auto glyphsPolyData = vtkSmartPointer<vtkPolyData>::New();
	auto glyphsPoints = vtkSmartPointer<vtkPoints>::New();

	auto dir = vtkDoubleArray::New();
	dir->SetName("direction");
	dir->SetNumberOfComponents(3);
	dir->Allocate(data->GetNumberOfCells());

	auto activeData = data->GetCellData()->GetScalars();
	double d[3];

	int iDiff, jDiff;
	vtkIdType line[2];
	double p1[3], p2[3], pCenter[3];

	auto lines = vtkSmartPointer<vtkCellArray>::New();
	for (vtkIdType idx = 0; idx < activeData->GetNumberOfTuples(); ++idx) {
		vtkIdType i = idx % (dims[0] - 1);
		vtkIdType j = idx / (dims[0] - 1);

		auto v = static_cast<int> (activeData->GetTuple1(idx));
		getDiff(v, &iDiff, &jDiff);
		if (iDiff == 0 && jDiff == 0) {continue;}
		if (iDiff == -1 && i == 0) {continue;}
		if (iDiff == 1 && i == dims[0] - 2) {continue;}
		if (jDiff == -1 && j == 0) {continue;}
		if (jDiff == 1 && j == dims[1] - 2) {continue;}

		line[0] = idx;
		line[1] = (i + iDiff) + (j + jDiff) * (dims[0] - 1);
		lines->InsertNextCell(2, line);

		linesPoints->GetPoint(line[0], p1);
		linesPoints->GetPoint(line[1], p2);
		for (int k = 0; k < 3; ++k) {
			pCenter[k] = (p1[k] + p2[k]) / 2;
		}
		glyphsPoints->InsertNextPoint(pCenter);
		d[0] = iDiff;
		d[1] = jDiff;
		d[2] = 0;
		dir->InsertNextTuple3(d[0], d[1], d[2]);
	}

	linesPolyData->SetLines(lines);

	glyphsPolyData->SetPoints(glyphsPoints);
	glyphsPolyData->GetPointData()->AddArray(dir);
	glyphsPolyData->GetPointData()->SetActiveVectors("direction");
	auto glyph = vtkSmartPointer<vtkGlyph3D>::New();
	glyph->SetInputData(glyphsPolyData);
	glyph->SetScaleModeToDataScalingOff();
	glyph->SetVectorModeToUseVector();

	auto cone = buildConeSource(view);
	glyph->SetSourceConnection(cone->GetOutputPort());
	cone->Delete();

	auto append = vtkSmartPointer<vtkAppendPolyData>::New();
	append->AddInputData(linesPolyData);
	append->AddInputConnection(glyph->GetOutputPort());
	append->Update();
	auto ret = append->GetOutput();
	ret->Register(nullptr);

	return ret;
}

void DirectionSettingContainer::getDiff(int value, int* iDiff, int* jDiff) const
{
	if (mode == Mode::Mode1to8) {
		if (value == 1) {     *iDiff =  1; *jDiff =  1;} // rightup
		else if (value == 2) {*iDiff =  0; *jDiff =  1;} // up
		else if (value == 3) {*iDiff = -1; *jDiff =  1;} // leftup
		else if (value == 4) {*iDiff = -1; *jDiff =  0;} // left
		else if (value == 5) {*iDiff = -1; *jDiff = -1;} // leftdown
		else if (value == 6) {*iDiff =  0; *jDiff = -1;} // down
		else if (value == 7) {*iDiff =  1; *jDiff = -1;} // rightdown
		else if (value == 8) {*iDiff =  1; *jDiff =  0;} // right
		else {*iDiff = 0; *jDiff = 0;} // default
	} else if (mode == Mode::Mode1to128) {
		if (value == 1) {       *iDiff =  1; *jDiff =  0;} // right
		else if (value ==   2) {*iDiff =  1; *jDiff = -1;} // rightdown
		else if (value ==   4) {*iDiff =  0; *jDiff = -1;} // down
		else if (value ==   8) {*iDiff = -1; *jDiff = -1;} // leftdown
		else if (value ==  16) {*iDiff = -1; *jDiff =  0;} // left
		else if (value ==  32) {*iDiff = -1; *jDiff =  1;} // leftup
		else if (value ==  64) {*iDiff =  0; *jDiff =  1;} // up
		else if (value == 128) {*iDiff =  1; *jDiff =  1;} // rightup
		else {*iDiff = 0; *jDiff = 0;} // default
	}
}

vtkConeSource* DirectionSettingContainer::buildConeSource(VTKGraphicsView* view) const
{
	auto source = vtkConeSource::New();
	double height = view->stdDistance(arrowSize.value() * view->devicePixelRatioF());
	source->SetHeight(height);
	source->SetAngle(15);

	return source;
}
