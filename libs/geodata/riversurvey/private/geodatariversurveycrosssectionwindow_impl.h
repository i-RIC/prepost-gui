#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_IMPL_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_IMPL_H

#include "../geodatariversurveycrosssectionwindow.h"

#include <QIcon>

class ColorSource;
class RealNumberEditWidget;

class QCheckBox;
class QComboBox;
class QStandardItemModel;

struct ReferenceRiverPathPoint {
	int dataIndex;
	GeoDataRiverPathPoint* point;
};

class GeoDataRiverSurveyCrosssectionWindow::Impl
{
public:
	Impl(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi);
	~Impl();

	GeoDataRiverSurvey* m_targetRiverSurvey;
	GeoDataRiverPathPoint* m_editTargetPoint;

	GeoDataRiverSurvey* m_gridCreatingConditionRiverSurvey;
	GeoDataRiverPathPoint* m_gridCreatingConditionPoint;

	QString m_crosssectionName;

	QList<bool> m_riverSurveyEnables;
	QList<GeoDataRiverSurvey*> m_riverSurveys;
	QList<QColor> m_riverSurveyColors;
	QList<GeoDataRiverPathPoint*> m_riverPathPoints;
	QList<QString> m_crosssectionNames;
	QList<ReferenceRiverPathPoint> m_referenceRiverPathPoints;

	QComboBox* m_crosssectionComboBox;
	QComboBox* m_referenceComboBox;
	QCheckBox* m_referenceCheckBox;
	QCheckBox* m_autoRescaleCheckBox;

	QCheckBox* m_gridDisplayCheckBox;
	QCheckBox* m_scaleDisplayCheckBox;
	QCheckBox* m_aspectRatioDisplayCheckBox;
	QCheckBox* m_markersDisplayCheckBox;

	RealNumberEditWidget* m_aspectRatioEdit;
	QCheckBox* m_fixAspectRatioCheckBox;
	QCheckBox* m_fixRegionCheckBox;

	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;

	PreProcessorGeoDataGroupDataItemInterface* m_groupDataItem;

	QMenu* m_elevationPointMenu;
	QAction* m_editFromSelectedPointAction;
	QAction* m_editFromSelectedPointWithDialogAction;
	QAction* m_inactivateByWEOnlyThisAction;
	QAction* m_inactivateByWEAllAction;
	QAction* m_deleteAction;

	GeoDataRiverSurveyCrosssectionWindowProjectDataItem* m_projectDataItem;
	QIcon m_icon;
	ColorSource* m_colorSource;

	bool m_settingUp;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_IMPL_H
