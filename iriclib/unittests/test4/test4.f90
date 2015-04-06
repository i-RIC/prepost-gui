program Test4
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier

	integer:: isize
	double precision, dimension(:), allocatable:: grid_x
	double precision, dimension(:), allocatable:: node_real
	double precision, dimension(:), allocatable:: cell_real

	integer:: i, j

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)

	isize = 10

	allocate(grid_x(isize))
	allocate(node_real(isize))
	allocate(cell_real(isize-1))
	do i = 1, isize
		grid_x(i) = i
		node_real(i) = i * 2
	end do

	do i = 1, isize-1
		cell_real(i) = i * 3
	end do

	call cg_iric_writegridcoord1d_f(isize, grid_x, ier)
	call cg_iric_write_grid_real_node_f('node_real', node_real, ier);
	call cg_iric_write_grid_real_cell_f('cell_real', cell_real, ier);

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Test4
