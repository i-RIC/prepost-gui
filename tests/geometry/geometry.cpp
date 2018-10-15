#include <guibase/geometry/pointring.h>
#include <guibase/geometry/polygonregion.h>

#include <vtkType.h>
#include <iostream>

using namespace std;

void outputPointRing(const PointRing& ring)
{
	for (vtkIdType id : ring.points()) {
		cout << id << " ";
	}
	cout << endl;
}

void outputRegion(const PolygonRegion& region)
{
	cout << "Region: " << endl;
	outputPointRing(*region.region());
	cout << "Holes: " << endl;
	for (auto h : region.holes()) {
		outputPointRing(*h);
	}
}

int main(int /*argc*/, char* /*argv*/[])
{
	vtkPoints* points = nullptr;

	cout << "Case 1" << std::endl;
	PolygonRegion case1_r1(vector<vtkIdType>({0, 1, 2, 3}), points);
	PolygonRegion case1_r2(vector<vtkIdType>({ 2, 1, 4, 5 }), points);
	case1_r1.merge(&case1_r2);
	outputRegion(case1_r1);

	cout << "Case 2-1" << std::endl;
	PolygonRegion case2_1_r1(vector<vtkIdType>({ 0, 1, 2, 3, 4 }), points);
	PolygonRegion case2_1_r2(vector<vtkIdType>({ 2, 5, 6, 3 }), points);
	case2_1_r1.merge(&case2_1_r2);
	outputRegion(case2_1_r1);

	cout << "Case 2-2" << std::endl;
	PolygonRegion case2_2_r1(vector<vtkIdType>({ 3, 4, 0, 1, 2 }), points);
	PolygonRegion case2_2_r2(vector<vtkIdType>({ 2, 5, 6, 3 }), points);
	case2_2_r1.merge(&case2_2_r2);
	outputRegion(case2_2_r1);

	cout << "Case 2-3" << std::endl;
	PolygonRegion case2_3_r1(vector<vtkIdType>({ 3, 4, 0, 1, 2 }), points);
	PolygonRegion case2_3_r2(vector<vtkIdType>({ 5, 2, 3, 6 }), points);
	case2_3_r1.merge(&case2_3_r2);
	outputRegion(case2_3_r1);

	cout << "Case 3-1" << std::endl;
	PolygonRegion case3_1_r1(vector<vtkIdType>({ 0, 1, 2, 3, 4 }), points);
	PolygonRegion case3_1_r2(vector<vtkIdType>({ 0, 1, 2, 6, 5 }), points);
	case3_1_r1.merge(&case3_1_r2);
	outputRegion(case3_1_r1);

	cout << "Case 3-2" << std::endl;
	PolygonRegion case3_2_r1(vector<vtkIdType>({ 0, 1, 2, 3, 4 }), points);
	PolygonRegion case3_2_r2(vector<vtkIdType>({ 6, 5, 0, 1, 2 }), points);
	case3_2_r1.merge(&case3_1_r2);
	outputRegion(case3_2_r1);

	cout << "Case 3-3" << std::endl;
	PolygonRegion case3_3_r1(vector<vtkIdType>({ 0, 1, 2, 3, 4 }), points);
	PolygonRegion case3_3_r2(vector<vtkIdType>({ 1, 0, 5, 6, 2 }), points);
	case3_3_r1.merge(&case3_3_r2);
	outputRegion(case3_3_r1);

	cout << "Case 3-4" << std::endl;
	PolygonRegion case3_4_r1(vector<vtkIdType>({ 0, 1, 2, 3, 4 }), points);
	PolygonRegion case3_4_r2(vector<vtkIdType>({ 2, 1, 0, 5, 6 }), points);
	case3_4_r1.merge(&case3_4_r2);
	outputRegion(case3_4_r1);

	cout << "Case 4-1" << std::endl;
	PolygonRegion case4_1_r1(vector<vtkIdType>({ 0, 1, 2, 3, 4, 5 }), points);
	PolygonRegion case4_1_r2(vector<vtkIdType>({ 6, 7, 0, 1, 2, 3 }), points);
	case4_1_r1.merge(&case4_1_r2);
	outputRegion(case4_1_r1);

	cout << "Case 4-2" << std::endl;
	PolygonRegion case4_2_r1(vector<vtkIdType>({ 0, 1, 2, 3, 4, 5 }), points);
	PolygonRegion case4_2_r2(vector<vtkIdType>({ 3, 2, 1, 0, 7, 6}), points);
	case4_2_r1.merge(&case4_2_r2);
	outputRegion(case4_2_r1);

	cout << "Case 5-1" << std::endl;
	PolygonRegion case5_1_r1(vector<vtkIdType>({ 0, 1, 2 }), points);
	PolygonRegion case5_1_r2(vector<vtkIdType>({ 0, 2, 3 }), points);
	case5_1_r1.merge(&case5_1_r2);
	outputRegion(case5_1_r1);

	cout << "Case 5-2" << std::endl;
	PolygonRegion case5_2_r1(vector<vtkIdType>({ 0, 1, 2 }), points);
	PolygonRegion case5_2_r2(vector<vtkIdType>({ 2, 3, 0 }), points);
	case5_2_r1.merge(&case5_2_r2);
	outputRegion(case5_2_r1);

	cout << "Case 5-3" << std::endl;
	PolygonRegion case5_3_r1(vector<vtkIdType>({ 0, 1, 2 }), points);
	PolygonRegion case5_3_r2(vector<vtkIdType>({ 0, 3, 2 }), points);
	case5_3_r1.merge(&case5_3_r2);
	outputRegion(case5_3_r1);

	cout << "Case 6-1" << std::endl;
	PolygonRegion case6_1_r1(vector<vtkIdType>({ 0, 1, 2, 3 }), points);
	PolygonRegion case6_1_r2(vector<vtkIdType>({ 0, 3, 4 }), points);
	case6_1_r1.merge(&case6_1_r2);
	outputRegion(case6_1_r1);

	cout << "Case 6-2" << std::endl;
	PolygonRegion case6_2_r1(vector<vtkIdType>({ 2, 3, 0, 1}), points);
	PolygonRegion case6_2_r2(vector<vtkIdType>({ 0, 4, 3 }), points);
	case6_2_r1.merge(&case6_2_r2);
	outputRegion(case6_2_r1);

	cout << "Case 6-3" << std::endl;
	PolygonRegion case6_3_r1(vector<vtkIdType>({ 2, 3, 0, 1}), points);
	PolygonRegion case6_3_r2(vector<vtkIdType>({ 0, 3, 4 }), points);
	case6_3_r1.merge(&case6_3_r2);
	outputRegion(case6_3_r1);

}
