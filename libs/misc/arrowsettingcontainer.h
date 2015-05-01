#ifndef ARROWSETTINGCONTAINER_H
#define ARROWSETTINGCONTAINER_H

#include "misc_global.h"
#include "xmlattributecontainer.h"

/// Container for arrow shape settings
class MISCDLL_EXPORT ArrowSettingContainer : public XmlAttributeContainer
{
	static const int DEFAULT_ARROWSIZE;
	static const int DEFAULT_LINEWIDTH;

public:
	ArrowSettingContainer();
	virtual ~ArrowSettingContainer() {}
	/// The size of cone of arrow (in pixels)
	int arrowSize() const {return m_arrowSize;}
	/// The width of arrow axis (in pixels)
	int lineWidth() const {return m_lineWidth;}
	void setArrowSize(int size) {m_arrowSize = size;}
	void setLineWidth(int width) {m_lineWidth = width;}

	void load(const QDomNode&);
	void save(QXmlStreamWriter&) const;

private:
	int m_arrowSize;
	int m_lineWidth;
};

#endif // ARROWSETTINGCONTAINER_H
