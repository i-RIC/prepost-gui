#ifndef WINDOWGEOMETRYCONTAINER_H
#define WINDOWGEOMETRYCONTAINER_H

#include "misc_global.h"
#include "xmlattributecontainer.h"

class QWidget;

class MISCDLL_EXPORT WindowGeometryContainer : public XmlAttributeContainer
{
public:
	WindowGeometryContainer();
	WindowGeometryContainer(QWidget *w, bool ignoreMax = false);
	~WindowGeometryContainer() {}

	void load(const QDomNode&) override;
	void save(QXmlStreamWriter&) const override;

	void setWidget(QWidget* w);
	void setIgnoreMax(bool ignoreMax);

private:
	WindowGeometryContainer(const WindowGeometryContainer&) = delete;
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) = delete;

	bool m_ignoreMax;
	QWidget* m_window;
};

#endif // WINDOWGEOMETRYCONTAINER_H
