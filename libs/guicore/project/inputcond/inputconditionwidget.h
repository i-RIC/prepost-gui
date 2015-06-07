#ifndef CGNSFILEINPUTCONDITIONWIDGET_H
#define CGNSFILEINPUTCONDITIONWIDGET_H

#include "../../guicore_global.h"

#include <QList>
#include <QWidget>
#include <QDomNode>
#include "inputconditioncontainer.h"

class InputConditionDependency;

class GUICOREDLL_EXPORT InputConditionWidget : public QWidget
{
	Q_OBJECT

public:
	/// Constructor
	InputConditionWidget(QDomNode /*n*/) {
		m_dependency = nullptr;
	}
	virtual ~InputConditionWidget() {}
	void setDependency(InputConditionDependency* dep);
	InputConditionDependency* dependency() const {return m_dependency;}
	virtual void setMaximum(const QVariant& /*value*/) {}
	virtual void setMinimum(const QVariant& /*value*/) {}
	static const int margin = 0;
	static QList<QDomNode> getEnums(QDomNode defNode);
	static bool hasEnums(QDomNode defNode);

protected:
	InputConditionDependency* m_dependency;
};
#endif // CGNSFILEINPUTCONDITIONWIDGET_H
