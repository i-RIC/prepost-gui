#ifndef VTKTEXTPROPERTYSETTINGCONTAINER_H
#define VTKTEXTPROPERTYSETTINGCONTAINER_H

#include "guibase_global.h"
#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/compositecontainer.h>

class QColor;
class vtkTextProperty;

/// Container class for vtkTextProperty setting
class GUIBASEDLL_EXPORT vtkTextPropertySettingContainer : public CompositeContainer
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
	/// Copy constructor
	vtkTextPropertySettingContainer(const vtkTextPropertySettingContainer& c);
	/// Destructor
	virtual ~vtkTextPropertySettingContainer() {}
	/// Copy operator
	vtkTextPropertySettingContainer& operator=(const vtkTextPropertySettingContainer& c);

	/// The font family
	FontFamily fontFamily() const {return m_fontFamily;}
	/// Set the font family
	void setFontFamily(FontFamily ff) {m_fontFamily = ff;}
	/// The font color
	QColor fontColor() const;
	/// Set the font color
	void setFontColor(const QColor& c);
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

	/// Get setting from vtkTextProperty instance
	void getSetting(vtkTextProperty* prop);
	/// Apply setting to vtkTextProperty instance
	void applySetting(vtkTextProperty* prop);

private:
	EnumContainerT<FontFamily> m_fontFamily;
	ColorContainer m_fontColor;
	IntContainer m_fontSize;
	BoolContainer m_isBold;
	BoolContainer m_isItalic;
	BoolContainer m_isShadow;
};

#endif // VTKTEXTPROPERTYSETTINGCONTAINER_H
