program Test2
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier
	
	integer:: isize, jsize, ksize
	double precision, dimension(:,:), allocatable:: grid_x, grid_y
	double precision, dimension(:,:,:), allocatable:: grid3d_x, grid3d_y, grid3d_z
	double precision, dimension(:,:), allocatable:: node_real
	double precision, dimension(:,:,:), allocatable:: node3d_real

	double precision, dimension(:,:,:), allocatable:: result_real

	integer:: i, j, k, t
	double precision timevalue

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
	print *, 'grid x, y, z: isize, jsize=', isize, jsize
	do i = 1, isize
		do j = 1, jsize
			print *, '(', i, ', ', j, ') = (', grid_x(i, j), ', ', grid_y(i, j), ')'
		end do
	end do

	! 格子点で定義された属性 node_real のメモリを確保
	allocate(node_real(isize, jsize))
	! 属性を読み込む
	call cg_iric_read_grid_real_node_f('node_real', node_real, ier)
	print *, 'node_real: isize, jsize=', isize, jsize
	do i = 1, isize
		do j = 1, jsize
			print *, ' (',i,',',j,')=(',node_real(i,j),')'
		end do
	end do

	! 2次元格子を元に、3次元格子を生成
	ksize = 5
	allocate( &
		grid3d_x(isize, jsize, ksize), &
		grid3d_y(isize, jsize, ksize), &
		grid3d_z(isize, jsize, ksize), &
		node3d_real(isize, jsize, ksize) &
	)

	do i = 1, isize
		do j = 1, jsize
			grid3d_x(i, j, 1:ksize) = grid_x(i, j)
			grid3d_y(i, j, 1:ksize) = grid_y(i, j)
			node3d_real(i, j, 1:ksize) = node_real(i, j)
		end do
	end do
	do k = 1, ksize
		grid3d_z(1:isize, 1:jsize, k) = k
	end do

	! 生成した3次元格子を出力
	call cg_iric_writegridcoord3d_f(isize, jsize, ksize, &
		grid3d_x, grid3d_y, grid3d_z, ier)
	call cg_iric_write_grid_real_node_f('node_real', node3d_real, ier)

	! allocate で確保したメモリを開放
	deallocate(grid_x, grid_y, node_real)

	! 計算結果を出力
	allocate(result_real(isize, jsize, ksize))
	do i = 1, isize
		do j = 1, jsize
			do k = 1, ksize
				result_real(i, j, k) = i * j * k
			end do
		end do
	end do
	do t = 1, 5
		timevalue = t
		call cg_iric_write_sol_time_f(timevalue, ier);
		grid3d_x(1, 1, 1) = grid3d_x(1, 1, 1) + 0.1
		call cg_iric_write_sol_gridcoord3d_f(grid3d_x, grid3d_y, grid3d_z, ier)
		call cg_iric_write_sol_real_f('result_real', result_real, ier)
	end do

	deallocate(grid3d_x, grid3d_y, grid3d_z, node3d_real)

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Test2
