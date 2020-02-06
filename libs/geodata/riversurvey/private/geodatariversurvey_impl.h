#ifndef GEODATARIVERSURVEY_IMPL_H
#define GEODATARIVERSURVEY_IMPL_H

#include "../geodatariversurvey.h"

#include <guibase/polyline/polylinecontroller.h>
#include <guibase/vtktool/vtklabel2dactor.h>

#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>

#include <QCursor>
#include <QPixmap>

class GeoDataRiverSurveyBackgroundGridCreateThread;

class vtkActor;
class vtkActor2D;
class vtkLabeledDataMapper;
class vtkPolyData;
class vtkStringArray;
class vtkUnstructuredGrid;

class QAction;

class GeoDataRiverSurvey::Impl
{
public:
	enum class Mode {
		CreateMode,
		EditMode
	};

	enum class CreateMouseEventMode {
		Normal,
		BeforeDefining,
		Defining,
		MoveVertex,
		MoveVertexPrepare,
		AddVertex,
		AddVertexPrepare,
		AddVertexNotPossible,
		RemoveVertexPrepare,
		RemoveVertexNotPossible,
		EditCoodinatesDialog
	};

	enum class EditMouseEventMode {
		Normal,
		Translate,
		TranslatePrepare,
		RotateRight,
		RotatePrepareRight,
		RotateLeft,
		RotatePrepareLeft,
		Shift,
		ShiftPrepare,
		MoveExtentionEndPointLeft,
		MoveExtensionEndPointPrepareLeft,
		MoveExtentionEndPointRight,
		MoveExtensionEndPointPrepareRight,
		ExpansionRight,
		ExpansionPrepareRight,
		ExpansionLeft,
		ExpansionPrepareLeft,
		AddingExtension,
		Inserting,

		TranslateDialog,
		RotateDialog,
		ShiftDialog,
		ExpansionDialog
	};

	Impl(GeoDataRiverSurvey* rs);
	~Impl();

	void setupActions();
	void setupVtkObjects();
	void setupMenu();
	void setupCreateModeMenu(QMenu* menu);
	void setupEditModeMenu(QMenu* menu);

	void updateLabelsAndSplines();

	void updateActionStatus();
	void createModeUpdateActionStatus();
	void editModeUpdateActionStatus();

	void updateVtkPointsObjects();
	void updateVtkCenterAndBankLinesObjects();
	void updateVtkSelectedObjects();
	void updateVtkVerticalCenterLinesObjects();
	void updateVtkNameLabelObjects();
	void updateVtkBackgroundObjects();

	void importLine(PolyLineController* line);
	void exportLine(PolyLineController* line, const QString &lineName);

	void createModeUpdateMouseEventMode(const QPoint& mousePosition);
	void editModeUpdateMouseEventMode();

	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void createModeUpdateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void editModeUpdateMouseCursor(PreProcessorGraphicsViewInterface* v);

	Mode m_mode;
	CreateMouseEventMode m_createMouseEventMode;
	EditMouseEventMode m_editMouseEventMode;

	// data for create mode

	PolyLineController m_centerLineController;
	PolyLineController m_leftBankLineController;
	PolyLineController m_rightBankLineController;

	PolyLineController m_centerLineSplineController;
	PolyLineController m_leftBankLineSplineController;
	PolyLineController m_rightBankLineSplineController;

	PolyLineController m_upstreamLineController;
	PolyLineController m_downstreamLineController;

	vtkLabel2DActor m_upstreamActor;
	vtkLabel2DActor m_downstreamActor;

	PolyLineController* m_activePoints;
	PolyLineController* m_activeLine;

	double m_previousLeftBankDistance;
	double m_previousRightBankDistance;

	// data for edit mode

	vtkPoints* m_pointPoints;

	vtkPolyData* m_riverCenterPoints;
	vtkActor* m_riverCenterPointsActor;

	vtkPolyData* m_crossSectionLines;
	vtkActor* m_crossSectionLinesActor;

	vtkPolyData* m_centerAndBankLines;
	vtkActor* m_centerAndBankLinesActor;

	vtkPolyData* m_selectedRiverCenterPoints;
	vtkActor* m_selectedRiverCenterPointsActor;

	vtkPolyData* m_selectedLeftBankPoints;
	vtkActor* m_selectedLeftBankPointsActor;

	vtkPolyData* m_selectedRightBankPoints;
	vtkActor* m_selectedRightBankPointsActor;

	vtkPolyData* m_selectedCrossSectionLines;
	vtkActor* m_selectedCrossSectionLinesActor;

	vtkPolyData* m_verticalCrossSectionLines;
	vtkActor* m_verticalCrossSectionLinesActor;

	vtkPolyData* m_blackCrossSection;
	vtkActor* m_blackCrossSectionActor;

	vtkPoints* m_rightBankPoints;
	vtkUnstructuredGrid* m_rightBankPointSet;

	vtkStringArray* m_labelArray;
	vtkLabeledDataMapper* m_labelMapper;
	vtkActor2D* m_labelActor;

	vtkSmartPointer<vtkStructuredGrid> m_backgroundGrid;
	vtkActor* m_backgroundActor;

	QMenu* m_rightClickingMenu;
	bool m_menuIsSetup;

	QAction* m_generateAction;
	QAction* m_buildBankLinesAction;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_importCenterLineAction;
	QAction* m_exportCenterLineAction;

	QAction* m_addUpperSideAction;
	QAction* m_addLowerSideAction;
	QAction* m_moveAction;
	QAction* m_rotateAction;
	QAction* m_shiftAction;
	QAction* m_expandAction;
	QAction* m_deleteAction;
	QAction* m_renameAction;
	QAction* m_addLeftExtensionPointAction;
	QAction* m_addRightExtensionPointAction;
	QAction* m_removeLeftExtensionPointAction;
	QAction* m_removeRightExtensionPointAction;
	QAction* m_openCrossSectionWindowAction;
	QAction* m_showBackgroundAction;
	QAction* m_interpolateSplineAction;
	QAction* m_interpolateLinearAction;
	QAction* m_mapPointsAction;

	QPixmap m_pixmapAdd;
	QPixmap m_pixmapRemove;
	QPixmap m_pixmapMove;
	QPixmap m_pixmapRotate;
	QPixmap m_pixmapExpand;
	QPixmap m_pixmapShift;

	QCursor m_cursorAdd;
	QCursor m_cursorRemove;
	QCursor m_cursorMove;
	QCursor m_cursorRotate;
	QCursor m_cursorExpand;
	QCursor m_cursorShift;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	bool m_definingBoundingBox;
	bool m_leftButtonDown;

	int m_selectedVertexId;
	int m_selectedEdgeId;

	Qt::KeyboardModifiers m_keyboardModifiers;
	GridCreatingConditionRiverSurveyInterface* m_gridCreatingCondition;
	GeoDataRiverSurveyBackgroundGridCreateThread* m_gridThread;

	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_IMPL_H
