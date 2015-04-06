program SampleProgram
	implicit none
	include 'cgnslib_f.h'
  integer:: fin, ier
  integer:: icount, istatus
	character(200)::condFile
	integer:: maxiterations
	double precision:: timestep
	integer:: surfacetype
	double precision:: constantsurface
	integer:: variable_surface_size
	double precision, dimension(:), allocatable:: variable_surface_time
	double precision, dimension(:), allocatable:: variable_surface_elevation

	integer:: isize, jsize
	double precision, dimension(:,:), allocatable:: grid_x, grid_y
	double precision, dimension(:,:), allocatable:: elevation
	integer, dimension(:,:), allocatable:: obstacle

	integer:: inflowid
	integer:: inflow_count
	integer:: inflow_element_max
	integer:: discharge_variable_sizemax
	integer, dimension(:), allocatable:: inflow_element_count
	integer, dimension(:,:,:), allocatable:: inflow_element
	integer, dimension(:), allocatable:: discharge_type
	double precision, dimension(:), allocatable:: discharge_constant
	integer, dimension(:), allocatable:: discharge_variable_size
	double precision, dimension(:,:), allocatable:: discharge_variable_time
	double precision, dimension(:,:), allocatable:: discharge_variable_value

	double precision:: time
	integer:: iteration
	double precision, dimension(:,:), allocatable:: velocity_x, velocity_y
	double precision, dimension(:,:), allocatable:: depth
	integer, dimension(:,:), allocatable:: wetflag
	double precision:: convergence

  write(*,*) "Sample Program"

	icount = nargs()
	if ( icount.eq.2 ) then
		call getarg(1, condFile, istatus)
	else
		write(*,*) "Input File not specified."
		stop
  endif

  ! 計算データファイルを開く
	call cg_open_f(condFile, CG_MODE_MODIFY, fin, ier)
	if (ier /=0) stop "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

  ! 計算条件の読み込み
	call cg_iric_read_integer_f("maxIteretions", maxiterations, ier)
	call cg_iric_read_real_f("timeStep", timestep, ier)
	call cg_iric_read_integer_f("surfaceType", surfacetype, ier)
	call cg_iric_read_real_f("constantSurface", constantsurface, ier)

	call cg_iric_read_functionalsize_f("variableSurface", variable_surface_size, ier)
	allocate(variable_surface_time(variable_surface_size))
	allocate(variable_surface_elevation(variable_surface_size))
	call cg_iric_read_functional_f("variableSurface", variable_surface_time, variable_surface_elevation, ier)

	! 格子のサイズを調べる
	call cg_iric_gotogridcoord2d_f(isize, jsize, ier)

	! 格子を読み込むためのメモリを確保
	allocate(grid_x(isize,jsize), grid_y(isize,jsize))
	! 格子を読み込む
	call cg_iric_getgridcoord2d_f(grid_x, grid_y, ier)

	! 格子点で定義された属性 のメモリを確保
	allocate(elevation(isize, jsize))
	allocate(obstacle(isize - 1, jsize - 1))

	! 属性を読み込む
	call cg_iric_read_grid_real_node_f("Elevation", elevation, ier)
	call cg_iric_read_grid_integer_cell_f("Obstacle", obstacle, ier)

	! 流入口の数を読み込む
	call cg_iric_read_bc_count_f('inflow', inflow_count)

	! 流入口の数に従って、境界条件を保持するメモリを確保。
	allocate(inflow_element_count(inflow_count))
	allocate(discharge_type(inflow_count), discharge_constant(inflow_count))
	allocate(discharge_variable_size(inflow_count))

	! 流入口に指定された格子点の数と、時間依存の流入量のサイズを調べる
	inflow_element_max = 0
	do inflowid = 1, inflow_count
		! 流入口に指定された格子点の数
		call cg_iric_read_bc_indicessize_f('inflow', inflowid, inflow_element_count(inflowid))
		if (inflow_element_max < inflow_element_count(inflowid)) then
			inflow_element_max = inflow_element_count(inflowid)
		end if
		! 流入口の時間依存の流入量のデータの数
		call cg_iric_read_bc_functionalsize_f('inflow', inflowid, 'FunctionalDischarge', discharge_variable_size(inflowid), ier);
		if (discharge_variable_sizemax < discharge_variable_size(inflowid)) then
			discharge_variable_sizemax = discharge_variable_size(inflowid)
		end if
	end do

	! 流入口に指定された格子点と、時間依存の流入量を保持するメモリを確保。
	allocate(inflow_element(inflow_count, 2, inflow_element_max))
	allocate(discharge_variable_time(inflow_count, discharge_variable_sizemax))
	allocate(discharge_variable_value(inflow_count, discharge_variable_sizemax))

	! 境界条件の読み込み
	do inflowid = 1, inflow_count
		! 流入口に指定された格子点
		call cg_iric_read_bc_indices_f('inflow', inflowid, inflow_element(inflowid:inflowid,:,:), ier)
		! 流入量の種類 (0 = 一定、1 = 時間依存)
		call cg_iric_read_bc_integer_f('inflow', inflowid, 'Type', discharge_type(inflowid:inflowid), ier)
		! 流入量 (一定)
		call cg_iric_read_bc_real_f('inflow', inflowid, 'ConstantDischarge', discharge_constant(inflowid:inflowid), ier)
		! 流入量 (時間依存)
		call cg_iric_read_bc_functional_f('inflow', inflowid, 'FunctionalDischarge', discharge_variable_time(inflowid:inflowid,:), discharge_variable_value(inflowid:inflowid,:), ier)
	end do

	allocate(velocity_x(isize,jsize), velocity_y(isize,jsize), depth(isize,jsize), wetflag(isize,jsize))
	iteration = 0
	time = 0
	do
		time = time + timestep
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
		iteration = iteration + 1
		if (iteration > maxiterations) exit
	end do

	! 計算データファイルを閉じる
	call cg_close_f(fin, ier)
	stop
end program SampleProgram
