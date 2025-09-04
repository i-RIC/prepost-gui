#include "directionsettingcontainer.h"

#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>

#include <QRandomGenerator64>
#include <QSettings>

#include <map>

namespace {
	const int FIND_DOWNSTREAM_POINT_STARTPOINTCOUNT = 10;
}

const int DirectionSettingContainer::DEFAULT_LINEWIDTH = 1;
const int DirectionSettingContainer::DEFAULT_ARROWSIZE = 8;

DirectionSettingContainer::DirectionSettingContainer() :
	CompositeContainer({&mode, &color, &wrongColor, &opacity, &lineWidth, &arrowSize}),
	mode {"mode", Mode::Mode1to128},
	color {"color", Qt::blue},
	wrongColor {"wrongColor", Qt::red},
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

std::vector<vtkIdType> DirectionSettingContainer::findWrongPoints(vtkPointSet* data)
{
	vtkIdType p = findDownstreamPoint(data);
	return findWrongPoints(data, p);
}

std::vector<vtkIdType> DirectionSettingContainer::findWrongPoints(vtkPointSet* data, vtkIdType downstreamPoint)
{
	int dims[3];
	auto grid = vtkStructuredGrid::SafeDownCast(data);
	grid->GetDimensions(dims);
	vtkIdType cellINum = dims[0] - 1;
	vtkIdType cellJNum = dims[1] - 1;

	std::vector<bool> wrong;
	wrong.assign(data->GetNumberOfCells(), true);

	auto vals = vtkIntArray::SafeDownCast(data->GetCellData()->GetScalars());
	int iDiff, jDiff;
	for (vtkIdType i = 0; i < data->GetNumberOfCells(); ++i) {
		auto value = vals->GetValue(i);
		getDiff(value, &iDiff, &jDiff);
		if (iDiff == 0 && jDiff == 0) {
			wrong[i] = false;
		}
	}
	std::unordered_set<vtkIdType> visited;

	findWrongPointsRecursively(vals, cellINum, cellJNum, downstreamPoint, visited, &wrong);

	std::vector<vtkIdType> ret;
	for (vtkIdType i = 0; i < wrong.size(); ++i) {
		if (wrong[i]) {
			ret.push_back(i);
		}
	}
	return ret;
}

vtkIdType DirectionSettingContainer::findDownstreamPoint(vtkPointSet* data)
{
	int dims[3];
	std::map<vtkIdType, unsigned char> points;
	auto grid = vtkStructuredGrid::SafeDownCast(data);
	grid->GetDimensions(dims);
	int cellINum = dims[0] - 1;
	int cellJNum = dims[1] - 1;

	auto numCells = data->GetNumberOfCells();
	int testedCount = 0;
	auto vals = vtkIntArray::SafeDownCast(data->GetCellData()->GetScalars());
	while (testedCount < FIND_DOWNSTREAM_POINT_STARTPOINTCOUNT) {
		std::unordered_set<vtkIdType> visited;
		auto index = QRandomGenerator64::global()->generate64() % numCells;
		auto value = vals->GetValue(index);
		int iDiff, jDiff;
		getDiff(value, &iDiff, &jDiff);
		if (iDiff == 0 && jDiff == 0) {
			continue;
		}

		while ((! (iDiff == 0 && jDiff == 0)) && (visited.find(index) == visited.end())) {
			visited.insert(index);

			int i = index % cellINum;
			int j = (index - i) / cellINum;
			int new_i = i + iDiff;
			int new_j = j + jDiff;
			if (new_i < 0 || new_i >= cellINum || new_j < 0 || new_j >= cellJNum) {
				break;
			}
			index = new_i + new_j * cellINum;

			value = vals->GetValue(index);
			getDiff(value, &iDiff, &jDiff);
		}
		auto it = points.find(index);
		if (it == points.end()) {
			auto pair = points.insert({index, 0});
			it = pair.first;
		}
		++ it->second;
		++ testedCount;
	}
	vtkIdType p = 0;
	unsigned int maxCount = 0;

	for (auto pair : points) {
		if (pair.second > maxCount) {
			p = pair.first;
			maxCount = pair.second;
		}
	}
	return p;
}

void DirectionSettingContainer::buildDirectionPolygonData(vtkPointSet* data, SolverDefinitionGridAttribute* att, VTKGraphicsView* view, const std::vector<vtkIdType>& wrongCells, vtkActor* actor, vtkActor* wrongActor) const
{
	std::unordered_set<vtkIdType> wrongCellsSet;
	for (auto id : wrongCells) {
		wrongCellsSet.insert(id);
	}

	auto linesPoints = vtkSmartPointer<vtkPoints>::New();
	linesPoints->SetDataTypeToDouble();
	int dims[3];
	auto grid = vtkStructuredGrid::SafeDownCast(data);
	bool returnEmpty = false;

	if (grid == nullptr) {
		returnEmpty = true;
	} else {
		grid->GetDimensions(dims);
		if (dims[0] < 2 || dims[1] < 2) {
			returnEmpty = true;
		}
	}

	if (returnEmpty) {
		auto empty = vtkSmartPointer<vtkPolyData>::New();
		auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

		mapper->SetInputData(empty);
		actor->SetMapper(mapper);
		wrongActor->SetMapper(mapper);
		return;
	}

	for (vtkIdType cellId = 0; cellId < data->GetNumberOfCells(); ++cellId) {
		auto cell = data->GetCell(cellId);
		auto center = vtkPointsUtil::getCenter(cell);
		linesPoints->InsertNextPoint(center.x(), center.y(), 0);
	}

	auto glyphsPoints = vtkSmartPointer<vtkPoints>::New();
	auto wrongGlyphsPoints = vtkSmartPointer<vtkPoints>::New();

	auto dir = vtkDoubleArray::New();
	dir->SetName("direction");
	dir->SetNumberOfComponents(3);
	dir->Allocate(data->GetNumberOfCells());

	auto wrongDir = vtkDoubleArray::New();
	wrongDir->SetName("direction");
	wrongDir->SetNumberOfComponents(3);
	wrongDir->Allocate(data->GetNumberOfCells());

	auto origCellIds = vtkIntArray::SafeDownCast(data->GetCellData()->GetArray(v4Grid2d::ORIGINAL_ID));
	auto activeData = data->GetCellData()->GetScalars();
	double d[3];

	int iDiff, jDiff;
	vtkIdType line[2];
	double p1[3], p2[3], pCenter[3];

	auto lines = vtkSmartPointer<vtkCellArray>::New();
	auto wrongLines = vtkSmartPointer<vtkCellArray>::New();

	for (vtkIdType idx = 0; idx < activeData->GetNumberOfTuples(); ++idx) {
		auto origCellId = origCellIds->GetValue(idx);

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

		bool wrong = (wrongCellsSet.find(origCellId) != wrongCellsSet.end());
		if (wrong) {
			wrongLines->InsertNextCell(2, line);
		} else {
			lines->InsertNextCell(2, line);
		}
		linesPoints->GetPoint(line[0], p1);
		linesPoints->GetPoint(line[1], p2);
		for (int k = 0; k < 3; ++k) {
			pCenter[k] = (p1[k] + p2[k]) / 2;
		}
		d[0] = iDiff;
		d[1] = jDiff;
		d[2] = 0;
		if (wrong) {
			wrongGlyphsPoints->InsertNextPoint(pCenter);
			wrongDir->InsertNextTuple3(d[0], d[1], d[2]);
		} else {
			glyphsPoints->InsertNextPoint(pCenter);
			dir->InsertNextTuple3(d[0], d[1], d[2]);
		}
	}

	auto linesPolyData = vtkSmartPointer<vtkPolyData>::New();
	linesPolyData->SetPoints(linesPoints);
	linesPolyData->SetLines(lines);

	auto wrongLinesPolyData = vtkSmartPointer<vtkPolyData>::New();
	wrongLinesPolyData->SetPoints(linesPoints);
	wrongLinesPolyData->SetLines(wrongLines);

	auto glyphsPolyData = vtkSmartPointer<vtkPolyData>::New();
	glyphsPolyData->SetPoints(glyphsPoints);
	glyphsPolyData->GetPointData()->AddArray(dir);
	glyphsPolyData->GetPointData()->SetActiveVectors("direction");

	auto wrongGlyphsPolyData = vtkSmartPointer<vtkPolyData>::New();
	wrongGlyphsPolyData->SetPoints(wrongGlyphsPoints);
	wrongGlyphsPolyData->GetPointData()->AddArray(wrongDir);
	wrongGlyphsPolyData->GetPointData()->SetActiveVectors("direction");

	auto glyph = vtkSmartPointer<vtkGlyph3D>::New();
	glyph->SetInputData(glyphsPolyData);
	glyph->SetScaleModeToDataScalingOff();
	glyph->SetVectorModeToUseVector();
	auto cone = buildConeSource(view);
	glyph->SetSourceConnection(cone->GetOutputPort());
	cone->Delete();

	auto wrongGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	wrongGlyph->SetInputData(wrongGlyphsPolyData);
	wrongGlyph->SetScaleModeToDataScalingOff();
	wrongGlyph->SetVectorModeToUseVector();
	auto wrongCone = buildConeSource(view);
	wrongGlyph->SetSourceConnection(wrongCone->GetOutputPort());
	wrongCone->Delete();

	auto append = vtkSmartPointer<vtkAppendPolyData>::New();
	append->AddInputData(linesPolyData);
	append->AddInputConnection(glyph->GetOutputPort());
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->ScalarVisibilityOff();
	mapper->SetInputConnection(append->GetOutputPort());
	actor->SetMapper(mapper);

	auto wrongAppend = vtkSmartPointer<vtkAppendPolyData>::New();
	wrongAppend->AddInputData(wrongLinesPolyData);
	wrongAppend->AddInputConnection(wrongGlyph->GetOutputPort());
	auto wrongMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	wrongMapper->ScalarVisibilityOff();
	wrongMapper->SetInputConnection(wrongAppend->GetOutputPort());
	wrongActor->SetMapper(wrongMapper);

	auto prop = actor->GetProperty();
	prop->SetColor(color);
	prop->SetOpacity(opacity);
	prop->SetLineWidth(lineWidth);

	auto wrongProp = wrongActor->GetProperty();
	wrongProp->SetColor(wrongColor);
	wrongProp->SetOpacity(opacity);
	wrongProp->SetLineWidth(lineWidth);
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

void DirectionSettingContainer::findWrongPointsRecursively(vtkIntArray* data, vtkIdType cellICount, vtkIdType cellJCount, vtkIdType current, std::unordered_set<vtkIdType> visited, std::vector<bool>* wrong)
{
	if (visited.find(current) != visited.end()) {return;}

	visited.insert(current);
	vtkIdType i = current % cellICount;
	vtkIdType j = (current - i) / cellICount;

	for (int iDiff = -1; iDiff <= 1; ++iDiff) {
		for (int jDiff = -1; jDiff <= 1; ++jDiff) {
			if (iDiff == 0 && jDiff == 0) {continue;}

			auto newI = i + iDiff;
			if (newI < 0 || newI >= cellICount) {continue;}
			auto newJ = j + jDiff;
			if (newJ < 0 || newJ >= cellJCount) {continue;}

			auto newIndex = newI + newJ * cellICount;
			if (newIndex >= wrong->size()) {continue;}

			auto val = data->GetValue(newIndex);
			int newIDiff, newJDiff;
			getDiff(val, &newIDiff, &newJDiff);

			if (iDiff + newIDiff == 0 && jDiff + newJDiff == 0) {
				(*wrong)[newIndex] = false;
				findWrongPointsRecursively(data, cellICount, cellJCount, newIndex, visited, wrong);
			}
		}
	}
}
