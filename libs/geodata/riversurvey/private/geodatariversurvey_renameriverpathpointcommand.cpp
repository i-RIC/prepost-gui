#include "geodatariversurvey_renameriverpathpointcommand.h"

GeoDataRiverSurvey::RenameRiverPathPointCommand::RenameRiverPathPointCommand(const QString& newname, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs) :
	QUndoCommand{GeoDataRiverSurvey::tr("Rename Traversal Line")},
	m_newName {newname},
	m_oldName {p->name()},
	m_point {p},
	m_rs {rs}
{}

void GeoDataRiverSurvey::RenameRiverPathPointCommand::redo()
{
	setName(m_newName);
	m_rs->setModified();
}

void GeoDataRiverSurvey::RenameRiverPathPointCommand::undo()
{
	setName(m_oldName);
}

void GeoDataRiverSurvey::RenameRiverPathPointCommand::setName(const QString& name)
{
	m_point->setName(name);
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}
