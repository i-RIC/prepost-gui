#ifndef CGNSFILEINPUTCONDITIONCONTAINERSET_H
#define CGNSFILEINPUTCONDITIONCONTAINERSET_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditioncontainerinteger.h"
#include "cgnsfileinputconditioncontainerreal.h"
#include "cgnsfileinputconditioncontainerstring.h"
#include "cgnsfileinputconditioncontainerfunctional.h"

#include <QObject>
#include <QMap>
#include <QString>


class QDomNode;
class CgnsFileInputConditionDialog;
class QWidget;

class GUICOREDLL_EXPORT CgnsFileInputConditionContainerSet : public QObject
{
	Q_OBJECT

public:
	CgnsFileInputConditionContainerSet(QWidget* widget);
	void clear();
	void setup(const QDomNode& condNode, bool forBC = false);
	void setBCProperty(const QString& bcname, int bcindex);
	void setComplexProperty(const QString& compname, int compindex);
	CgnsFileInputConditionContainerSet* clone() const;
	void copyValues(const CgnsFileInputConditionContainerSet* set);

private:
	void setupSimple(const QDomNode& contNode);
	void setupCustom(const QDomNode& contNode);
	void setupCustomRec(const QDomNode& node);
	void setupContaner(const QDomNode& node);

public:
	void setDefaultValues();
	int load();
	int save();
	void reset();
	/**
	 * constant integer values
	 */
	QMap<QString, CgnsFileInputConditionContainerInteger>& integers(){return m_integers;}
	CgnsFileInputConditionContainerInteger& integer(const QString& name){return m_integers[name];}
	/**
	 * constant real values
	 */
	QMap<QString, CgnsFileInputConditionContainerReal>& reals(){return m_reals;}
	CgnsFileInputConditionContainerReal& real(const QString& name){return m_reals[name];}
	/**
	 * constant string values (including filenames)
	 */
	QMap<QString, CgnsFileInputConditionContainerString>& strings(){return m_strings;}
	CgnsFileInputConditionContainerString& string(const QString& name){return m_strings[name];}
	/**
	 *
	 */
	QMap<QString, CgnsFileInputConditionContainerFunctional>& functionals(){return m_functionals;}
	CgnsFileInputConditionContainerFunctional& functional(const QString& name){return m_functionals[name];}
	CgnsFileInputConditionContainer* container(const QString& name){return m_containers[name];}
	QWidget* parentWidget(){return m_parentWidget;}

signals:
	void modified();

private:
	QMap<QString, CgnsFileInputConditionContainerInteger> m_integers;
	QMap<QString, CgnsFileInputConditionContainerReal> m_reals;
	QMap<QString, CgnsFileInputConditionContainerString> m_strings;
	QMap<QString, CgnsFileInputConditionContainerFunctional> m_functionals;
	/**
	 * pointer to one of upper four.
	 */
	QMap<QString, CgnsFileInputConditionContainer*> m_containers;

	QWidget* m_parentWidget;
};

#endif // CGNSFILEINPUTCONDITIONCONTAINERSET_H
