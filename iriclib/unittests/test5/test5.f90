program Test5
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier

	integer:: isize, jsize, ksize
	double precision, dimension(:,:,:), allocatable:: grid_x, grid_y, grid_z
	double precision, dimension(:,:,:), allocatable:: node_real
	double precision, dimension(:,:,:), allocatable:: cell_real

	integer:: i, j, k

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)

	isize = 10
	jsize = 8
	ksize = 6

	allocate(grid_x(isize, jsize, ksize), grid_y(isize, jsize, ksize), grid_z(isize, jsize, ksize))
	allocate(node_real(isize, jsize, ksize))
	allocate(cell_real(isize-1, jsize-1, ksize-1))
	do i = 1, isize
		do j = 1, jsize
			do k = 1, ksize
				grid_x(i, j, k) = i
				grid_y(i, j, k) = j
				grid_z(i, j, k) = k
				node_real(i, j, k) = i * 2 + j * 3 - k * 1
			end do
		end do
	end do

	do i = 1, isize-1
		do j = 1, jsize-1
			do k = 1, ksize-1
				cell_real(i, j, k) = i * 3 - j * 2 + k * 4
			end do
		end do
	end do

	call cg_iric_writegridcoord3d_f(isize, jsize, ksize, grid_x, grid_y, grid_z, ier)
	call cg_iric_write_grid_real_node_f('node_real', node_real, ier);
	call cg_iric_write_grid_real_cell_f('cell_real', cell_real, ier);

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Test5
