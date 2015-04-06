program Test11
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier
	
	integer:: isize, jsize
	double precision, dimension(:,:), allocatable:: grid_x, grid_y

	double precision, dimension(:,:), allocatable:: result_real
	double precision, dimension(:), allocatable:: particlex, particley

	integer:: i, j, iter
	double precision:: t

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

	if (ier /=0) STOP "*** No grid data ***"
	! 出力
	print *, 'grid x, y: isize, jsize=', isize, jsize
	do i = 1, isize
		do j = 1, jsize
			print *, '(', i, ', ', j, ') = (', grid_x(i, j), ', ', grid_y(i, j), ')'
		end do
	end do

	! allocate で確保したメモリを開放
	deallocate(grid_x, grid_y)

	! 計算結果を出力
	allocate(result_real(isize, jsize))
	allocate(particlex(10), particley(10))
	do i = 1, isize
		do j = 1, jsize
			result_real(i, j) = i * j
		end do
	end do
	do i = 1, 10
		particlex(i) = i
		particley(i) = 0
	end do
	do iter = 1, 5
		t = iter * 2.1
		call cg_iric_write_sol_time_f(t, ier)
		call cg_iric_write_sol_real_f('result_real', result_real, ier)
		call cg_iric_write_sol_particle_pos2d_f(iter, particlex, particley, ier)
	end do

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Test11
