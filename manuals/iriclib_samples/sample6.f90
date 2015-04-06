
program Sample6
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, isize, jsize
	double precision:: time
	double precision:: convergence
	double precision, dimension(:,:), allocatable::grid_x, grid_y
	real, dimension(:,:), allocatable:: velocity_x, velocity_y, depth
	integer, dimension(:,:), allocatable:: wetflag

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
	! 計算結果を保持するメモリも確保
	allocate(velocity_x(isize,jsize), velocity_y(isize,jsize), depth(isize, jsize), wetflag(isize,jsize))
	! 格子を読み込む
	call cg_iric_getgridcoord2d_f (grid_x, grid_y, ier)

	! 初期状態の情報を出力
	time = 0
	convergence = 0.1
	call cg_iric_write_sol_time_f(time, ier)
	! 格子を出力
	call cg_iric_write_sol_gridcoord2d_f (grid_x, grid_y, ier)
	! 計算結果を出力
	call cg_iric_write_sol_real_f ('VelocityX', velocity_x, ier)
	call cg_iric_write_sol_real_f ('VelocityY', velocity_y, ier)
	call cg_iric_write_sol_real_f ('Depth', depth, ier)
	call cg_iric_write_sol_integer_f ('Wet', wetflag, ier)
	call cg_iric_write_sol_baseiterative_real_f ('Convergence', convergence, ier)
	do
		time = time + 10.0
		! (ここで計算を実行。格子の形状も変化)
		call cg_iric_write_sol_time_f(time, ier)
		! 格子を出力
		call cg_iric_write_sol_gridcoord2d_f (grid_x, grid_y, ier)
		! 計算結果を出力
		call cg_iric_write_sol_real_f ('VelocityX', velocity_x, ier)
		call cg_iric_write_sol_real_f ('VelocityY', velocity_y, ier)
		call cg_iric_write_sol_real_f ('Depth', depth, ier)
		call cg_iric_write_sol_integer_f ('Wet', wetflag, ier)
		call cg_iric_write_sol_baseiterative_real_f ('Convergence', convergence, ier)

		If (time > 1000) exit
	end do

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample6
