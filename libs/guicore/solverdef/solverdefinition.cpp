#include "solverdefinition.h"
#include "solverdefinitiongridtype.h"
#include "solverdefinitiontranslator.h"

#include <misc/errormessage.h>
#include <misc/xmlsupport.h>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

const QString SolverDefinition::FILENAME = QString("definition.xml");
const QString SolverDefinition::README = QString("README");
const QString SolverDefinition::LICENSE = QString("LICENSE");

SolverDefinition::~SolverDefinition()
{
	for (auto it = m_gridTypes.begin(); it != m_gridTypes.end(); ++it) {
		delete *it;
	}
	delete m_dummyGridType;
}

void SolverDefinition::load(const QLocale& locale)
{
	QDir folder = m_abstract.folder();
	QString filename = folder.absoluteFilePath(SolverDefinition::FILENAME);
	// Set up translator first.
	SolverDefinitionTranslator translator(folder.absolutePath(), locale);
	QString errorStr;
	int errorLine;
	int errorColumn;
	QFile file(filename);
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = m_document.setContent(&file, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(filename).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
	m_abstract.setupDomDocument(&m_document);

	QDomNode SDNode = m_document.documentElement();
	QDomElement sdElem = SDNode.toElement();
	// executable filename
	m_executableFilename = folder.absoluteFilePath(sdElem.attribute("executable"));
	// iterationtype
	setIterationType(sdElem);
	// setup gridtypes
	QDomNode gdsNode = iRIC::getChildNode(sdElem, "GridTypes");
	if (gdsNode.isNull()) {
		// Try to find "GridRelatedCondition" node just under SolverDefinition node.
		// if found, it means that this solver supports only one grid, with one gridtype.
		QDomNode gdCondNode = iRIC::getChildNode(sdElem, "GridRelatedCondition");
		if (gdCondNode.isNull()) {
			// No grid related condition node found.
		} else {
			// found. build one gridtype, using SolverDefinition node.
			SolverDefinitionGridType* gt = setupGridType(SDNode, translator, true);
			// The Grid Type caption is modified.
			gt->setCaption(QString(tr("%1 Grids")).arg(caption()));
			m_gridTypes.append(gt);
			m_gridTypeNameMap.insert(gt->name(), gt);
		}
	} else {
		setupGridTypes(gdsNode, translator);
	}
	// Create dummy grid type for post-processor to handle grids.
	m_dummyGridType = new SolverDefinitionGridType(m_abstract.name(), m_abstract.caption());
}

void SolverDefinition::setIterationType(const QDomElement& elem)
{
	QString itype = elem.attribute("iterationtype", "none");
	if (itype == "none") {
		m_iterationType = NoIteration;
	} else if (itype == "time") {
		m_iterationType = TimeIteration;
	} else if (itype == "convergence") {
		m_iterationType = ConvergenceIteration;
	} else {
		m_iterationType = NoIteration;
	}
}

void SolverDefinition::setupGridTypes(const QDomNode& node, const SolverDefinitionTranslator& translator)
{
	QDomNode child = node.firstChild();
	bool isPrimary = true;
	while (! child.isNull()) {
		if (child.nodeName() == "GridType") {
			SolverDefinitionGridType* gt = setupGridType(child, translator, isPrimary);
			m_gridTypes.append(gt);
			m_gridTypeNameMap.insert(gt->name(), gt);
			isPrimary = false;
		}
		child = child.nextSibling();
	}
}

SolverDefinitionGridType* SolverDefinition::setupGridType(const QDomNode& node, const SolverDefinitionTranslator& translator, bool isPrimary)
{
	SolverDefinitionGridType* gt = new SolverDefinitionGridType(node.toElement(), translator, isPrimary);
	return gt;
}
