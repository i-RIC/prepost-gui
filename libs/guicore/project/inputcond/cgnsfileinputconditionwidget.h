#ifndef CGNSFILEINPUTCONDITIONWIDGET_H
#define CGNSFILEINPUTCONDITIONWIDGET_H

#include "../../guicore_global.h"

#include <QList>
#include <QWidget>
#include <QDomNode>
#include "cgnsfileinputconditioncontainer.h"

class CgnsFileInputConditionDependency;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidget : public QWidget
{
	Q_OBJECT
public:
	/// Constructor
	CgnsFileInputConditionWidget(QDomNode /*n*/) {
		m_dependency = nullptr;
	}
	virtual ~CgnsFileInputConditionWidget() {}
	void setDependency(CgnsFileInputConditionDependency* dep);
	CgnsFileInputConditionDependency* dependency() const {return m_dependency;}
	virtual void setMaximum(const QVariant& /*value*/) {}
	virtual void setMinimum(const QVariant& /*value*/) {}
	static const int margin = 0;
	static QList<QDomNode> getEnums(QDomNode defNode);
	static bool hasEnums(QDomNode defNode);

protected:
	CgnsFileInputConditionDependency* m_dependency;
};
#endif // CGNSFILEINPUTCONDITIONWIDGET_H
