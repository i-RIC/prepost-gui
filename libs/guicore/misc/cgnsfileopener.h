#ifndef CGNSFILEOPENER_H
#define CGNSFILEOPENER_H

#include "../guicore_global.h"

#include <string>

class GUICOREDLL_EXPORT CgnsFileOpener
{
public:
	CgnsFileOpener(const std::string& filename, int openMode);
	~CgnsFileOpener();

	int fileId() const;

private:
	class Impl;
	Impl* impl;
};

#endif // CGNSFILEOPENER_H
