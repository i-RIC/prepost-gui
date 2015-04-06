program Sample3
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, discharge_size, i, j
	integer:: isize, jsize
	double precision, dimension(:,:), allocatable:: grid_x, grid_y
	double precision, dimension(:,:), allocatable:: elevation
	integer, dimension(:,:), allocatable:: obstacle

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
	! （出力）
	print *, 'grid x,y: isize, jsize=', isize, jsize
	do i = 1, min(isize,5)
		do j = 1, min(jsize,5)
			print *, ' (',i,',',j,')=(',grid_x(i,j),',',grid_y(i,j),')'
		end do
	end do

	! 格子点で定義された属性 elevation のメモリを確保
	allocate(elevation(isize, jsize))
	! 属性を読み込む
	call cg_iric_read_grid_real_node_f('Elevation', elevation, ier)
	print *, 'Elevation: isize, jsize=', isize, jsize
	do i = 1, min(isize,5)
		do j = 1, min(jsize,5)
			print *, ' (',i,',',j,')=(',elevation(i,j),')'
		end do
	end do

	! セルで定義された属性 obstacle のメモリを確保。セルの属性なのでサイズは (isize-1) * (jsize-1)
	allocate(obstacle(isize-1, jsize-1))
	! 属性を読み込む
	call cg_iric_read_grid_integer_cell_f('Obstacle', obstacle, ier)
	print *, 'Obstacle: isize -1, jsize-1=', isize-1, jsize-1
	do i = 1, min(isize-1,5)
		do j = 1, min(jsize-1,5)
			print *, ' (',i,',',j,')=(',obstacle(i,j),')'
		end do
	end do

	! allocate で確保したメモリを開放
	deallocate(grid_x, grid_y, elevation, obstacle)

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample3
