#ifndef RIVERSURVEYDATACREATOR_H
#define RIVERSURVEYDATACREATOR_H

class Project;
class RiverSurveyData;

class QWidget;

class RiverSurveyDataCreator
{
public:
	static bool checkIfReadyToCreate(const Project& project, QWidget* w);
	static RiverSurveyData* create(const Project& project);

private:
	RiverSurveyDataCreator();
	~RiverSurveyDataCreator();
};

#endif // RIVERSURVEYDATACREATOR_H
