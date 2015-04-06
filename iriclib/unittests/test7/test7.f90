program Test7
	implicit none
	include 'cgnslib_f.h'

	integer:: fin1, fin2, ier
	
	integer:: isize, jsize
	double precision, dimension(:,:), allocatable:: grid_x, grid_y
	double precision, dimension(:,:), allocatable:: result_real
	double precision, dimension(:,:), allocatable:: result_int

	double precision:: result_biter_real
	integer:: result_biter_int

	integer:: i, j, solid, solcount, iter
	double precision:: t

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin1, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"
	call cg_open_f('result.cgn', CG_MODE_READ, fin2, ier)

	! iRIClib の初期化
	call cg_iric_init_f(fin1, ier)
	! 格子がないので失敗する
	call cg_iric_initread_f(fin2, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file2 ***"

	! 格子のサイズを調べる
	call cg_iric_gotogridcoord2d_mul_f(fin2, isize, jsize, ier)

	! 格子を読み込むためのメモリを確保
	allocate(grid_x(isize,jsize), grid_y(isize,jsize))
	! 格子を読み込む
	call cg_iric_getgridcoord2d_mul_f(fin2, grid_x, grid_y, ier)

	if (ier /=0) STOP "*** No grid data ***"

	! 格子を書き込む
	call cg_iric_writegridcoord2d_mul_f(fin1, isize, jsize, grid_x, grid_y, ier)
	if (ier /=0) STOP "*** No grid data ***"


	call cg_iric_read_sol_count_mul_f(fin2, solcount, ier)

	! 計算結果を処理
	allocate(result_int(isize, jsize), result_real(isize, jsize))
	do solid = 1, solcount
		! 計算結果を読み込み
		call cg_iric_read_sol_time_mul_f(fin2, solid, t, ier)
		call cg_iric_read_sol_gridcoord2d_mul_f(fin2, solid, grid_x, grid_y, ier)
		call cg_iric_read_sol_baseiterative_integer_mul_f(fin2, solid, 'result_biter_int', result_biter_int, ier)
		call cg_iric_read_sol_baseiterative_real_mul_f(fin2, solid, 'result_biter_real', result_biter_real, ier)
		call cg_iric_read_sol_integer_mul_f(fin2, solid, 'result_int', result_int, ier)
		call cg_iric_read_sol_real_mul_f(fin2, solid, 'result_real', result_real, ier)

		call cg_iric_write_sol_time_mul_f(fin1, t * 2, ier)
		grid_x = grid_x * 2
		grid_y = grid_y * 2
		result_biter_real = result_biter_real * 4
		result_biter_int = result_biter_int * 3
		result_real = result_real * 2.5
		result_int = result_int * 2
		call cg_iric_write_sol_gridcoord2d_mul_f(fin1, grid_x, grid_y, ier)
		call cg_iric_write_sol_baseiterative_integer_mul_f(fin1, 'result_biter_int', result_biter_int, ier)
		call cg_iric_write_sol_baseiterative_real_mul_f(fin1, 'result_biter_real', result_biter_real, ier)
		call cg_iric_write_sol_integer_mul_f(fin1, 'result_int', result_int, ier)
		call cg_iric_write_sol_real_mul_f(fin1, 'result_real', result_real, ier)
	end do

	! CGNS ファイルのクローズ
	call cg_close_f(fin1, ier)
	call cg_close_f(fin2, ier)
	stop
end program Test7
