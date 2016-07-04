#ifndef IRICMERGABLECOMMANDS_H
#define IRICMERGABLECOMMANDS_H

namespace iRIC
{
	enum MergableCommand {
		GeoDataRiverPathPointTranslate,
		GeoDataRiverPathPointMouseRotate,
		GeoDataRiverPathPointMouseShift,
		GeoDataRiverPathPointMouseMoveExtension,
		GeoDataRiverPathPointInsert,

		GeoDataRiverCrosssectionDragMove,

		GeoDataPolygonDefineNewPoint,
		GeoDataPolygonMove,
		GeoDataPolygonVertexMove,
		GeoDataPolygonAddVertex,

		GeoDataPMPolygonAddPointCommand,
		GeoDataPointMapBreakLineAddPoint,

		GridCreatingConditionRiverSurveyMovePoint,

		GridCreatingConditionCenterAndWidthDefineNewPoint,
		GridCreatingConditionCenterAndWidthMove,
		GridCreatingConditionCenterAndWidthVertexMove,
		GridCreatingConditionCenterAndWidthAddVertex,

		GridCreatingConditionTriangleDefineNewPoint,
		GridCreatingConditionTriangleMove,
		GridCreatingConditionTriangleVertexMove,
		GridCreatingConditionTriangleAddVertex,
		GridCreatingConditionTrianglePolyLineDefineNewPoint,
		GridCreatingConditionTrianglePolyLineMove,
		GridCreatingConditionTrianglePolyLineVertexMove,
		GridCreatingConditionTrianglePolyLineAddVertex,

		GridCreatingConditionCompoundChannelPolygonDefineNewPoint,
		GridCreatingConditionCompoundChannelPolygonMove,
		GridCreatingConditionCompoundChannelPolygonVertexMove,
		GridCreatingConditionCompoundChannelPolygonAddVertex,
		GridCreatingConditionCompoundChannelPolyLineDefineNewPoint,
		GridCreatingConditionCompoundChannelPolyLineMove,
		GridCreatingConditionCompoundChannelPolyLineVertexMove,
		GridCreatingConditionCompoundChannelPolyLineAddVertex,

		GridPointMouseMove,
		Structured2DGridCrosssectionDragMove,

		DistanceMeasureDefine,
		DistanceMeasureMoveVertex,
	};
}

#endif // IRICMERGABLECOMMANDS_H
