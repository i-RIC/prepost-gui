#include "solverdefinition.h"
#include "solverdefinitionabstract.h"
#include "solverdefinitiongridtype.h"
#include "solverdefinitiontranslator.h"
#include "private/solverdefinition_impl.h"

#include <misc/errormessage.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QMap>

const QString SolverDefinition::FILENAME {"definition.xml"};
const QString SolverDefinition::README {"README"};
const QString SolverDefinition::LICENSE {"LICENSE"};

SolverDefinition::Impl::Impl(const QString& solverfolder, const QLocale& locale, SolverDefinition *p) :
	m_locale {locale},
	m_abstract {solverfolder, locale, 0},
	m_parent {p}
{}

SolverDefinition::Impl::~Impl()
{
	for (auto t : m_gridTypes) {
		delete t;
	}
	delete m_dummyGridType;
}

void SolverDefinition::Impl::load()
{
	QDir folder = m_abstract.folder();
	QString filename = folder.absoluteFilePath(SolverDefinition::FILENAME);
	// Set up translator first.
	SolverDefinitionTranslator translator = buildTranslator();
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
	setupIterationType(sdElem);
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
			SolverDefinitionGridType* gt = setupGridType(SDNode, true);
			// The Grid Type caption is modified.
			gt->setCaption(QString(tr("%1 Grids")).arg(m_parent->caption()));
			m_gridTypes.append(gt);
			m_gridTypeNameMap.insert(gt->name(), gt);
		}
	} else {
		setupGridTypes(gdsNode, translator);
	}
	// Create dummy grid type for post-processor to handle grids.
	m_dummyGridType = new SolverDefinitionGridType(m_abstract.name(), m_abstract.caption());
}

void SolverDefinition::Impl::setupIterationType(const QDomElement& elem)
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

void SolverDefinition::Impl::setupGridTypes(const QDomNode& node, const SolverDefinitionTranslator& /*translator*/)
{
	QDomNode child = node.firstChild();
	bool isPrimary = true;
	while (! child.isNull()) {
		if (child.nodeName() == "GridType") {
			SolverDefinitionGridType* gt = setupGridType(child, isPrimary);
			m_gridTypes.append(gt);
			m_gridTypeNameMap.insert(gt->name(), gt);
			isPrimary = false;
		}
		child = child.nextSibling();
	}
}

SolverDefinitionGridType* SolverDefinition::Impl::setupGridType(const QDomNode& node, bool isPrimary)
{
	return new SolverDefinitionGridType(node.toElement(), m_parent, isPrimary);
}

SolverDefinitionTranslator SolverDefinition::Impl::buildTranslator() const
{
	QDir folder = m_abstract.folder();
	return SolverDefinitionTranslator(folder.absolutePath(), m_locale);
}

// Public interface implementation

SolverDefinition::SolverDefinition(const QString& solverfolder, const QLocale& locale) :
	impl {new Impl {solverfolder, locale, this}}
{
	impl->load();
}

SolverDefinition::~SolverDefinition()
{
	delete impl;
}

const std::string& SolverDefinition::name() const
{
	return impl->m_abstract.name();
}

const QString& SolverDefinition::caption() const
{
	return impl->m_abstract.caption();
}

const VersionNumber& SolverDefinition::version() const
{
	return impl->m_abstract.version();
}

QString SolverDefinition::description() const
{
	return impl->m_abstract.description();
}

QString SolverDefinition::license() const
{
	return impl->m_abstract.license();
}

const QString& SolverDefinition::copyright() const
{
	return impl->m_abstract.copyright();
}

SolverDefinition::IterationType SolverDefinition::iterationType() const
{
	return impl->m_iterationType;
}

const QString& SolverDefinition::executableFilename() const
{
	return impl->m_executableFilename;
}

const SolverDefinitionAbstract& SolverDefinition::abstract() const
{
	return impl->m_abstract;
}

const QDir& SolverDefinition::folder() const
{
	return impl->m_abstract.folder();
}

const QList<SolverDefinitionGridType*> SolverDefinition::gridTypes() const
{
	return impl->m_gridTypes;
}

SolverDefinitionGridType* SolverDefinition::dummyGridType() const
{
	return impl->m_dummyGridType;
}

SolverDefinitionGridType* SolverDefinition::gridType(const std::string& name) const
{
	return impl->m_gridTypeNameMap.value(name);
}

const QDomDocument& SolverDefinition::document() const
{
	return impl->m_document;
}

SolverDefinitionTranslator SolverDefinition::buildTranslator() const
{
	return impl->buildTranslator();
}
