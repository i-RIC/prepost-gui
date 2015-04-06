program Test3
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier
	
	integer:: isize, jsize
	double precision, dimension(:,:), allocatable:: grid_x, grid_y
	double precision, dimension(:,:), allocatable:: node_real
	double precision, dimension(:,:), allocatable:: cell_real
	integer, dimension(:,:), allocatable:: node_int
	integer, dimension(:,:), allocatable:: cell_int

	integer:: i, j

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)

	isize = 10
	jsize = 8

	allocate(grid_x(isize, jsize), grid_y(isize, jsize))
	allocate(node_real(isize, jsize), node_int(isize, jsize))
	allocate(cell_real(isize-1, jsize-1), cell_int(isize-1, jsize-1))
	do i = 1, isize
		do j = 1, jsize
			grid_x(i, j) = i
			grid_y(i, j) = j
			node_real(i, j) = i * 2 - j
			node_int(i, j) = i + j
		end do
	end do

	do i = 1, isize-1
		do j = 1, jsize-1
			cell_real(i, j) = i * 3 - j * 2
			cell_int(i, j) = i + j * 2
		end do
	end do

	call cg_iric_writegridcoord2d_f(isize, jsize, grid_x, grid_y, ier)
	call cg_iric_write_grid_real_node_f('node_real', node_real, ier);
	call cg_iric_write_grid_integer_node_f('node_int', node_int, ier);
	call cg_iric_write_grid_real_cell_f('cell_real', cell_real, ier);
	call cg_iric_write_grid_integer_cell_f('cell_int', cell_int, ier);

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Test3
