#ifndef SVGSNAPSHOTENABLEDWINDOWINTERFACE_H
#define SVGSNAPSHOTENABLEDWINDOWINTERFACE_H

#include "../guicore_global.h"

class QSvgGenerator;

/// This class is the abstract class of windows those support
/// saving snapshot of the window as SVG.
class SvgSnapshotEnabledWindowInterface
{
public:
	/// Render itself into the QSvgGenerator instance handled.
	virtual renderSvg(QSvgGenerator* generator) = 0;
};

#endif // SVGSNAPSHOTENABLEDWINDOWINTERFACE_H
