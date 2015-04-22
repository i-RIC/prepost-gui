#include "gridcreatingconditionfactory.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

#include <gridcreatingcondition/centerandwidth/gridcreatingconditioncreatorcenterandwidth.h>
#include <gridcreatingcondition/riversurvey/gridcreatingconditioncreatorriversurvey.h>
#include <gridcreatingcondition/gridgenerator/gridcreatingconditioncreatorgridgenerator.h>
#include <gridcreatingcondition/gridcombine/gridcreatingconditioncreatorgridcombine.h>
#include <gridcreatingcondition/rectangularregion/gridcreatingconditioncreatorrectangularregion.h>
#include <gridcreatingcondition/rectangularregionlonlat/gridcreatingconditioncreatorrectangularregionlonlat.h>
#include <gridcreatingcondition/riversurvey15d/gridcreatingconditioncreatorriversurvey15d.h>
#include <gridcreatingcondition/triangle/gridcreatingconditioncreatortriangle.h>
#include <gridcreatingcondition/compoundchannel/gridcreatingconditioncreatorcompoundchannel.h>
#include <gridcreatingcondition/externalprogram/gridcreatingconditioncreatorexternalprogram.h>

#include <QDomNode>
#include <QDomElement>
#include <QLocale>
#include <QSettings>

GridCreatingConditionFactory* GridCreatingConditionFactory::m_instance(0);

GridCreatingConditionFactory::GridCreatingConditionFactory(QWidget* mainWindow)
{
	// @todo
	// add codes here, to register the gridcreating condition creators
	// you develop.
	m_creators.append(new GridCreatingConditionCreatorCenterAndWidth());
	m_creators.append(new GridCreatingConditionCreatorRiverSurvey());
	m_creators.append(new GridCreatingConditionCreatorRectangularRegion());
	m_creators.append(new GridCreatingConditionCreatorRectangularRegionLonLat());
	m_creators.append(new GridCreatingConditionCreatorRiverSurvey15D());
	m_creators.append(new GridCreatingConditionCreatorTriangle());
	m_creators.append(new GridCreatingConditionCreatorCompoundChannel());

	// Get locale info
	QSettings settings;
	QString loc = settings.value("general/locale", "").value<QString>();
	QLocale locale;
	if (loc == ""){
		// locale is set to the system value.
		locale = QLocale::system();
	} else {
		locale = QLocale(loc);
	}
	m_creators.append(GridCreatingConditionCreatorExternalProgram::getList(locale, mainWindow));
	setupNameMap();
}

void GridCreatingConditionFactory::setupNameMap()
{
	for (auto it = m_creators.begin(); it != m_creators.end(); ++it){
		m_creatorNameMap.insert((*it)->name(), *it);
	}
}

const QList<GridCreatingConditionCreator*> GridCreatingConditionFactory::compatibleCreators(SolverDefinitionGridType::GridType gt) const
{
	QList<GridCreatingConditionCreator*> ret;
	for (auto it = m_creators.begin(); it != m_creators.end(); ++it){
		if ((*it)->gridType() == gt){
			ret.append(*it);
		}
	}
	return ret;
}

GridCreatingConditionCreator* GridCreatingConditionFactory::getCreator(const QString& name) const
{
	return m_creatorNameMap.value(name);
}

GridCreatingCondition* GridCreatingConditionFactory::restore(const QDomNode& node, ProjectDataItem* item) const
{
	QString creatorName = node.toElement().attribute("name");
	GridCreatingConditionCreator* creator = getCreator(creatorName);
	if (creator == nullptr){return nullptr;}
	return creator->restore(node, item);
}
