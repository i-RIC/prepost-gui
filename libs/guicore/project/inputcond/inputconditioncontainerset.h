#ifndef INPUTCONDITIONCONTAINERSET_H
#define INPUTCONDITIONCONTAINERSET_H

#include "../../guicore_global.h"
#include "inputconditioncontainerinteger.h"
#include "inputconditioncontainerreal.h"
#include "inputconditioncontainerstring.h"
#include "inputconditioncontainerfunctional.h"

#include <QObject>
#include <QString>

#include <string>
#include <map>

namespace iRICLib {
	class H5CgnsConditionGroup;
} // namespace iRICLib

class QDomNode;
class QWidget;
class InputConditionDialog;
class SolverDefinition;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionContainerSet : public QObject
{
	Q_OBJECT

public:
	InputConditionContainerSet();
	void clear();
	void setup(const QDomNode& condNode, const SolverDefinition& def, const SolverDefinitionTranslator &t, bool forBC = false);
	void setBCProperty(const std::string& bcname, int bcindex);
	void setComplexProperty(const std::string& compname, int compindex);
	InputConditionContainerSet* clone() const;
	void copyValues(const InputConditionContainerSet* set);

	int load(const iRICLib::H5CgnsConditionGroup& group);
	int save();
	void reset();
	void setDefaultValues();

	bool importFromYaml(const QString& filename);
	bool exportToYaml(const QString& filename);

	std::map<std::string, InputConditionContainerInteger>& integers();
	const std::map<std::string, InputConditionContainerInteger>& integers() const;
	InputConditionContainerInteger& integer(const std::string& name);

	std::map<std::string, InputConditionContainerReal>& reals();
	const std::map<std::string, InputConditionContainerReal>& reals() const;
	InputConditionContainerReal& real(const std::string& name);

	std::map<std::string, InputConditionContainerString>& strings();
	const std::map<std::string, InputConditionContainerString>& strings() const;
	InputConditionContainerString& string(const std::string& name);

	std::map<std::string, InputConditionContainerFunctional>& functionals();
	const std::map<std::string, InputConditionContainerFunctional>& functionals() const;
	InputConditionContainerFunctional& functional(const std::string& name);

	InputConditionContainer* container(const std::string& name);

signals:
	void modified();

private:
	void setupSimple(const QDomNode& contNode, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void setupCustom(const QDomNode& contNode, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void setupCustomRec(const QDomNode& node, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void setupContaner(const QDomNode& node, const SolverDefinition& def, const SolverDefinitionTranslator& t);

	std::map<std::string, InputConditionContainerInteger> m_integers;
	std::map<std::string, InputConditionContainerReal> m_reals;
	std::map<std::string, InputConditionContainerString> m_strings;
	std::map<std::string, InputConditionContainerFunctional> m_functionals;
	// pointer to one of upper four.
	std::map<std::string, InputConditionContainer*> m_containers;
};

#endif // INPUTCONDITIONCONTAINERSET_H
