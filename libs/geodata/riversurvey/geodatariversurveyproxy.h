#ifndef GEODATARIVERSURVEYPROXY_H
#define GEODATARIVERSURVEYPROXY_H

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkUnstructuredGrid.h>

#include <guicore/pre/geodata/geodataproxy.h>
#include "geodatariversurvey.h"

class GeoDataRiverSurveyProxyDisplaySettingCommand;

class GeoDataRiverSurveyProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataRiverSurveyProxy(GeoDataRiverSurvey* geodata) : GeoDataProxy(geodata) {}
	~GeoDataRiverSurveyProxy();
	void setupActors() override;

	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

public slots:
	void updateGraphics() override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

private:
	bool m_showBackground;
	bool m_showLines;
	int m_opacityPercent;
	int m_crosssectionLinesScale;
	QColor m_crosssectionLinesColor;

	vtkSmartPointer<vtkActor> m_riverCenterLineActor;
	vtkSmartPointer<vtkActor> m_leftBankLineActor;
	vtkSmartPointer<vtkActor> m_rightBankLineActor;

	vtkSmartPointer<vtkActor> m_firstAndLastCrosssectionsActor;
	vtkSmartPointer<vtkActor> m_crossectionsActor;

	vtkSmartPointer<vtkActor> m_backgroundActor;
	vtkSmartPointer<vtkActor> m_crosssectionLinesActor;

	vtkSmartPointer<vtkActor2D> m_labelActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_crosssectionLines;

public:
	friend class GeoDataRiverSurveyProxyDisplaySettingCommand;
};

#endif // GEODATARIVERSURVEYPROXY_H
