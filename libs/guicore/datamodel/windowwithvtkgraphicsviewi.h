#ifndef WINDOWWITHVTKGRAPHICSVIEWI_H
#define WINDOWWITHVTKGRAPHICSVIEWI_H

class VTKGraphicsView;

class WindowWithVtkGraphicsViewI
{
public:
	virtual ~WindowWithVtkGraphicsViewI() {}

	virtual VTKGraphicsView* getVtkGraphicsView() const = 0;
};

#endif // WINDOWWITHVTKGRAPHICSVIEWI_H
