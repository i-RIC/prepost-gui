#ifndef INPUTCONDITIONCONTAINERSET_H
#define INPUTCONDITIONCONTAINERSET_H

#include "../../guicore_global.h"
#include "inputconditioncontainerinteger.h"
#include "inputconditioncontainerreal.h"
#include "inputconditioncontainerstring.h"
#include "inputconditioncontainerfunctional.h"

#include <QObject>
#include <QMap>
#include <QString>

#include <string>

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

	int load();
	int save();
	void reset();
	void setDefaultValues();

	bool importFromYaml(const QString& filename);
	bool exportToYaml(const QString& filename);

	QMap<std::string, InputConditionContainerInteger>& integers();
	const QMap<std::string, InputConditionContainerInteger>& integers() const;
	InputConditionContainerInteger& integer(const std::string& name);

	QMap<std::string, InputConditionContainerReal>& reals();
	const QMap<std::string, InputConditionContainerReal>& reals() const;
	InputConditionContainerReal& real(const std::string& name);

	QMap<std::string, InputConditionContainerString>& strings();
	const QMap<std::string, InputConditionContainerString>& strings() const;
	InputConditionContainerString& string(const std::string& name);

	QMap<std::string, InputConditionContainerFunctional>& functionals();
	const QMap<std::string, InputConditionContainerFunctional>& functionals() const;
	InputConditionContainerFunctional& functional(const std::string& name);

	InputConditionContainer* container(const std::string& name);

signals:
	void modified();

private:
	void setupSimple(const QDomNode& contNode, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void setupCustom(const QDomNode& contNode, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void setupCustomRec(const QDomNode& node, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void setupContaner(const QDomNode& node, const SolverDefinition& def, const SolverDefinitionTranslator& t);

private:
	QMap<std::string, InputConditionContainerInteger> m_integers;
	QMap<std::string, InputConditionContainerReal> m_reals;
	QMap<std::string, InputConditionContainerString> m_strings;
	QMap<std::string, InputConditionContainerFunctional> m_functionals;
	// pointer to one of upper four.
	QMap<std::string, InputConditionContainer*> m_containers;
};

#endif // INPUTCONDITIONCONTAINERSET_H
