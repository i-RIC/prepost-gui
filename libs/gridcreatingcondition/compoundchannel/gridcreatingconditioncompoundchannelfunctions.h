#ifndef GRIDCREATINGCONDITIONCREATORCOMPOUNDCHANNELFUNCTIONS_H
#define GRIDCREATINGCONDITIONCREATORCOMPOUNDCHANNELFUNCTIONS_H

class GridCreatingConditionCompoundChannelFunctions
{
public:
	GridCreatingConditionCompoundChannelFunctions() {}
	~GridCreatingConditionCompoundChannelFunctions() {}
	void generateGrid(int tns, double* tx1, double* ty1, double* tx2, double* ty2, double* tx3, double* ty3, double* tx4, double* ty4, double* txgrd, double* tygrd, double relax, int Iter, double* para3, int Ny_1, int Ny_2, int Ny_3);

private:
	bool allocate(int ns, int nn, int npart);
	void deallocate();
	void combine(int Nx, int Ny_1, int Ny_2, int Ny_3, int NyA);

	int offset(int i, int j) {return m_ns * (j - 1) + (i - 1);}
	int partOffset(int i, int j, int part) {return m_nn * m_ns * (part - 1) + m_ns * (j - 1) + (i - 1);}
	double& X(int i, int j) {return *(m_X + offset(i, j));}
	double& Y(int i, int j) {return *(m_Y + offset(i, j));}
	double& G11(int i, int j) {return *(m_G11 + offset(i, j));}
	double& G22(int i, int j) {return *(m_G22 + offset(i, j));}
	double& Xbb(int i, int j) {return *(m_Xbb + offset(i, j));}
	double& Ybb(int i, int j) {return *(m_Ybb + offset(i, j));}
	double& Xbbq(int i, int j) {return *(m_Xbbq + offset(i, j));}
	double& Ybbq(int i, int j) {return *(m_Ybbq + offset(i, j));}
	double& XNew(int i, int j) {return *(m_XNew + offset(i, j));}
	double& YNew(int i, int j) {return *(m_YNew + offset(i, j));}
	double& Xall(int i, int j) {return *(m_Xall + offset(i, j));}
	double& Yall(int i, int j) {return *(m_Yall + offset(i, j));}
	int& IL(int i) {return *(m_IL + i - 1);}
	int& IR(int i) {return *(m_IR + i - 1);}
	double& Xdiv(int i, int j, int part) {return *(m_Xdiv + partOffset(i, j, part));}
	double& Ydiv(int i, int j, int part) {return *(m_Ydiv + partOffset(i, j, part));}
	double& XdivF(int i, int j, int part) {return *(m_XdivF + partOffset(i, j, part));}
	double& YdivF(int i, int j, int part) {return *(m_YdivF + partOffset(i, j, part));}

	int m_ns;
	int m_nn;
	int m_npart;

	double* m_X;
	double* m_Y;
	double* m_G11;
	double* m_G22;
	double* m_Xbb;
	double* m_Ybb;
	double* m_Xbbq;
	double* m_Ybbq;
	double* m_XNew;
	double* m_YNew;
	double* m_Xall;
	double* m_Yall;

	int* m_IL;
	int* m_IR;
	double* m_Xdiv;
	double* m_Ydiv;
	double* m_XdivF;
	double* m_YdivF;
};

#endif // GRIDCREATINGCONDITIONCREATORCOMPOUNDCHANNELFUNCTIONS_H
