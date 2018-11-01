#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_IMPL_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_IMPL_H

#include "../geodatariversurveycrosssectionwindow.h"

#include <QIcon>

class ColorSource;

class QCheckBox;
class QComboBox;
class QStandardItemModel;

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

	QComboBox* m_crosssectionComboBox;
	QCheckBox* m_autoRescaleCheckBox;

	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;

	PreProcessorGeoDataGroupDataItemInterface* m_groupDataItem;

	QMenu* m_elevationPointMenu;
	QAction* m_editFromSelectedPointAction;
	QAction* m_inactivateByWEOnlyThisAction;
	QAction* m_inactivateByWEAllAction;
	QAction* m_deleteAction;

	GeoDataRiverSurveyCrosssectionWindowProjectDataItem* m_projectDataItem;
	QIcon m_icon;
	ColorSource* m_colorSource;

	bool m_settingUp;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_IMPL_H
