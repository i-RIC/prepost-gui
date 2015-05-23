#ifndef VTKTEXTPROPERTYSETTINGCONTAINER_H
#define VTKTEXTPROPERTYSETTINGCONTAINER_H

#include "guibase_global.h"
#include <misc/xmlattributecontainer.h>
#include <QColor>

class vtkTextProperty;

/// Container class for vtkTextProperty setting
class GUIBASEDLL_EXPORT vtkTextPropertySettingContainer : public XmlAttributeContainer
{
public:
	/// Font family
	enum FontFamily {
		ffArial   = 0,   ///< Arial
		ffCourier = 1,   ///< Courier
		ffTimes   = 2    ///< Times
	};
	/// Constructor
	vtkTextPropertySettingContainer();
	virtual ~vtkTextPropertySettingContainer() {}

	/// The font family
	FontFamily fontFamily() const {return m_fontFamily;}
	/// Set the font family
	void setFontFamily(FontFamily ff) {m_fontFamily = ff;}
	/// The font color
	const QColor& fontColor() const {return m_fontColor;}
	/// Set the font color
	void setFontColor(const QColor& c) {m_fontColor = c;}
	/// The font size (in pixels)
	int fontSize() const {return m_fontSize;}
	/// Set the font size (in pixels)
	void setFontSize(int size) {m_fontSize = size;}
	/// The font bold flag
	bool isBold() const {return m_isBold;}
	/// Set the font bold flag
	void setBold(bool bold) {m_isBold = bold;}
	/// The font italic flag
	bool isItalic() const {return m_isItalic;}
	/// Set the font italic flag
	void setItalic(bool italic) {m_isItalic = italic;}
	/// The font shadow flag
	bool isShadow() const {return m_isShadow;}
	/// Set the font shadow flag
	void setShadow(bool shadow) {m_isShadow = shadow;}

	/// Load setting from XML attributes
	void load(const QDomNode&);
	/// Save setting to XML attributes
	void save(QXmlStreamWriter&) const;

	/// Get setting from vtkTextProperty instance
	void getSetting(vtkTextProperty* prop);
	/// Apply setting to vtkTextProperty instance
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
