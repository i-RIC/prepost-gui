#ifndef CGNSFILEINPUTCONDITIONDEPENDENCY_H
#define CGNSFILEINPUTCONDITIONDEPENDENCY_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"
#include <QList>
#include <QObject>

class QDomNode;
class CgnsFileInputConditionWidgetSet;
class CgnsFileInputConditionContainer;
class CgnsFileInputConditionContainerSet;
class CgnsFileInputConditionDependencyBuilder;

/// This class holds the dependencies between input conditions.
class GUICOREDLL_EXPORT CgnsFileInputConditionDependency : public QObject
{
	Q_OBJECT
public:

	class Action {
	protected:
		/// Constructor
		Action(CgnsFileInputConditionWidget* w){
			m_target = w;
		}
	public:
		virtual ~Action(){}
		virtual void positiveAction() = 0;
		virtual void negativeAction() = 0;
	protected:
		CgnsFileInputConditionWidget* m_target;
	};

	class Condition {
	protected:
		Condition(CgnsFileInputConditionDependency* d);
	public:
		virtual ~Condition(){}
		virtual bool match() = 0;
		friend class CgnsFileInputConditionDependencyBuilder;
	};
	class ActionEnable : public Action {
	public:
		/// Constructor
		ActionEnable(CgnsFileInputConditionWidget* w) : Action(w){}
		~ActionEnable(){}
		void positiveAction(){
			m_target->setDisabled(false);
		}
		void negativeAction(){
			m_target->setDisabled(true);
		}
	};
public:
	/// Constructor
	CgnsFileInputConditionDependency();
	void setCondition(Condition* c){
		m_condition = c;
	}
	void addAction(Action* a){
		m_actions.push_back(a);
	}
	/// Build Action object and return the pointer to it.
	static Action* buildAction(
		const QDomNode& node,
		CgnsFileInputConditionContainerSet* cs,
		CgnsFileInputConditionWidgetSet* ws,
		CgnsFileInputConditionWidget* w
	    );
	static Condition* buildCondition(
		const QDomNode& node,
		CgnsFileInputConditionContainerSet* cs,
		CgnsFileInputConditionDependency * d
	    );

public slots:
	void check(){
		if (m_condition->match()){
			for (auto it = m_actions.begin(); it != m_actions.end(); ++it){
				(*it)->positiveAction();
			}
		}else{
			for (auto it = m_actions.begin(); it != m_actions.end(); ++it){
				(*it)->negativeAction();
			}
		}
	}

private:
	Condition* m_condition;
	QList<Action*> m_actions;
};

#endif // CGNSFILEINPUTCONDITIONDEPENDENCY_H
