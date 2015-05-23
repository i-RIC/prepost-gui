#ifndef INTEGERNUMBERDISPLAYLABEL_H
#define INTEGERNUMBERDISPLAYLABEL_H

#include "guibase_global.h"

#include <QLabel>

/// Label to show integer number
class GUIBASEDLL_EXPORT IntegerNumberDisplayLabel : public QLabel
{
	Q_OBJECT

public:
	/// Display mode in case data is cleared
	enum ClearMode {
		cmEmpty,    ///< Display nothing
		cmLines     ///< Display lines, like "-----"
	};
	explicit IntegerNumberDisplayLabel(QWidget* parent = nullptr);
	/// The clear mode
	ClearMode clearMode() const {return m_clearMode;}
	/// Set the clear mode
	void setClearMode(ClearMode cm) {m_clearMode = cm;}

public slots:
	/// Set the value to display
	void setValue(unsigned int value);
	/// Clear the value
	void clear();

private:
	ClearMode m_clearMode;
};

#endif // INTEGERNUMBERDISPLAYLABEL_H
