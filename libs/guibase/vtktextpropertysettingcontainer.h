#ifndef VTKTEXTPROPERTYSETTINGCONTAINER_H
#define VTKTEXTPROPERTYSETTINGCONTAINER_H

#include "guibase_global.h"
#include <misc/xmlattributecontainer.h>
#include <QColor>

class vtkTextProperty;

class GUIBASEDLL_EXPORT vtkTextPropertySettingContainer : public XmlAttributeContainer
{
public:
	enum FontFamily {
		ffArial   = 0,
		ffCourier = 1,
		ffTimes   = 2
	};
	/// Constructor
	vtkTextPropertySettingContainer();
	virtual ~vtkTextPropertySettingContainer(){}

	FontFamily fontFamily() const {return m_fontFamily;}
	void setFontFamily(FontFamily ff){m_fontFamily = ff;}
	const QColor& fontColor() const {return m_fontColor;}
	void setFontColor(const QColor& c){m_fontColor = c;}
	int fontSize() const {return m_fontSize;}
	void setFontSize(int size){m_fontSize = size;}
	bool isBold() const {return m_isBold;}
	void setBold(bool bold){m_isBold = bold;}
	bool isItalic() const {return m_isItalic;}
	void setItalic(bool italic){m_isItalic = italic;}
	bool isShadow() const {return m_isShadow;}
	void setShadow(bool shadow){m_isShadow = shadow;}

	void load(const QDomNode &);
	void save(QXmlStreamWriter &) const;

	void getSetting(vtkTextProperty* prop);
	void applySetting(vtkTextProperty* prop);

private:
	FontFamily m_fontFamily;
	QColor m_fontColor;
	int m_fontSize;
	bool m_isBold;
	bool m_isItalic;
	bool m_isShadow;
};

#endif // VTKTEXTPROPERTYSETTINGCONTAINER_H
