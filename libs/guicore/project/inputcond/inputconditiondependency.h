#ifndef INPUTCONDITIONDEPENDENCY_H
#define INPUTCONDITIONDEPENDENCY_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"
#include <QList>
#include <QObject>

class QDomNode;
class InputConditionWidgetSet;
class InputConditionContainer;
class InputConditionContainerSet;
class InputConditionDependencyBuilder;

/// This class holds the dependencies between input conditions.
class GUICOREDLL_EXPORT InputConditionDependency : public QObject
{
	Q_OBJECT

public:

	class Action
	{

	protected:
		Action(InputConditionWidget* w);

	public:
		virtual ~Action();
		virtual void positiveAction() = 0;
		virtual void negativeAction() = 0;

	protected:
		InputConditionWidget* m_target;
	};

	class Condition
	{

	protected:
		Condition(InputConditionDependency* d);

	public:
		virtual ~Condition();
		virtual bool match() = 0;
		friend class InputConditionDependencyBuilder;
	};

	class ActionEnable : public Action
	{
	public:
		ActionEnable(InputConditionWidget* w);
		~ActionEnable();

		void positiveAction() override;
		void negativeAction() override;
	};

public:
	InputConditionDependency(QObject* parent = 0);
	void setCondition(Condition* c);
	void addAction(Action* a);

	/// Build Action object and return the pointer to it.
	static Action* buildAction(
		const QDomNode& node,
		InputConditionContainerSet* cs,
		InputConditionWidgetSet* ws,
		InputConditionWidget* w
	);
	static Condition* buildCondition(
		const QDomNode& node,
		InputConditionContainerSet* cs,
		InputConditionDependency* d
	);

public slots:
	virtual void check();

private:
	Condition* m_condition;
	QList<Action*> m_actions;
};

#endif // INPUTCONDITIONDEPENDENCY_H
