program SampleProgram
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier
	integer:: icount, istatus
	integer:: i, j
	integer:: imax, jmax
	integer:: elev_on
	double precision:: elev_value
	double precision, dimension(:,:), allocatable::grid_x, grid_y, elevation

	character(200)::condFile

	icount = nargs()
	if ( icount.eq.2 ) then
		call getarg(1, condFile, istatus)
	else
		stop "Input File not specified."
	endif

	! 格子生成データファイルを開く
	call cg_open_f(condFile, CG_MODE_MODIFY, fin, ier)
	if (ier /=0) stop "*** Open error of CGNS file ***"

	! iRIClib の初期化。戻り値は 1 になるが問題ない。
	call cg_iric_init_f(fin, ier)

	! 格子生成条件の読み込み
	! 簡潔に記述するため、エラー処理は行っていない
	call cg_iric_read_integer_f("imax", imax, ier)
	call cg_iric_read_integer_f("jmax", jmax, ier)
	call cg_iric_read_integer_f("elev_on", elev_on, ier)
	call cg_iric_read_real_f("elev_value", elev_value, ier)

	! エラー処理
	if (imax * jmax > 100000 ) then
		! 100000 より大きい格子は生成できない
		call cg_iric_write_errorcode_f(1, ier)
		call cg_close_f(fin, ier)
		stop
  endif

	! 格子生成用のメモリを確保
	allocate(grid_x(imax,jmax), grid_y(imax,jmax))
	allocate(elevation(imax,jmax))

	! 格子を生成
	do i = 1, imax
		do j = 1, jmax
			grid_x(i, j) = i
			grid_y(i, j) = j
			elevation(i, j) = elev_value
		end do
	end do

	! 格子を出力
	call cg_iric_writegridcoord2d_f(imax, jmax, grid_x, grid_y, ier)
	if (elev_on == 1) then
		call cg_iric_write_grid_real_node_f("Elevation", elevation, ier);
	end if

	! 格子生成データファイルを閉じる
 	call cg_close_f(fin, ier)
end program SampleProgram
