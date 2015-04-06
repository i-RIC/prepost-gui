program Sample1
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, i_flow

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

	call cg_iric_read_integer_f('i_flow', i_flow, ier)
	print *, i_flow;

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample1
