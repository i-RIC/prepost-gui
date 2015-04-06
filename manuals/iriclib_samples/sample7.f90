
program Sample7
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, isize, jsize, ksize, i, j, k, aret
	double precision:: time
	double precision:: convergence
	double precision, dimension(:,:), allocatable::grid_x, grid_y, elevation
	double precision, dimension(:,:,:), allocatable::grid3d_x, grid3d_y, grid3d_z
	double precision, dimension(:,:,:), allocatable:: velocity, density

	! CGNS ファイルのオープン
	call cg_open_f('test3d.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

	! 格子のサイズを調べる
	call cg_iric_gotogridcoord2d_f(isize, jsize, ier)
	! 格子を読み込むためのメモリを確保
	allocate(grid_x(isize,jsize), grid_y(isize,jsize), elevation(isize,jsize))
	! 格子を読み込む
	call cg_iric_getgridcoord2d_f(grid_x, grid_y, ier)
	call cg_iric_read_grid_real_node_f('Elevation', elevation, ier)

	! 読み込んだ2次元格子を元に、3次元格子を生成。
	! 3次元格子は Z方向に、深さ 5 で、5分割する

	ksize = 6
	allocate(grid3d_x(isize,jsize,ksize), grid3d_y(isize,jsize,ksize), grid3d_z(isize,jsize,ksize))
	allocate(velocity(isize,jsize,ksize), STAT = aret)
	print *, aret
	allocate(density(isize,jsize,ksize), STAT = aret)
	print *, aret
	do i = 1, isize
		do j = 1, jsize
			do k = 1, ksize
				grid3d_x(i,j,k) = grid_x(i,j)
				grid3d_y(i,j,k) = grid_y(i,j)
				grid3d_z(i,j,k) = elevation(i,j) + (k - 1)
				velocity(i,j,k) = 0
				density(i,j,k) = 0
			end do
		end do
	end do
	! 生成した3次元格子を出力
	call cg_iric_writegridcoord3d_f(isize, jsize, ksize, grid3d_x, grid3d_y, grid3d_z, ier)

	! 初期状態の情報を出力
	time = 0
	convergence = 0.1
	call cg_iric_write_sol_time_f(time, ier)
	! 格子を出力
	call cg_iric_write_sol_gridcoord3d_f(grid3d_x, grid3d_y, grid3d_z, ier)
	! 計算結果を出力
	call cg_iric_write_sol_real_f('Velocity', velocity, ier)
	call cg_iric_write_sol_real_f('Density', density, ier)
	call cg_iric_write_sol_baseiterative_real_f ('Convergence', convergence, ier)


	do
		time = time + 10.0
		! (ここで計算を実行。格子の形状も変化)
		call cg_iric_write_sol_time_f(time, ier)
		! 格子を出力
		call cg_iric_write_sol_gridcoord3d_f(grid3d_x, grid3d_y, grid3d_z, ier)
		! 計算結果を出力
		call cg_iric_write_sol_real_f('Velocity', velocity, ier)
		call cg_iric_write_sol_real_f('Density', density, ier)
		call cg_iric_write_sol_baseiterative_real_f ('Convergence', convergence, ier)

		If (time > 100) exit
	end do

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample7
