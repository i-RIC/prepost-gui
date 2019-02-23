#include "gridcreatingconditionfactory.h"

#include <gridcreatingcondition/centerandwidth/gridcreatingconditioncreatorcenterandwidth.h>
#include <gridcreatingcondition/compoundchannel/gridcreatingconditioncreatorcompoundchannel.h>
#include <gridcreatingcondition/externalprogram/gridcreatingconditioncreatorexternalprogram.h>
#include <gridcreatingcondition/gridcombine/gridcreatingconditioncreatorgridcombine.h>
#include <gridcreatingcondition/gridgenerator/gridcreatingconditioncreatorgridgenerator.h>
#include <gridcreatingcondition/laplace/gridcreatingconditioncreatorlaplace.h>
#include <gridcreatingcondition/poisson/gridcreatingconditioncreatorpoisson.h>
#include <gridcreatingcondition/rectangularregion/gridcreatingconditioncreatorrectangularregion.h>
#include <gridcreatingcondition/rectangularregionlonlat/gridcreatingconditioncreatorrectangularregionlonlat.h>
#include <gridcreatingcondition/riversurvey/gridcreatingconditioncreatorriversurvey.h>
#include <gridcreatingcondition/riversurvey15d/gridcreatingconditioncreatorriversurvey15d.h>
#include <gridcreatingcondition/triangle/gridcreatingconditioncreatortriangle.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

#include <QDomElement>
#include <QDomNode>
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
	m_creators.append(new GridCreatingConditionCreatorPoisson());
	m_creators.append(new GridCreatingConditionCreatorLaplace());

	// Get locale info
	QSettings settings;
	QString loc = settings.value("general/locale", "").value<QString>();
	QLocale locale;
	if (loc == "") {
		// locale is set to the system value.
		locale = QLocale::system();
	} else {
		locale = QLocale(loc);
	}
	m_creators.append(GridCreatingConditionCreatorExternalProgram::getList(locale, mainWindow));
	setupNameMap();
}

GridCreatingConditionFactory& GridCreatingConditionFactory::instance(QWidget* mainWindow)
{
	if (m_instance == nullptr) {
		m_instance = new GridCreatingConditionFactory(mainWindow);
	}
	return *m_instance;
}

const QList<GridCreatingConditionCreator*> GridCreatingConditionFactory::compatibleCreators(const SolverDefinitionGridType& gridType) const
{
	QList<GridCreatingConditionCreator*> ret;
	QList<GridCreatingConditionCreator*>::const_iterator it;

	if (gridType.availableGridGenerators().size() > 0) {
		// gridgenerators specified. select the generators in the list only.
		QList<QString> availableGenerators = gridType.availableGridGenerators();
		for (it = m_creators.begin(); it != m_creators.end(); ++it){
			QString genName = (*it)->name();
			if (availableGenerators.contains(genName)) {
				ret.append(*it);
			}
		}
	} else {
		// select grid generators that can generate grids that are supported by the solver.
		for (it = m_creators.begin(); it != m_creators.end(); ++it){
			QList<SolverDefinitionGridType::GridType> types = gridType.availableGridTypes();
			QList<SolverDefinitionGridType::GridType>::const_iterator it2;
			for (it2 = types.begin(); it2 != types.end(); ++it2) {
				if ((*it)->gridType() == *it2) {
					ret.append(*it);
					break;
				}
			}
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
	if (creator == nullptr) {return nullptr;}
	return creator->restore(node, item);
}

void GridCreatingConditionFactory::setMainWindow(QWidget* mw)
{
	m_mainWindow = mw;
}

void GridCreatingConditionFactory::setupNameMap()
{
	for (auto it = m_creators.begin(); it != m_creators.end(); ++it) {
		m_creatorNameMap.insert((*it)->name(), *it);
	}
}
