#ifndef IRICMERGABLECOMMANDS_H
#define IRICMERGABLECOMMANDS_H

namespace iRIC {
	enum MergableCommand {
		RawDataRiverPathPointTranslate,
		RawDataRiverPathPointMouseRotate,
		RawDataRiverPathPointMouseShift,
		RawDataRiverPathPointMouseMoveExtension,
		RawDataRiverPathPointInsert,

		RawDataRiverCrosssectionDragMove,

		RawDataPolygonDefineNewPoint,
		RawDataPolygonMove,
		RawDataPolygonVertexMove,
		RawDataPolygonAddVertex,

		RawDataPMPolygonAddPointCommand,
		RawDataPointMapBreakLineAddPoint,

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
