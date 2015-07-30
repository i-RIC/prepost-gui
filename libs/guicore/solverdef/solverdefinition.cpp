#include "solverdefinition.h"
#include "solverdefinitionabstract.h"
#include "solverdefinitiongridtype.h"
#include "solverdefinitiontranslator.h"

#include <misc/errormessage.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QMap>

const QString SolverDefinition::FILENAME {"definition.xml"};

class SolverDefinition::Impl
{
public:
	/// Constructor
	Impl(const QString& solverfolder, const QLocale& locale, SolverDefinition* p);
	/// Destructor
	~Impl();

	/// Load information from solver defintion file and translation files.
	void load(const QLocale& locale);
	void setupIterationType(const QDomElement& elem);
	void setupGridTypes(const QDomNode& node, const SolverDefinitionTranslator& translator);
	SolverDefinitionGridType* setupGridType(const QDomNode& node, const SolverDefinitionTranslator& translator, bool isPrimary);

	IterationType m_iterationType;
	QString m_executableFilename {};
	QList<SolverDefinitionGridType*> m_gridTypes {};
	SolverDefinitionGridType* m_dummyGridType {nullptr};
	QMap<QString, SolverDefinitionGridType*> m_gridTypeNameMap {};
	QDomDocument m_document {};

	SolverDefinitionAbstract m_abstract;

private:
	SolverDefinition* m_parent;
};

SolverDefinition::Impl::Impl(const QString& solverfolder, const QLocale& locale, SolverDefinition *p) :
	m_abstract {solverfolder, locale, 0},
	m_parent {p}
{
	load(locale);
}

SolverDefinition::Impl::~Impl()
{
	for (auto t : m_gridTypes) {
		delete t;
	}
	delete m_dummyGridType;
}

void SolverDefinition::Impl::load(const QLocale& locale)
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
			SolverDefinitionGridType* gt = setupGridType(SDNode, translator, true);
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

void SolverDefinition::Impl::setupGridTypes(const QDomNode& node, const SolverDefinitionTranslator& translator)
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

SolverDefinitionGridType* SolverDefinition::Impl::setupGridType(const QDomNode& node, const SolverDefinitionTranslator& translator, bool isPrimary)
{
	return new SolverDefinitionGridType(node.toElement(), translator, isPrimary);
}

// Public interface implementation

SolverDefinition::SolverDefinition(const QString& solverfolder, const QLocale& locale) :
	m_impl {new Impl {solverfolder, locale, this}}
{}

SolverDefinition::~SolverDefinition()
{
	delete m_impl;
}

const QString& SolverDefinition::name() const
{
	return m_impl->m_abstract.name();
}

const QString& SolverDefinition::caption() const
{
	return m_impl->m_abstract.caption();
}

const VersionNumber& SolverDefinition::version() const
{
	return m_impl->m_abstract.version();
}

QString SolverDefinition::description() const
{
	return m_impl->m_abstract.description();
}

QString SolverDefinition::license() const
{
	return m_impl->m_abstract.license();
}

const QString& SolverDefinition::copyright() const
{
	return m_impl->m_abstract.copyright();
}

SolverDefinition::IterationType SolverDefinition::iterationType() const
{
	return m_impl->m_iterationType;
}

const QString& SolverDefinition::executableFilename() const
{
	return m_impl->m_executableFilename;
}

const SolverDefinitionAbstract& SolverDefinition::abstract() const
{
	return m_impl->m_abstract;
}

const QDir& SolverDefinition::folder() const
{
	return m_impl->m_abstract.folder();
}

const QList<SolverDefinitionGridType*> SolverDefinition::gridTypes() const
{
	return m_impl->m_gridTypes;
}

SolverDefinitionGridType* SolverDefinition::dummyGridType() const
{
	return m_impl->m_dummyGridType;
}

SolverDefinitionGridType* SolverDefinition::gridType(const QString& name) const
{
	return m_impl->m_gridTypeNameMap.value(name);
}

const QDomDocument& SolverDefinition::document() const
{
	return m_impl->m_document;
}
