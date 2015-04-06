program Sample5
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, isize, jsize
	double precision:: time
	double precision, dimension(:,:), allocatable:: grid_x, grid_y

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

	! 格子のサイズを調べる
	call cg_iric_gotogridcoord2d_f(isize, jsize, ier)
	! 格子を読み込むためのメモリを確保
	allocate(grid_x(isize,jsize), grid_y(isize,jsize))
	! 格子を読み込む
	call cg_iric_getgridcoord2d_f(grid_x, grid_y, ier)

	! 初期状態の情報を出力
	time = 0

	call cg_iric_write_sol_time_f(time, ier)
	! 格子を出力
	call cg_iric_write_sol_gridcoord2d_f (grid_x, grid_y, ier)

	do
		time = time + 10.0
		! (ここで計算を実行)
		call cg_iric_write_sol_time_f(time, ier)
		call cg_iric_write_sol_gridcoord2d_f (grid_x, grid_y, ier)
		If (time > 1000) exit
	end do

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample5
