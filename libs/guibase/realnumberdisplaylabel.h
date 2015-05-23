#ifndef REALNUMBERDISPLAYLABEL_H
#define REALNUMBERDISPLAYLABEL_H

#include "guibase_global.h"

#include <QLabel>

/// Label to show real number.
class GUIBASEDLL_EXPORT RealNumberDisplayLabel : public QLabel
{
	Q_OBJECT

public:
	/// Display mode in case data is cleared
	enum ClearMode {
		cmEmpty,    ///< Display nothing
		cmLines,    ///< Display lines, like "-----"
	};
	explicit RealNumberDisplayLabel(QWidget* parent = nullptr);
	/// The clear mode
	ClearMode clearMode() {return m_clearMode;}
	/// Set the clear mode
	void setClearMode(ClearMode cm) {m_clearMode = cm;}

public slots:
	/// Set the value to display
	void setValue(double value);
	/// Clear the value
	void clear();

private:
	ClearMode m_clearMode;
};

#endif // REALNUMBERDISPLAYLABEL_H
