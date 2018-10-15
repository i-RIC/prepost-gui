#ifndef GEODATARIVERSURVEYPROXY_H
#define GEODATARIVERSURVEYPROXY_H

#include "geodatariversurveydisplaysetting.h"

#include <guicore/pre/geodata/geodataproxy.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkUnstructuredGrid.h>

class GeoDataRiverSurvey;

class GeoDataRiverSurveyProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataRiverSurveyProxy(GeoDataRiverSurvey* geodata);
	~GeoDataRiverSurveyProxy();

	void setupActors() override;

	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

public slots:
	void updateGraphics() override;

private:
	void assignActorZValues(const ZDepthRange& range) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GeoDataRiverSurveyDisplaySetting m_setting;

	vtkSmartPointer<vtkActor> m_riverCenterLineActor;
	vtkSmartPointer<vtkActor> m_leftBankLineActor;
	vtkSmartPointer<vtkActor> m_rightBankLineActor;

	vtkSmartPointer<vtkActor> m_firstAndLastCrosssectionsActor;
	vtkSmartPointer<vtkActor> m_crossectionsActor;

	vtkSmartPointer<vtkActor> m_backgroundActor;
	vtkSmartPointer<vtkActor> m_crosssectionLinesActor;

	vtkSmartPointer<vtkActor2D> m_labelActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_crosssectionLines;

	class SetSettingCommand;
};

#endif // GEODATARIVERSURVEYPROXY_H
