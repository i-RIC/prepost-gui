#ifndef SVGSNAPSHOTENABLEDWINDOW_H
#define SVGSNAPSHOTENABLEDWINDOW_H

#include "../guicore_global.h"

class QSvgGenerator;

/// This class is the abstract class of windows those support
/// saving snapshot of the window as SVG.
class SvgSnapshotEnabledWindow
{
public:
	/// Render itself into the QSvgGenerator instance handled.
	virtual renderSvg(QSvgGenerator* generator) = 0;
};

#endif // SVGSNAPSHOTENABLEDWINDOW_H
