#ifndef __vtkCustomScalarBarActor_h
#define __vtkCustomScalarBarActor_h

#include "guibase_global.h"

#include <vtkScalarBarActor.h>

class GUIBASEDLL_EXPORT vtkCustomScalarBarActor : public vtkScalarBarActor
{
public:
	static vtkCustomScalarBarActor* New();

protected:
	void ConfigureScalarBar() override;

protected:
	vtkCustomScalarBarActor();

private:
	vtkCustomScalarBarActor(const vtkCustomScalarBarActor&);  // Not implemented.
	void operator=(const vtkCustomScalarBarActor&);  // Not implemented.
};

#endif // __vtkCustomScalarBarActor_h
