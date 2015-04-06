program Sample4
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, i
	double precision:: time

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

	! 初期状態の情報を出力
	time = 0

	call cg_iric_write_sol_time_f(time, ier)
	! (ここで、初期の計算格子や計算結果を出力)

	do
		time = time + 10.0
		! (ここで計算を実行)
		call cg_iric_write_sol_time_f(time, ier)
		! (ここで、計算格子や計算結果を出力)
		If (time > 1000) exit
	end do

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample4
