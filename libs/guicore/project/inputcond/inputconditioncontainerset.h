#ifndef CGNSFILEINPUTCONDITIONCONTAINERSET_H
#define CGNSFILEINPUTCONDITIONCONTAINERSET_H

#include "../../guicore_global.h"
#include "inputconditioncontainerinteger.h"
#include "inputconditioncontainerreal.h"
#include "inputconditioncontainerstring.h"
#include "inputconditioncontainerfunctional.h"

#include <QObject>
#include <QMap>
#include <QString>

class QDomNode;
class QWidget;
class InputConditionDialog;
class SolverDefinition;

class GUICOREDLL_EXPORT InputConditionContainerSet : public QObject
{
	Q_OBJECT

public:
	InputConditionContainerSet(QWidget* widget);
	void clear();
	void setup(const QDomNode& condNode, const SolverDefinition& def, bool forBC = false);
	void setBCProperty(const QString& bcname, int bcindex);
	void setComplexProperty(const QString& compname, int compindex);
	InputConditionContainerSet* clone() const;
	void copyValues(const InputConditionContainerSet* set);

private:
	void setupSimple(const QDomNode& contNode, const SolverDefinition& def);
	void setupCustom(const QDomNode& contNode, const SolverDefinition& def);
	void setupCustomRec(const QDomNode& node, const SolverDefinition& def);
	void setupContaner(const QDomNode& node, const SolverDefinition& def);

public:
	void setDefaultValues();
	int load();
	int save();
	void reset();
	QMap<QString, InputConditionContainerInteger>& integers() {return m_integers;}
	const QMap<QString, InputConditionContainerInteger>& integers() const {return m_integers;}
	InputConditionContainerInteger& integer(const QString& name) {return m_integers[name];}
	QMap<QString, InputConditionContainerReal>& reals() {return m_reals;}
	const QMap<QString, InputConditionContainerReal>& reals() const {return m_reals;}
	InputConditionContainerReal& real(const QString& name) {return m_reals[name];}
	QMap<QString, InputConditionContainerString>& strings() {return m_strings;}
	const QMap<QString, InputConditionContainerString>& strings() const {return m_strings;}
	InputConditionContainerString& string(const QString& name) {return m_strings[name];}
	QMap<QString, InputConditionContainerFunctional>& functionals() {return m_functionals;}
	const QMap<QString, InputConditionContainerFunctional>& functionals() const {return m_functionals;}
	InputConditionContainerFunctional& functional(const QString& name) {return m_functionals[name];}
	InputConditionContainer* container(const QString& name) {return m_containers[name];}
	QWidget* parentWidget() const {return m_parentWidget;}

signals:
	void modified();

private:
	QMap<QString, InputConditionContainerInteger> m_integers;
	QMap<QString, InputConditionContainerReal> m_reals;
	QMap<QString, InputConditionContainerString> m_strings;
	QMap<QString, InputConditionContainerFunctional> m_functionals;
	/**
	 * pointer to one of upper four.
	 */
	QMap<QString, InputConditionContainer*> m_containers;

	QWidget* m_parentWidget;
};

#endif // CGNSFILEINPUTCONDITIONCONTAINERSET_H
