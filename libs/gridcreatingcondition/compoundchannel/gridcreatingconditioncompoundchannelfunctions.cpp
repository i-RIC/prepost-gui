#include "gridcreatingconditioncompoundchannelfunctions.h"
#include <cmath>
#include <QVector2D>

#define BIGDOUBLE 1.0E12;

void GridCreatingConditionCompoundChannelFunctions::generateGrid(int tns, double* tx1, double* ty1, double* tx2, double* ty2, double* tx3, double* ty3, double* tx4, double* ty4, double* txgrd, double* tygrd, double relax, int Iter, double* para3, int Ny_1, int Ny_2, int Ny_3)
{
	/*
	      T = 0
	      nx = tns
	!      Ny_1=5
	!      Ny_2=10
	!      Ny_3=3
	      Npart=3
	      NyA=Ny_1-1+Ny_2-1+Ny_3
	*/
	double T = 0;
	int Nx = tns;
	int Npart = 3;
	int NyA = Ny_1 - 1 + Ny_2 - 1 + Ny_3;

	/*
	      CALL alloc_GridGen(nx, NyA, Npart)
	*/
	// allocate memory.
	allocate(Nx, NyA, Npart);

	/*
	!      Open(100,File='XY1.csv',Status='Unknown')
	!      Open(101,File='XY2.csv',Status='Unknown')
	!      Open(102,File='XY3.csv',Status='Unknown')

	      Open(9110,File='GridAll.dat',Status='Unknown')

	      Open(141,File='XC.dat',Status='Unknown')
	      Open(151,File='YC.dat',Status='Unknown')

	      L=0
	      aL99=0.
	!      Nx3=38
	!      Read(100,*)
	!!      Do 805 I=1,Nx3  !change rmcd 5/1/09
	!      Do 805 I=1,NXS
	!     Read(100,*)Xbbq(I,Ny),Ybbq(I,Ny),Xbbq(I,1),Ybbq(I,1)
	!!      if(I.lt.43)Read(100,*)
	!  805 continue
	*/
	// open ignored.
	int L = 0;
	int aL99 = 0;

	/*
	      KK=0
	*/
	int KK = 0;
	/*
		9955 KK=KK+1
	*/

Label9955:
	KK = KK + 1;

	/*
	      if(KK.eq.1)then
	      Ny=Ny_1
	!      Read(100,*)
	!      Do I=1,Nx
	!      Read(100,*)tx2(I),ty2(I),tx1(I),ty1(I)
	!      enddo
	      elseif(KK.eq.2)then
	      Ny=Ny_2
	!      Read(101,*)
	!      Do I=1,Nx
	!      Read(101,*)tx3(I),ty3(I),tx2(I),ty2(I)
	!      enddo
	      else
	      Ny=Ny_3
	!      Read(102,*)
	!      Do I=1,Nx
	!      Read(102,*)tx4(I),ty4(I),tx3(I),ty3(I)
	!      enddo
	      endif
	*/
	int Ny;
	if (KK == 1) {
		Ny = Ny_1;
	} else if (KK == 2) {
		Ny = Ny_2;
	} else {
		Ny = Ny_3;
	}

	/*
	      Do I=1,Nx  !change rmcd 5/1/09 to allow for end points to
	        iii = 5
	      if(KK.eq.1)then
	        Xbb(I,Ny) = tx2(i) !be passed in the subroutine
	        Ybb(I,NY) = ty2(i)
	        Xbb(I,1) = tx1(i)
	        Ybb(I,1) = ty1(i)
	      elseif(KK.eq.2)then
	        Xbb(I,Ny) = tx3(i) !be passed in the subroutine
	        Ybb(I,NY) = ty3(i)
	        Xbb(I,1) = tx2(i)
	        Ybb(I,1) = ty2(i)
	      else
	        Xbb(I,Ny) = tx4(i) !be passed in the subroutine
	        Ybb(I,NY) = ty4(i)
	        Xbb(I,1) = tx3(i)
	        Ybb(I,1) = ty3(i)
	      endif
	      ENDDO

	!      Iq=1
	!      L=1
	!      Do 9805 I=1,Nx
	!      if(L.eq.5)then
	!      L=1
	!      Iq=Iq+1
	!      endif
	!      Xbb(I,Ny)=Xbbq(Iq,Ny)+(Xbbq(Iq+1,Ny)-Xbbq(Iq,Ny))*((L-1)/4.)
	!      Ybb(I,Ny)=Ybbq(Iq,Ny)+(Ybbq(Iq+1,Ny)-Ybbq(Iq,Ny))*((L-1)/4.)
	!      Xbb(I,1)=Xbbq(Iq,1)+(Xbbq(Iq+1,1)-Xbbq(Iq,1))*((L-1)/4.)
	!      Ybb(I,1)=Ybbq(Iq,1)+(Ybbq(Iq+1,1)-Ybbq(Iq,1))*((L-1)/4.)
	!      L=L+1
	!!c      write(*,*)Xbb(I,Ny),Xbbq(Iq,Ny),I,Iq,L
	!!c      pause
	! 9805 continue
	*/
	for (int I = 1; I <= Nx; ++I) {
		// int iii = 5; // not used.
		if (KK == 1) {
			Xbb(I, Ny) = *(tx2 + I - 1);
			Ybb(I, Ny) = *(ty2 + I - 1);
			Xbb(I, 1) = *(tx1 + I - 1);
			Ybb(I, 1) = *(ty1 + I - 1);
		} else if (KK == 2) {
			Xbb(I, Ny) = *(tx3 + I - 1);
			Ybb(I, Ny) = *(ty3 + I - 1);
			Xbb(I, 1) = *(tx2 + I - 1);
			Ybb(I, 1) = *(ty2 + I - 1);
		} else {
			Xbb(I, Ny) = *(tx4 + I - 1);
			Ybb(I, Ny) = *(ty4 + I - 1);
			Xbb(I, 1) = *(tx3 + I - 1);
			Ybb(I, 1) = *(ty3 + I - 1);
		}
	}
	/*
	      Do 1815 I=Nx,1,-1
	      Xbb(I,1)=Xbb(I,1)
	      Ybb(I,1)=Ybb(I,1)
	      Xbb(I,Ny)=Xbb(I,Ny)
	      Ybb(I,Ny)=Ybb(I,Ny)
	 1815 continue
	*/
	/*
		// NEEDLESS CODE.
		for (int I = Nx; I >= 1; --I){
			Xbb(I, 1) = Xbb(I, 1);
			Ybb(I, 1) = Ybb(I, 1);
			Xbb(I, Ny) = Xbb(I, Ny);
			Ybb(I, Ny) = Ybb(I, Ny);
		}
	*/
	/*
	  913 Format(13x,4F13.5)
	      Do 8051 I=1,Nx
	      if(I.eq.1.or.I.eq.Nx)then
	      X(I,1)=Xbb(I,1)
	      Y(I,1)=Ybb(I,1)
	      X(I,Ny)=Xbb(I,Ny)
	      Y(I,Ny)=Ybb(I,Ny)
	      else
	      X(I,1)=Xbb(I,1)-(Xbb(I,1)-Xbb(I-1,1))*0.1E-8
	      Y(I,1)=Ybb(I,1)-(Ybb(I,1)-Ybb(I-1,1))*0.1E-8
	      X(I,Ny)=Xbb(I,Ny)-(Xbb(I,Ny)-Xbb(I-1,Ny))*0.1E-8
	      Y(I,Ny)=Ybb(I,Ny)-(Ybb(I,Ny)-Ybb(I-1,Ny))*0.1E-8
	      endif
	      IL(I)=I
	      Ir(I)=I
	 8051 continue
	*/
	for (int I = 1; I <= Nx; ++I) {
		if (I == 1 || I == Nx) {
			X(I, 1) = Xbb(I, 1);
			Y(I, 1) = Ybb(I, 1);
			X(I, Ny) = Xbb(I, Ny);
			Y(I, Ny) = Ybb(I, Ny);
		} else {
			X(I, 1) = Xbb(I, 1) - (Xbb(I, 1) - Xbb(I - 1, 1)) * 0.1E-8;
			Y(I, 1) = Ybb(I, 1) - (Ybb(I, 1) - Ybb(I - 1, 1)) * 0.1E-8;
			X(I, Ny) = Xbb(I, Ny) - (Xbb(I, Ny) - Xbb(I - 1, Ny)) * 0.1E-8;
			Y(I, Ny) = Ybb(I, Ny) - (Ybb(I, Ny) - Ybb(I - 1, Ny)) * 0.1E-8;
		}
		IL(I) = I;
		IR(I) = I;
	}
	/*
	!*---- Initial conditions ----*
	      Do 806 I=1,Nx
	      Xstep=(X(I,Ny)-X(I,1))/(Ny-1)
	      Ystep=(Y(I,Ny)-Y(I,1))/(Ny-1)
	      Do 806 J=2,Ny-1
	!c      if(J.eq.2)then
	!c      X(I,J)=X(I,J-1)+Xstep/2.
	!c      Y(I,J)=Y(I,J-1)+Ystep/2.
	!c      else
	      X(I,J)=X(I,J-1)+Xstep
	      Y(I,J)=Y(I,J-1)+Ystep
	!c      endif
	  806 continue

	!c      go to 230

	*/
	// ---- Initial conditions ----
	for (int I = 1; I <= Nx; ++I) {
		double Xstep = (X(I, Ny) - X(I, 1)) / (Ny - 1);
		double Ystep = (Y(I, Ny) - Y(I, 1)) / (Ny - 1);
		for (int J = 2; J <= Ny - 1; ++J) {
			X(I, J) = X(I, J - 1) + Xstep;
			Y(I, J) = Y(I, J - 1) + Ystep;
		}
	}
	/*
	 235  Xmax=-1e12
	      Ymax=-1e12
	      Do 220 I=3,Nx-3
	      Do 220 J=2,Ny-1
	      Xfl=(X(I,J)+X(I+1,J)+X(I,J+1)+X(I+1,J+1))/4.
	      Xfr=(X(I,J)+X(I+1,J)+X(I,J-1)+X(I+1,J-1))/4.
	      Xbl=(X(I,J)+X(I-1,J)+X(I,J+1)+X(I-1,J+1))/4.
	      Xbr=(X(I,J)+X(I-1,J)+X(I,J-1)+X(I-1,J-1))/4.
	      Yfl=(Y(I,J)+Y(I+1,J)+Y(I,J+1)+Y(I+1,J+1))/4.
	      Yfr=(Y(I,J)+Y(I+1,J)+Y(I,J-1)+Y(I+1,J-1))/4.
	      Ybl=(Y(I,J)+Y(I-1,J)+Y(I,J+1)+Y(I-1,J+1))/4.
	      Ybr=(Y(I,J)+Y(I-1,J)+Y(I,J-1)+Y(I-1,J-1))/4.
	      G1f=((X(I+1,J)-X(I,J))**2.+(Y(I+1,J)-Y(I,J))**2.)**0.5
	      G1b=((X(I,J)-X(I-1,J))**2.+(Y(I,J)-Y(I-1,J))**2.)**0.5
	      G1l=((Xfl-Xbl)**2.+(Yfl-Ybl)**2.)**0.5
	      G1r=((Xfr-Xbr)**2.+(Yfr-Ybr)**2.)**0.5
	      G2f=((Xfl-Xfr)**2.+(Yfl-Yfr)**2.)**0.5
	      G2b=((Xbl-Xbr)**2.+(Ybl-Ybr)**2.)**0.5
	      G2l=((X(I,J+1)-X(I,J))**2.+(Y(I,J+1)-Y(I,J))**2.)**0.5
	      G2r=((X(I,J)-X(I,J-1))**2.+(Y(I,J)-Y(I,J-1))**2.)**0.5
	      Af=G2f/G1f
	      Ab=G2b/G1b
	      Al=G1l/G2l
	      Ar=G1r/G2r
	!ChangeS
	      Xf=(X(I,J)+X(I+1,J))/2.
	      Xb=(X(I,J)+X(I-1,J))/2.
	      Xl=(X(I,J)+X(I,J+1))/2.
	      Xr=(X(I,J)+X(I,J-1))/2.
	      Yf=(Y(I,J)+Y(I+1,J))/2.
	      Yb=(Y(I,J)+Y(I-1,J))/2.
	      Yl=(Y(I,J)+Y(I,J+1))/2.
	      Yr=(Y(I,J)+Y(I,J-1))/2.
	      G1=((Xf-Xb)**2.+(Yf-Yb)**2.)**0.5
	      G2=((Xl-Xr)**2.+(Yl-Yr)**2.)**0.5
	      Afy=G2/G1
	      Aby=G2/G1
	      Aly=G1/G2
	      Ary=G1/G2
	      Aa=0.0000001
	      Cc=0.8
	      if(I.gt.0.)then
	      sgnp=1.
	      elseif(I.eq.0)then
	      sgnp=0.
	      else
	      sgnp=-1
	      endif
	      if(J-5.gt.0.)then
	      sgnq=1.
	      elseif(J-5.eq.0)then
	      sgnq=0.
	      else
	      sgnq=-1
	      endif
	!c      if(I.ge.64.and.I.le.164)then
	!c      Pp=Aa*exp(-Cc)*0.05*0.
	!c      elseif(I.lt.64)then
	!c      Pp=Aa*exp(-Cc*abs(I-64))-Aa*exp(-Cc*abs(I-Nx))
	!c      elseif(I.gt.164)then
	!c      Pp=-Aa*exp(-Cc*abs(I-164))
	!c      endif
	      Pp=0.
	!c      Qq=-Aa*sgnq*exp(-Cc*abs(J-5))*0.0005
	      Qq=0.
	!c      Aa=1.
	!c      Cc=1.
	!c      if(X(I,J)-800..gt.0.)then
	!c      sgn=1.
	!c      elseif(X(I,J)-800.eq.0.)then
	!c      sgn=0.
	!c      else
	!c      sgn=-1
	!c      endif
	!c      Pp=-Aa*sgn*exp(-Cc*abs(X(I,J)-800.))*0.00001
	!c      Qq=0.
	      Tra=(Xf-Xb)*(Yl-Yr)-(Xl-Xr)*(Yf-Yb)
	      Bx=Tra**2.*(Pp*(Xf-Xb)+Qq*(Xl-Xr))
	      By=Tra**2.*(Pp*(Yf-Yb)+Qq*(Yl-Yr))
	      Xnew1=(Afy*X(I+1,J)+Aby*X(I-1,J)+Aly*X(I,J+1)+Ary*X(I,J-1)+Bx)&
	      /(Afy+Aby+Aly+Ary)
	      Ynew1=(Afy*Y(I+1,J)+Aby*Y(I-1,J)+Aly*Y(I,J+1)+Ary*Y(I,J-1)+By)&
	     /(Afy+Aby+Aly+Ary)
	      Para1=relax
	      Para2=1.-Para1
	      Xnew1=(Xnew1*Para1+X(I,J)*Para2)
	      Ynew1=(Ynew1*Para1+Y(I,J)*Para2)
	!c      Xnew1=(Af*X(I+1,J)+Ab*X(I-1,J)+Al*X(I,J+1)+Ar*X(I,J-1))
	!c     &/(Af+Ab+Al+Ar)
	!c      Ynew1=(Af*Y(I+1,J)+Ab*Y(I-1,J)+Al*Y(I,J+1)+Ar*Y(I,J-1))
	!c     &/(Af+Ab+Al+Ar)
	!ChangeE
	!c      Xnew=X(I,J)+(Xnew1-X(I,J))*0.2
	!c      Ynew=Y(I,J)+(Ynew1-Y(I,J))*0.2
	      Xe=Dabs(Xnew1-X(I,J))
	      Ye=Dabs(Ynew1-Y(I,J))
	      Xmax=Dmax1(Xe,Xmax)
	      Ymax=Dmax1(Ye,Ymax)
	      X(I,J)=Xnew1
	      Y(I,J)=Ynew1
	  220 Continue
	*/
Label235:

	double Xmax = - BIGDOUBLE;
	double Ymax = - BIGDOUBLE;
	for (int I = 3; I <= Nx - 3; ++I) {
		for (int J = 2; J <= Ny - 1; ++J) {
			double Xfl = (X(I, J) + X(I + 1, J) + X(I, J + 1) + X(I + 1, J + 1)) / 4;
			double Xfr = (X(I, J) + X(I + 1, J) + X(I, J - 1) + X(I + 1, J - 1)) / 4;
			double Xbl = (X(I, J) + X(I - 1, J) + X(I, J + 1) + X(I - 1, J + 1)) / 4;
			double Xbr = (X(I, J) + X(I - 1, J) + X(I, J - 1) + X(I - 1, J - 1)) / 4;

			double Yfl = (Y(I, J) + Y(I + 1, J) + Y(I, J + 1) + Y(I + 1, J + 1)) / 4;
			double Yfr = (Y(I, J) + Y(I + 1, J) + Y(I, J - 1) + Y(I + 1, J - 1)) / 4;
			double Ybl = (Y(I, J) + Y(I - 1, J) + Y(I, J + 1) + Y(I - 1, J + 1)) / 4;
			double Ybr = (Y(I, J) + Y(I - 1, J) + Y(I, J - 1) + Y(I - 1, J - 1)) / 4;

			double G1f = (QVector2D(X(I + 1, J), Y(I + 1, J)) - QVector2D(X(I, J), Y(I, J))).length();
			double G1b = (QVector2D(X(I - 1, J), Y(I - 1, J)) - QVector2D(X(I, J), Y(I, J))).length();
			double G1l = (QVector2D(Xfl, Yfl) - QVector2D(Xbl, Ybl)).length();
			double G1r = (QVector2D(Xfr, Yfr) - QVector2D(Xbr, Ybr)).length();
			double G2f = (QVector2D(Xfl, Yfl) - QVector2D(Xfr, Yfr)).length();
			double G2b = (QVector2D(Xbl, Ybl) - QVector2D(Xbr, Ybr)).length();
			double G2l = (QVector2D(X(I, J + 1), Y(I, J + 1)) - QVector2D(X(I, J), Y(I, J))).length();
			double G2r = (QVector2D(X(I, J - 1), Y(I, J - 1)) - QVector2D(X(I, J), Y(I, J))).length();

			double Af = G2f / G1f;
			double Ab = G2b / G1b;
			double Al = G1l / G2l;
			double Ar = G1r / G2r;

			double Xf = (X(I, J) + X(I + 1, J)) / 2;
			double Xb = (X(I, J) + X(I - 1, J)) / 2;
			double Xl = (X(I, J) + X(I, J + 1)) / 2;
			double Xr = (X(I, J) + X(I, J - 1)) / 2;
			double Yf = (Y(I, J) + Y(I + 1, J)) / 2;
			double Yb = (Y(I, J) + Y(I - 1, J)) / 2;
			double Yl = (Y(I, J) + Y(I, J + 1)) / 2;
			double Yr = (Y(I, J) + Y(I, J - 1)) / 2;

			double G1 = (QVector2D(Xf, Yf) - QVector2D(Xb, Yb)).length();
			double G2 = (QVector2D(Xl, Yl) - QVector2D(Xr, Yr)).length();
			double Afy = G2 / G1;
			double Aby = G2 / G1;
			double Aly = G1 / G2;
			double Ary = G1 / G2;

			double Aa = 0.0000001;
			double Cc = 0.8;
			int sgnp;
			int sgnq;
			if (I > 0) {
				sgnp = 1;
			} else if (I == 0) {
				sgnp = 0;
			} else {
				sgnp = - 1;
			}
			if (J - 5 > 0) {
				sgnq = 1;
			} else if (J - 5 == 0) {
				sgnq = 0;
			} else {
				sgnq = - 1;
			}
			double Pp = 0;
			double Qq = 0;

			double Tra = (Xf - Xb) * (Yl - Yr) - (Xl - Xr) * (Yf - Yb);
			double Bx = Tra * Tra * (Pp * (Xf - Xb) + Qq * (Xl - Xr));
			double By = Tra * Tra * (Pp * (Yf - Yb) + Qq * (Yl - Yr));

			double Xnew1 = (Afy * X(I + 1, J) + Aby * X(I - 1, J) + Aly * X(I, J + 1) + Ary * X(I, J - 1) + Bx)
										 / (Afy + Aby + Aly + Ary);
			double Ynew1 = (Afy * Y(I + 1, J) + Aby * Y(I - 1, J) + Aly * Y(I, J + 1) + Ary * Y(I, J - 1) + By)
										 / (Afy + Aby + Aly + Ary);
			double Para1 = relax;
			double Para2 = 1 - Para1;

			Xnew1 = Xnew1 * Para1 + X(I, J) * Para2;
			Ynew1 = Ynew1 * Para1 + Y(I, J) * Para2;

			double Xe = qAbs(Xnew1 - X(I, J));
			double Ye = qAbs(Ynew1 - Y(I, J));
			Xmax = qMax(Xe, Xmax);
			Ymax = qMax(Ye, Ymax);
			X(I,J) = Xnew1;
			Y(I,J) = Ynew1;
		}
	}
	/*
		  T=T+1.
		  if(T.ge.50)then
	!c      write(*,*)Xmax/0.02,Ymax/0.02
		  T=0.
		  endif
		  if(Xmax/0.02.le.0.1e-2.and.Ymax/0.02.le.0.1e-2)then
		  if(L.lt.Iter)then
	!c      if(L.lt.1)then
		  Xbou2=X(2,Ny)
		  Ybou2=Y(2,Ny)
		  do 1001 I=3,Nx-2
	!c      do 1001 I=3,Nx-1
	 2001 D1=Dsqrt((X(I,Ny)-X(I-1,Ny))**2.+(Y(I,Ny)-Y(I-1,Ny))**2.)
		  D2=Dsqrt((X(I,Ny-1)-X(I-1,Ny))**2.+(Y(I,Ny-1)-Y(I-1,Ny))**2.)
		  SS=Dsqrt((X(I,Ny-1)-X(I,Ny))**2.+(Y(I,Ny-1)-Y(I,Ny))**2.)
		  U1=Dsqrt((X(I,Ny)-X(I+1,Ny))**2.+(Y(I,Ny)-Y(I+1,Ny))**2.)
		  U2=Dsqrt((X(I,Ny-1)-X(I+1,Ny))**2.+(Y(I,Ny-1)-Y(I+1,Ny))**2.)
		  Dangle=Dacos((D1**2.+SS**2.-D2**2.)/(2.*D1*SS))
		  Uangle=Dacos((U1**2.+SS**2.-U2**2.)/(2.*U1*SS))
		  difDU=abs(Dangle-Uangle)
		  if(difDU.lt.0.1e-5)then
		  Xbou1=X(I,Ny)
		  Ybou1=Y(I,Ny)
		  go to 1001
		  else
		  Aveangle=(Dangle+Uangle)/2.
		  if(Dangle.lt.Uangle)then
		  Sangle=3.141592-Dangle-Aveangle
		  Dmod=Dabs((SS*dcos(Aveangle)*dcos(Sangle)+SS*dcos(Dangle))&
		 /(1.-dcos(Aveangle)*dcos(Aveangle)))
		  if(Dmod.gt.D1*0.9)Dmod=D1*0.9
		  Xbou1=X(I,Ny)-(X(I,Ny)-X(I-1,Ny))*Dmod/D1
		  Ybou1=Y(I,Ny)-(Y(I,Ny)-Y(I-1,Ny))*Dmod/D1
		  Abb=(Ybb(Il(I),Ny)-Ybb(Il(I)-1,Ny))&
		 /(Xbb(Il(I),Ny)-Xbb(Il(I)-1,Ny))
		  Bbb=Ybb(Il(I)-1,Ny)-Abb*Xbb(Il(I)-1,Ny)
		  Anew=(Y(I,Ny-1)-Ybou1)/(X(I,Ny-1)-Xbou1)
		  Bnew=Y(I,Ny-1)-Anew*X(I,Ny-1)
		  Xbou1=-(Bbb-Bnew)/(Abb-Anew)
		  Ybou1=Anew*Xbou1+Bnew
		  D1bb=Dsqrt((X(I,Ny)-Xbb(Il(I)-1,Ny))**2.&
		 +(Y(I,Ny)-Ybb(Il(I)-1,Ny))**2.)
		  D1new=Dsqrt((X(I,Ny)-Xbou1)**2.+(Y(I,Ny)-Ybou1)**2.)
		  if(D1new.gt.D1bb)then
		  Xbou1=Xbb(Il(I)-1,Ny)-(Xbb(Il(I),Ny)-Xbb(Il(I)-1,Ny))*0.1E-8
		  Ybou1=Ybb(Il(I)-1,Ny)-(Ybb(Il(I),Ny)-Ybb(Il(I)-1,Ny))*0.1E-8
	!c      X(I,Ny)=Xbb(Il(I)-1,Ny)-(Xbb(Il(I),Ny)-Xbb(Il(I)-1,Ny))*0.1E-8
	!c      Y(I,Ny)=Ybb(Il(I)-1,Ny)-(Ybb(Il(I),Ny)-Ybb(Il(I)-1,Ny))*0.1E-8
		  Il(I)=Il(I)-1
	!c      go to 2001
		  endif
		  else
		  Sangle=3.141592-Uangle-Aveangle
		  Dmod=Dabs((SS*dcos(Aveangle)*dcos(Sangle)+SS*dcos(Uangle))&
		 /(1.-dcos(Aveangle)*dcos(Aveangle)))
		  if(Dmod.gt.U1*0.9)Dmod=U1*0.9
		  Xbou1=X(I,Ny)+(X(I+1,Ny)-X(I,Ny))*Dmod/U1
		  Ybou1=Y(I,Ny)+(Y(I+1,Ny)-Y(I,Ny))*Dmod/U1
		  Abb=(Ybb(Il(I)-1,Ny)-Ybb(Il(I),Ny))&
		 /(Xbb(Il(I)-1,Ny)-Xbb(Il(I),Ny))
		  Bbb=Ybb(Il(I),Ny)-Abb*Xbb(Il(I),Ny)
		  Anew=(Y(I,Ny-1)-Ybou1)/(X(I,Ny-1)-Xbou1)
		  Bnew=Y(I,Ny-1)-Anew*X(I,Ny-1)
		  Xbou1=-(Bbb-Bnew)/(Abb-Anew)
		  Ybou1=Anew*Xbou1+Bnew
		  U1bb=Dsqrt((X(I,Ny)-Xbb(Il(I),Ny))**2.&
		 +(Y(I,Ny)-Ybb(Il(I),Ny))**2.)
		  U1new=Dsqrt((X(I,Ny)-Xbou1)**2.+(Y(I,Ny)-Ybou1)**2.)
		  if(U1new.gt.U1bb)then
		  Xbou1=Xbb(Il(I),Ny)+(Xbb(Il(I),Ny)-Xbb(Il(I)-1,Ny))*0.1E-8
		  Ybou1=Ybb(Il(I),Ny)+(Ybb(Il(I),Ny)-Ybb(Il(I)-1,Ny))*0.1E-8
	!c      X(I,Ny)=Xbb(Il(I),Ny)+(Xbb(Il(I),Ny)-Xbb(Il(I)-1,Ny))*0.1E-8
	!c      Y(I,Ny)=Ybb(Il(I),Ny)+(Ybb(Il(I),Ny)-Ybb(Il(I)-1,Ny))*0.1E-8
		  Il(I)=Il(I)+1
	!c      go to 2001
		  endif
		  endif
		  endif
		  X(I-1,Ny)=Xbou2
		  Y(I-1,Ny)=Ybou2
		  Xbou2=Xbou1
		  Ybou2=Ybou1
	 1001 continue
	*/

	T = T + 1;
	if (T >= 50) {
		T = 0;
	}
	if (Xmax/0.02 <= 0.1e-2 && Ymax / 0.02 <= 0.1e-2) {
		if (L < Iter) {
			//!c      if(L.lt.1)then
			double Xbou2 = X(2, Ny);
			double Ybou2 = Y(2, Ny);

			for (int I = 3; I <= Nx - 2; ++I) {
				double D1 = (QVector2D(X(I, Ny), Y(I, Ny)) - QVector2D(X(I - 1, Ny), Y(I - 1, Ny))).length();
				double D2 = (QVector2D(X(I, Ny - 1), Y(I, Ny - 1)) - QVector2D(X(I - 1, Ny), Y(I - 1, Ny))).length();
				double SS = (QVector2D(X(I, Ny - 1), Y(I, Ny - 1)) - QVector2D(X(I, Ny), Y(I, Ny))).length();
				double U1 = (QVector2D(X(I, Ny), Y(I, Ny)) - QVector2D(X(I + 1, Ny), Y(I + 1, Ny))).length();
				double U2 = (QVector2D(X(I, Ny - 1), Y(I, Ny - 1)) - QVector2D(X(I + 1, Ny), Y(I + 1, Ny))).length();

				double Dangle = acos((D1 * D1 + SS * SS - D2 * D2) / (2 * D1 * SS));
				double Uangle = acos((U1 * U1 + SS * SS - U2 * U2) / (2 * U1 * SS));
				double difDU = qAbs(Dangle - Uangle);

				double Xbou1;
				double Ybou1;

				if (difDU < 0.1e-5) {
					Xbou1 = X(I, Ny);
					Ybou1 = Y(I, Ny);
					goto Label1001;
				} else {
					double Aveangle = (Dangle + Uangle) / 2;
					if (Dangle < Uangle) {
						double Sangle = 3.141592 - Dangle - Aveangle;
						double Dmod = qAbs((SS * cos(Aveangle) * cos(Sangle) + SS * cos(Dangle))
															 / (1 - cos(Aveangle) * cos(Aveangle)));
						if (Dmod > D1 * 0.9) {Dmod = D1 * 0.9;}
						double Xbou1 = X(I, Ny) - (X(I, Ny) - X(I - 1, Ny)) * Dmod / D1;
						double Ybou1 = Y(I, Ny) - (Y(I, Ny) - Y(I - 1, Ny)) * Dmod / D1;
						double Abb = (Ybb(IL(I), Ny) - Ybb(IL(I) - 1, Ny)) / (Xbb(IL(I), Ny) - Xbb(IL(I) - 1, Ny));
						double Bbb = Ybb(IL(I) - 1, Ny) - Abb * Xbb(IL(I) - 1, Ny);
						double Anew = (Y(I, Ny - 1) - Ybou1) / (X(I, Ny - 1) - Xbou1);
						double Bnew = Y(I, Ny - 1) - Anew * X(I, Ny - 1);
						Xbou1 = - (Bbb - Bnew) / (Abb - Anew);
						Ybou1 = Anew * Xbou1 + Bnew;
						double D1bb = (QVector2D(X(I, Ny), Y(I, Ny)) - QVector2D(Xbb(IL(I) - 1, Ny), Ybb(IL(I) - 1, Ny))).length();
						double D1new = (QVector2D(X(I, Ny), Y(I, Ny)) - QVector2D(Xbou1, Ybou1)).length();

						if (D1new > D1bb) {
							Xbou1 = Xbb(IL(I) - 1, Ny) - (Xbb(IL(I), Ny) - Xbb(IL(I) - 1, Ny)) * 0.1E-8;
							Ybou1 = Ybb(IL(I) - 1, Ny) - (Ybb(IL(I), Ny) - Ybb(IL(I) - 1, Ny)) * 0.1E-8;
							IL(I)=IL(I) - 1;
						}
					} else {
						double Sangle = 3.141592 - Uangle - Aveangle;
						double Dmod = qAbs((SS * cos(Aveangle) * cos(Sangle) + SS * cos(Uangle))
															 / (1 - cos(Aveangle) * cos(Aveangle)));
						if (Dmod > U1 * 0.9) {Dmod = U1 * 0.9;}
						double Xbou1 = X(I, Ny) + (X(I + 1, Ny) - X(I, Ny)) * Dmod / U1;
						double Ybou1 = Y(I ,Ny) + (Y(I + 1, Ny) - Y(I, Ny)) * Dmod / U1;
						double Abb = (Ybb(IL(I) - 1, Ny) - Ybb(IL(I), Ny)) / (Xbb(IL(I) - 1, Ny) - Xbb(IL(I), Ny));
						double Bbb = Ybb(IL(I), Ny) - Abb * Xbb(IL(I), Ny);
						double Anew = (Y(I,Ny - 1) - Ybou1) / (X(I, Ny - 1) - Xbou1);
						double Bnew = Y(I, Ny - 1) - Anew * X(I, Ny - 1);
						Xbou1 = - (Bbb - Bnew) / (Abb - Anew);
						Ybou1 = Anew * Xbou1 + Bnew;
						double U1bb = (QVector2D(X(I, Ny), Y(I, Ny)) - QVector2D(Xbb(IL(I), Ny), Ybb(IL(I), Ny))).length();
						double U1new = (QVector2D(X(I, Ny), Y(I, Ny)) - QVector2D(Xbou1, Ybou1)).length();
						if (U1new > U1bb) {
							Xbou1 = Xbb(IL(I), Ny) + (Xbb(IL(I), Ny) - Xbb(IL(I) - 1, Ny)) * 0.1E-8;
							Ybou1 = Ybb(IL(I), Ny) + (Ybb(IL(I), Ny) - Ybb(IL(I) - 1, Ny)) * 0.1E-8;
							IL(I) = IL(I) + 1;
						}
					}
				}
				X(I - 1, Ny) = Xbou2;
				Y(I - 1, Ny) = Ybou2;
				Xbou2 = Xbou1;
				Ybou2 = Ybou1;
			}

Label1001:

			Xbou2 = X(2, 1);
			Ybou2 = Y(2, 1);

			for (int I = 3; I <= Nx - 2; ++I) {
				double D1 = (QVector2D(X(I, 1), Y(I, 1)) - QVector2D(X(I - 1, 1), Y(I - 1, 1))).length();
				double D2 = (QVector2D(X(I, 2), Y(I, 2)) - QVector2D(X(I - 1, 1), Y(I - 1, 1))).length();
				double SS = (QVector2D(X(I, 2), Y(I, 2)) - QVector2D(X(I, 1), Y(I, 1))).length();
				double U1 = (QVector2D(X(I, 1), Y(I, 1)) - QVector2D(X(I + 1, 1), Y(I + 1, 1))).length();
				double U2 = (QVector2D(X(I, 2), Y(I, 2)) - QVector2D(X(I + 1, 1), Y(I + 1, 1))).length();

				double Dangle = acos((D1 * D1 + SS * SS - D2 * D2) / (2 * D1 * SS));
				double Uangle = acos((U1 * U1 + SS * SS - U2 * U2) / (2 * U1 * SS));
				double difDU = qAbs(Dangle - Uangle);

				double Xbou1;
				double Ybou1;

				if (difDU < 0.1e-5) {
					Xbou1 = X(I, 1);
					Ybou1 = Y(I, 1);
					goto Label1002;
				} else {
					double Aveangle = (Dangle + Uangle) / 2;
					if (Dangle < Uangle) {
						double Sangle = 3.141592 - Dangle - Aveangle;
						double Dmod = qAbs((SS * cos(Aveangle) * cos(Sangle) + SS * cos(Uangle))
															 / (1 - cos(Aveangle) * cos(Aveangle)));
						if (Dmod > D1 * 0.9) {Dmod = D1 * 0.9;}
						Xbou1 = X(I, 1) - (X(I, 1) - X(I - 1, 1)) * Dmod / D1;
						Ybou1 = Y(I, 1) - (Y(I, 1) - Y(I - 1, 1)) * Dmod / D1;
						double Abb = (Ybb(IR(I), 1) - Ybb(IR(I) - 1, 1)) / (Xbb(IR(I), 1) - Xbb(IR(I) - 1, 1));
						double Bbb = Ybb(IR(I) - 1, 1) - Abb * Xbb(IR(I) - 1, 1);
						double Anew = (Y(I, 2) - Ybou1) / (X(I, 2) - Xbou1);
						double Bnew = Y(I, 2) - Anew * X(I, 2);
						Xbou1 = - (Bbb - Bnew) / (Abb - Anew);
						Ybou1 = Anew * Xbou1 + Bnew;

						double D1bb = (QVector2D(X(I, 1), Y(I, 1)) - QVector2D(Xbb(IL(I) - 1, 1), Ybb(IL(I) - 1, 1))).length();
						double D1new = (QVector2D(X(I, 1), Y(I, 1)) - QVector2D(Xbou1, Ybou1)).length();

						if (D1new > D1bb) {
							Xbou1 = Xbb(IR(I) - 1, 1) - (Xbb(IR(I), 1) - Xbb(IR(I) - 1, 1)) * 0.1E-8;
							Ybou1 = Ybb(IR(I) - 1, 1) - (Ybb(IR(I), 1) - Ybb(IR(I) - 1, 1)) * 0.1E-8;
							IR(I) = IR(I) - 1;
						}
					} else {
						double Sangle = 3.141592 - Uangle - Aveangle;
						double Dmod = qAbs((SS * cos(Aveangle) * cos(Sangle) + SS * cos(Uangle))
															 / (1 - cos(Aveangle) * cos(Aveangle)));
						if (Dmod > U1 *0.9) {Dmod = U1 * 0.9;}
						Xbou1 = X(I, 1) + (X(I + 1, 1) - X(I, 1)) * Dmod / U1;
						Ybou1 = Y(I, 1) + (Y(I + 1, 1) - Y(I, 1)) * Dmod / U1;
						double Abb = (Ybb(IR(I) - 1, 1) - Ybb(IR(I), 1)) / (Xbb(IR(I) - 1, 1) - Xbb(IR(I), 1));
						double Bbb = Ybb(IR(I), 1) - Abb * Xbb(IR(I), 1);
						double Anew = (Y(I, 2) - Ybou1) / (X(I, 2) - Xbou1);
						double Bnew = Y(I, 2) - Anew * X(I, 2);
						Xbou1 = - (Bbb - Bnew) / (Abb - Anew);
						Ybou1 = Anew * Xbou1 + Bnew;

						double U1bb = (QVector2D(X(I, 1), Y(I, 1)) - QVector2D(Xbb(IL(I), 1), Ybb(IL(I), 1))).length();
						double U1new = (QVector2D(X(I, 1), Y(I, 1)) - QVector2D(Xbou1, Ybou1)).length();

						if (U1new > U1bb) {
							Xbou1 = Xbb(IR(I), 1) + (Xbb(IR(I), 1) - Xbb(IR(I) - 1, 1)) * 0.1E-8;
							Ybou1 = Ybb(IR(I), 1) + (Ybb(IR(I), 1) - Ybb(IR(I) - 1, 1)) * 0.1E-8;
							IR(I) = IR(I) + 1;
						}
					}
				}
				X(I - 1, 1) = Xbou2;
				Y(I - 1, 1) = Ybou2;
				Xbou2 = Xbou1;
				Ybou2 = Ybou1;
			}

Label1002:

			if (Nx > 2) {
				double DisNx2 = (QVector2D(X(Nx - 2, 1), Y(Nx - 2, 1)) - QVector2D(X(Nx - 2, Ny), Y(Nx - 2, Ny))).length();
				for (int J = 2; J <= Ny - 1; ++J) {
					double DisNx2j = (QVector2D(X(Nx - 2, 1), Y(Nx - 2, 1)) - QVector2D(X(Nx - 2, J), Y(Nx - 2, J))).length();
					X(Nx, J) = X(Nx, 1) + (X(Nx, Ny) - X(Nx, 1)) * DisNx2j / DisNx2;
					Y(Nx, J) = Y(Nx, 1) + (Y(Nx, Ny) - Y(Nx, 1)) * DisNx2j / DisNx2;
					X(Nx - 1, J) = X(Nx - 1, 1) + (X(Nx - 1, Ny) - X(Nx - 1, 1)) * DisNx2j / DisNx2;
					Y(Nx - 1, J) = Y(Nx - 1, 1) + (Y(Nx - 1, Ny) - Y(Nx - 1, 1)) * DisNx2j / DisNx2;
				}
			}
			L = L + 1;
			double Para4 = (Ny_1 + Ny_2 + Ny_3) / Npart;
			if (KK == 1) {
				aL99 = aL99 + 1 * Ny_1 / Para4;
			} else if (KK == 2) {
				aL99 = aL99 + 1 * Ny_2 / Para4;
			} else {
				aL99 = aL99 + 1 * Ny_3 / Para4;
			}
			*para3 = aL99 / (Iter * Npart) * 100;
		} else {
			goto Label230;
		}
	}
	goto Label235;

	/* ---- Results --- */

Label230:


	if (KK == 1) {
//		nys = 1;
//		nye = Ny_1;
	} else if (KK == 2) {
//		nys = Ny_1 + 1;
//		nye = (Ny_1 + Ny_2) - 1;
	} else {
//		nys = (Ny_1 + Ny_2);
//		nye = (Ny_1 + Ny_2 + Ny_3) - 2;
	}
	for (int I = 1; I <= Nx; ++I) {
		for (int J = 1; J <= Ny; ++J) {
			Xdiv(I, J, KK) = X(I, J);
			Ydiv(I, J, KK) = Y(I, J);
		}
	}
	if (KK <= 2) {
		L = 0;
		goto Label9955;
	}

	// combine the grid.
	combine(Nx, Ny_1, Ny_2, Ny_3, NyA);

	// output the grid data to txgrd, tygrd.
	for (int J = 1; J <= NyA; ++J) {
		for (int I = 1; I <= Nx; ++I) {
			int index = Nx * (J - 1) + (I - 1);
			*(txgrd + index) = Xall(I, J);
			*(tygrd + index) = Yall(I, J);
		}
	}

	// deallocate memory.
	deallocate();
}

bool GridCreatingConditionCompoundChannelFunctions::allocate(int ns, int nn, int npart)
{
	/*
	    ALLOCATE(X(NS,NN), STAT = STATUS)
	    ALLOCATE(Y(NS,NN), STAT = STATUS)
	    ALLOCATE(G11(NS,NN), STAT = STATUS)
	    ALLOCATE(G22(NS,NN), STAT = STATUS)
	    ALLOCATE(Xbb(NS,NN), STAT = STATUS)
	    ALLOCATE(Ybb(NS,NN), STAT = STATUS)
	    ALLOCATE(Xbbq(NS,NN), STAT = STATUS)
	    ALLOCATE(Ybbq(NS,NN), STAT = STATUS)
	    ALLOCATE(XNew(NS,NN), STAT = STATUS)
	    ALLOCATE(Ynew(NS,NN), STAT = STATUS)
	    ALLOCATE(Il(NS), STAT = STATUS)
	    ALLOCATE(IR(NS), STAT = STATUS)
	    ALLOCATE(Xall(NS,NN), STAT = STATUS) !modified by Hiroshi
	    ALLOCATE(Yall(NS,NN), STAT = STATUS) !modified by Hiroshi
	    ALLOCATE(Xdiv(NS,NN,Npart), STAT = STATUS) !modified by Hiroshi
	    ALLOCATE(Ydiv(NS,NN,Npart), STAT = STATUS) !modified by Hiroshi
	    ALLOCATE(XdivF(NS,NN,Npart), STAT = STATUS) !modified by Hiroshi
	    ALLOCATE(YdivF(NS,NN,Npart), STAT = STATUS) !modified by Hiroshi
	*/

	m_ns = ns;
	m_nn = nn;
	m_npart = npart;

	m_X = new double[ns * nn];
	m_Y = new double[ns * nn];
	m_G11 = new double[ns * nn];
	m_G22 = new double[ns * nn];
	m_Xbb = new double[ns * nn];
	m_Ybb = new double[ns * nn];
	m_Xbbq = new double[ns * nn];
	m_Ybbq = new double[ns * nn];
	m_XNew = new double[ns * nn];
	m_YNew = new double[ns * nn];
	m_Xall = new double[ns * nn];
	m_Yall = new double[ns * nn];

	m_IL = new int[ns];
	m_IR = new int[ns];

	m_Xdiv = new double[ns * nn * npart];
	m_Ydiv = new double[ns * nn * npart];
	m_XdivF = new double[ns * nn * npart];
	m_YdivF = new double[ns * nn * npart];

	return true;
}

void GridCreatingConditionCompoundChannelFunctions::deallocate()
{
	/*
	    DEALLOCATE(X, STAT = STATUS)
	    DEALLOCATE(Y, STAT = STATUS)
	    DEALLOCATE(G11, STAT = STATUS)
	    DEALLOCATE(G22, STAT = STATUS)
	    DEALLOCATE(Xbb, STAT = STATUS)
	    DEALLOCATE(Ybb, STAT = STATUS)
	    DEALLOCATE(Xbbq, STAT = STATUS)
	    DEALLOCATE(Ybbq, STAT = STATUS)
	    DEALLOCATE(XNew, STAT = STATUS)
	    DEALLOCATE(Ynew, STAT = STATUS)
	    DEALLOCATE(Il, STAT = STATUS)
	    DEALLOCATE(IR, STAT = STATUS)
	    DEALLOCATE(Xall, STAT = STATUS) !modified by Hiroshi
	    DEALLOCATE(Yall, STAT = STATUS) !modified by Hiroshi
	    DEALLOCATE(Xdiv, STAT = STATUS) !modified by Hiroshi
	    DEALLOCATE(Ydiv, STAT = STATUS) !modified by Hiroshi
	    DEALLOCATE(XdivF, STAT = STATUS) !modified by Hiroshi
	    DEALLOCATE(YdivF, STAT = STATUS) !modified by Hiroshi
	*/
	delete m_X;
	delete m_Y;
	delete m_G11;
	delete m_G22;
	delete m_Xbb;
	delete m_Ybb;
	delete m_Xbbq;
	delete m_Ybbq;
	delete m_XNew;
	delete m_YNew;
	delete m_Xall;
	delete m_Yall;

	delete m_IL;
	delete m_IR;

	delete m_Xdiv;
	delete m_Ydiv;
	delete m_XdivF;
	delete m_YdivF;
}

void GridCreatingConditionCompoundChannelFunctions::combine(int Nx, int Ny_1, int Ny_2, int Ny_3, int NyA)
{
	/*
	      do 9997 Ic=2,Nx-1
	      Dismin1=1.0E12
	      Dismin2=1.0E12
	      do 9999 I=1,Nx
	      Dis1=Dsqrt((Xdiv(I,Ny_1,1)-Xdiv(Ic,1,2))**2.&
	     +(Ydiv(I,Ny_1,1)-Ydiv(Ic,1,2))**2.)
	      if(Dis1.le.Dismin1)then
	      Dismin2=Dismin1
	      Imin2=Imin1
	      Dismin1=Dis1
	      Imin1=I
	      elseif(Dis1.le.Dismin2)then
	      Dismin2=Dis1
	      Imin2=I
	      endif
	 9999 continue
	      do 9998 J=1,Ny_1
	      if(Imin1.gt.Imin2)then
	      XdivF(Ic,J,1)=Xdiv(Imin2,J,1)&
	     +(Xdiv(Imin1,J,1)-Xdiv(Imin2,J,1))&
	     *Dismin2/(Dismin1+Dismin2)
	      YdivF(Ic,J,1)=Ydiv(Imin2,J,1)&
	     +(Ydiv(Imin1,J,1)-Ydiv(Imin2,J,1))&
	     *Dismin2/(Dismin1+Dismin2)
	      else
	      XdivF(Ic,J,1)=Xdiv(Imin1,J,1)&
	     +(Xdiv(Imin2,J,1)-Xdiv(Imin1,J,1))&
	     *Dismin1/(Dismin1+Dismin2)
	      YdivF(Ic,J,1)=Ydiv(Imin1,J,1)&
	     +(Ydiv(Imin2,J,1)-Ydiv(Imin1,J,1))&
	     *Dismin1/(Dismin1+Dismin2)
	      endif
	 9998 continue
	 9997 continue
	*/

	for (int Ic = 2; Ic < Nx - 1; ++Ic) {
		double Dismin1 = BIGDOUBLE;
		double Dismin2 = BIGDOUBLE;
		int Imin1 = 1;
		int Imin2 = 1;
		for (int I = 1; I <= Nx; ++I) {
			double Dis1 = (QVector2D(Xdiv(I, Ny_1, 1), Ydiv(I, Ny_1, 1)) - QVector2D(Xdiv(Ic, 1, 2), Ydiv(Ic, 1, 2))).length();
			if (Dis1 <= Dismin1) {
				Dismin2 = Dismin1;
				Imin2 = Imin1;
				Dismin1 = Dis1;
				Imin1 = I;
			} else if (Dis1 <= Dismin2) {
				Dismin2 = Dis1;
				Imin2 = I;
			}
		}
		for (int J = 1; J <= Ny_1; ++ J) {
			if (Imin1 > Imin2) {
				XdivF(Ic, J, 1) =
					Xdiv(Imin2, J, 1) +
					(Xdiv(Imin1, J, 1) - Xdiv(Imin2, J, 1)) * Dismin2 / (Dismin1 + Dismin2);
				YdivF(Ic, J, 1) =
					Ydiv(Imin2, J, 1) +
					(Ydiv(Imin1, J, 1) - Ydiv(Imin2, J, 1)) * Dismin2 / (Dismin1 + Dismin2);
			} else {
				XdivF(Ic, J, 1) =
					Xdiv(Imin1, J, 1) +
					(Xdiv(Imin2, J, 1) - Xdiv(Imin1, J, 1)) * Dismin1 / (Dismin1 + Dismin2);
				YdivF(Ic, J, 1) =
					Ydiv(Imin1, J, 1) +
					(Ydiv(Imin2, J, 1) - Ydiv(Imin1, J, 1)) * Dismin1 / (Dismin1 + Dismin2);
			}
		}
	}
	/*
	      do 8997 Ic=2,Nx-1
	      Dismin1=1.0E12
	      Dismin2=1.0E12
	      do 8999 I=1,Nx
	      Dis1=Dsqrt((Xdiv(I,1,3)-Xdiv(Ic,Ny_2,2))**2.&
	     +(Ydiv(I,1,3)-Ydiv(Ic,Ny_2,2))**2.)
	      if(Dis1.le.Dismin1)then
	      Dismin2=Dismin1
	      Imin2=Imin1
	      Dismin1=Dis1
	      Imin1=I
	      elseif(Dis1.le.Dismin2)then
	      Dismin2=Dis1
	      Imin2=I
	      endif
	 8999 continue
	      do 8998 J=1,Ny_3
	      if(Imin1.gt.Imin2)then
	      XdivF(Ic,J,3)=Xdiv(Imin2,J,3)&
	     +(Xdiv(Imin1,J,3)-Xdiv(Imin2,J,3))&
	     *Dismin2/(Dismin1+Dismin2)
	      YdivF(Ic,J,3)=Ydiv(Imin2,J,3)&
	     +(Ydiv(Imin1,J,3)-Ydiv(Imin2,J,3))&
	     *Dismin2/(Dismin1+Dismin2)
	      else
	      XdivF(Ic,J,3)=Xdiv(Imin1,J,3)&
	     +(Xdiv(Imin2,J,3)-Xdiv(Imin1,J,3))&
	     *Dismin1/(Dismin1+Dismin2)
	      YdivF(Ic,J,3)=Ydiv(Imin1,J,3)&
	     +(Ydiv(Imin2,J,3)-Ydiv(Imin1,J,3))&
	     *Dismin1/(Dismin1+Dismin2)
	      endif
	 8998 continue
	 8997 continue
	*/
	for (int Ic = 2; Ic <= Nx - 1; ++Ic) {
		double Dismin1 = BIGDOUBLE;
		double Dismin2 = BIGDOUBLE;
		int Imin1 = 1;
		int Imin2 = 1;
		for (int I = 1; I <= Nx; ++I) {
			double Dis1 = (QVector2D(Xdiv(I, 1, 3), Ydiv(I, 1, 3)) - QVector2D(Xdiv(Ic, Ny_2, 2), Ydiv(Ic, Ny_2, 2))).length();
			if (Dis1 <= Dismin1) {
				Dismin2 = Dismin1;
				Imin2 = Imin1;
				Dismin1 = Dis1;
				Imin1 = I;
			} else if (Dis1 <= Dismin2) {
				Dismin2 = Dis1;
				Imin2 = I;
			}
		}
		for (int J = 1; J <= Ny_3; ++ J) {
			if (Imin1 > Imin2) {
				XdivF(Ic, J, 3) =
					Xdiv(Imin2, J, 3) +
					(Xdiv(Imin1, J, 3) - Xdiv(Imin2, J, 3)) * Dismin2 / (Dismin1 + Dismin2);
				YdivF(Ic, J, 3) =
					Ydiv(Imin2, J, 3) +
					(Ydiv(Imin1, J, 3) - Ydiv(Imin2, J, 3)) * Dismin2 / (Dismin1 + Dismin2);
			} else {
				XdivF(Ic, J, 3) =
					Xdiv(Imin1, J, 3) +
					(Xdiv(Imin2, J, 3) - Xdiv(Imin1, J, 3)) * Dismin1 / (Dismin1 + Dismin2);
				YdivF(Ic, J, 3) =
					Ydiv(Imin1, J, 3) +
					(Ydiv(Imin2, J, 3) - Ydiv(Imin1, J, 3)) * Dismin1 / (Dismin1 + Dismin2);
			}
		}
	}
	/*
	      do 9993 J=1,Ny_1
	      XdivF(1,J,1)=Xdiv(1,J,1)
	      YdivF(1,J,1)=Ydiv(1,J,1)
	      XdivF(Nx,J,1)=Xdiv(Nx,J,1)
	      YdivF(Nx,J,1)=Ydiv(Nx,J,1)
	 9993 continue
	*/
	for (int J = 1; J <= Ny_1; ++J) {
		XdivF(1, J, 1) = Xdiv(1, J, 1);
		YdivF(1, J, 1) = Ydiv(1, J, 1);
		XdivF(Nx, J, 1) = Xdiv(Nx, J, 1);
		YdivF(Nx, J, 1) = Ydiv(Nx, J, 1);
	}
	/*
	      do 9996 J=1,Ny_3
	      XdivF(1,J,3)=Xdiv(1,J,3)
	      YdivF(1,J,3)=Ydiv(1,J,3)
	      XdivF(Nx,J,3)=Xdiv(Nx,J,3)
	      YdivF(Nx,J,3)=Ydiv(Nx,J,3)
	 9996 continue
	*/
	for (int J = 1; J <= Ny_3; ++J) {
		XdivF(1, J, 3) = Xdiv(1, J, 3);
		YdivF(1, J, 3) = Ydiv(1, J, 3);
		XdivF(Nx, J, 3) = Xdiv(Nx, J, 3);
		YdivF(Nx, J, 3) = Ydiv(Nx, J, 3);
	}
	/*
	      do 9994 I=1,Nx
	      do 9994 J=1,Ny_2
	      XdivF(I,J,2)=Xdiv(I,J,2)
	      YdivF(I,J,2)=Ydiv(I,J,2)
	 9994 continue
	*/
	for (int I = 1; I <= Nx; ++I) {
		for (int J = 1; J <= Ny_2; ++J) {
			XdivF(I, J, 2) = Xdiv(I, J, 2);
			YdivF(I, J, 2) = Ydiv(I, J, 2);
		}
	}

	/*
	      do 9995 I=1,Nx
	      do 9995 J=1,NyA
	      Ny_11=Ny_1-1
	      Ny_12=Ny_11+Ny_2
	      Ny_13=Ny_12+Ny_3
	      if(J.le.Ny_11)then
	      Xall(I,J)=XdivF(I,J,1)
	      Yall(I,J)=YdivF(I,J,1)
	      elseif(J.le.Ny_12)then
	      Xall(I,J)=XdivF(I,J-Ny_11,2)
	      Yall(I,J)=YdivF(I,J-Ny_11,2)
	      else
	      Xall(I,J)=XdivF(I,J-Ny_12+1,3)
	      Yall(I,J)=YdivF(I,J-Ny_12+1,3)
	      endif
	 9995 continue
	*/
	int Ny_11 = Ny_1 - 1;
	int Ny_12 = Ny_11 + Ny_2;
	for (int I = 1; I <= Nx; ++I) {
		for (int J = 1; J <= NyA; ++J) {

			if (J <= Ny_11) {
				Xall(I, J) = XdivF(I, J, 1);
				Yall(I, J) = YdivF(I, J, 1);
			} else if (J <= Ny_12) {
				Xall(I, J) = XdivF(I, J - Ny_11, 2);
				Yall(I, J) = YdivF(I, J - Ny_11, 2);
			} else {
				Xall(I, J) = XdivF(I, J - Ny_12 + 1, 3);
				Yall(I, J) = YdivF(I, J - Ny_12 + 1, 3);
			}
		}
	}
}
