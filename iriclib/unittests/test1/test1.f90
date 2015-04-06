program Test1
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier
	integer:: intval
	double precision:: doubleval
	real:: singleval
	character(len=100):: stringval
	
	integer:: func1size
	double precision, dimension(:), allocatable:: func1time
	double precision, dimension(:), allocatable:: func1value

	integer:: func2size
	double precision, dimension(:), allocatable:: func2time
	double precision, dimension(:), allocatable:: func2value1
	double precision, dimension(:), allocatable:: func2value2

	real, dimension(:), allocatable:: func1timereal
	real, dimension(:), allocatable:: func1valuereal

	integer:: isize, jsize
	double precision, dimension(:,:), allocatable:: grid_x, grid_y
	double precision, dimension(:,:), allocatable:: node_real
	double precision, dimension(:,:), allocatable:: cell_real
	integer, dimension(:,:), allocatable:: node_int
	integer, dimension(:,:), allocatable:: cell_int

	integer:: bctestcount
	integer:: indexlenmax
	integer:: funcsizemax
	integer:: condid
	integer:: indexid

	integer, dimension(:), allocatable:: condindexlen
	integer, dimension(:,:,:), allocatable:: condindices
	integer, dimension(:), allocatable:: intparam
	double precision, dimension(:), allocatable:: realparam
	real, dimension(:), allocatable:: realsingleparam
	character(len=200), dimension(:), allocatable:: stringparam
	character(len=200):: tmpstr
	integer, dimension(:), allocatable:: func_size
	double precision, dimension(:,:), allocatable:: func_param;
	double precision, dimension(:,:), allocatable:: func_value;
	double precision, dimension(:,:), allocatable:: func_withname_param;
	double precision, dimension(:,:), allocatable:: func_withname_value;
	real, dimension(:,:), allocatable:: func_real_param;
	real, dimension(:,:), allocatable:: func_real_value;

	double precision, dimension(:,:), allocatable:: result_real
	integer, dimension(:,:), allocatable:: result_int

	double precision:: result_biter_real
	integer:: result_biter_int

	integer:: i, j, iter
	double precision:: t

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

	call cg_iric_read_integer_f('intval', intval, ier)
	print *, 'intval: ', intval;

	call cg_iric_read_real_f('doubleval', doubleval, ier)
	print *, 'doubleval: ', doubleval;

	call cg_iric_read_realsingle_f('singleval', singleval, ier)
	print *, 'singleval: ', singleval;

	call cg_iric_read_string_f('stringval', stringval, ier)
	print *, 'stringval: ', stringval;

	call cg_iric_read_functionalsize_f('func1', func1size, ier);
	print *, 'func1size: ', func1size;

	allocate(func1time(func1size), func1value(func1size))
	call cg_iric_read_functional_f('func1', func1time, func1value, ier)
	do i = 1, func1size
		print *, 'func1time(', i, '): ', func1time(i)
	end do
	do i = 1, func1size
		print *, 'func1value(', i, '): ', func1value(i)
	end do

	call cg_iric_read_functionalsize_f('func2', func2size, ier);
	print *, 'func2size: ', func2size;

	allocate(func2time(func2size), func2value1(func2size), func2value2(func2size))
	call cg_iric_read_functionalwithname_f('func2', 'time', func2time, ier)
	call cg_iric_read_functionalwithname_f('func2', 'value1', func2value1, ier)
	call cg_iric_read_functionalwithname_f('func2', 'value2', func2value2, ier)
	do i = 1, func2size
		print *, 'func2time(', i, '): ', func2time(i)
	end do
	do i = 1, func2size
		print *, 'func2value1(', i, '): ', func2value1(i)
	end do
	do i = 1, func2size
		print *, 'func2value2(', i, '): ', func2value2(i)
	end do

	call cg_iric_read_functionalsize_f('func1', func1size, ier);
	print *, 'func1size: ', func1size;

	allocate(func1timereal(func1size), func1valuereal(func1size))
	call cg_iric_read_functional_realsingle_f('func1', func1timereal, func1valuereal, ier)
	do i = 1, func1size
		print *, 'func1timereal(', i, '): ', func1timereal(i)
	end do
	do i = 1, func1size
		print *, 'func1valuereal(', i, '): ', func1valuereal(i)
	end do

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

	! 格子点で定義された属性 node_real, node_int のメモリを確保
	allocate(node_real(isize, jsize), node_int(isize, jsize))
	! 属性を読み込む
	call cg_iric_read_grid_real_node_f('node_real', node_real, ier)
	print *, 'node_real: isize, jsize=', isize, jsize
	do i = 1, isize
		do j = 1, jsize
			print *, ' (',i,',',j,')=(',node_real(i,j),')'
		end do
	end do

	call cg_iric_read_grid_integer_node_f('node_int', node_int, ier)
	print *, 'node_int: isize, jsize=', isize, jsize
	do i = 1, isize
		do j = 1, jsize
			print *, ' (',i,',',j,')=(',node_int(i,j),')'
		end do
	end do

	! セルで定義された属性 cell_real, cell_int のメモリを確保。
	! セルの属性なのでサイズは (isize-1) * (jsize-1)
	allocate(cell_real(isize-1, jsize-1), cell_int(isize-1, jsize-1))
	! 属性を読み込む
	call cg_iric_read_grid_real_cell_f('cell_real', cell_real, ier)
	print *, 'cell_real: isize -1, jsize-1=', isize-1, jsize-1
	do i = 1, isize-1
		do j = 1, jsize-1
			print *, ' (',i,',',j,')=(',cell_real(i,j),')'
		end do
	end do

	call cg_iric_read_grid_integer_cell_f('cell_int', cell_int, ier)
	print *, 'cell_int: isize -1, jsize-1=', isize-1, jsize-1
	do i = 1, isize-1
		do j = 1, jsize-1
			print *, ' (',i,',',j,')=(',cell_int(i,j),')'
		end do
	end do

	! 境界条件の数を取得する
	call cg_iric_read_bc_count_f('bctest', bctestcount)
	print *, 'bctestcount: ', bctestcount

	! 境界条件の数に従って、パラメータの保存用のメモリを確保する。
	allocate(condindexlen(bctestcount), intparam(bctestcount), realparam(bctestcount), realsingleparam(bctestcount))
	allocate(stringparam(bctestcount), func_size(bctestcount))

	! 境界条件が設定された格子点の数と、関数型の境界条件の最大サイズを調べる
	indexlenmax = 0
	funcsizemax = 0
	do condid = 1, bctestcount
		call cg_iric_read_bc_indicessize_f('bctest', condid, condindexlen(condid), ier)
		if (indexlenmax < condindexlen(condid)) then
			indexlenmax = condindexlen(condid)
		end if
		call cg_iric_read_bc_functionalsize_f('bctest', condid, 'funcparam', func_size(condid), ier)
		if (funcsizemax < func_size(condid)) then
			funcsizemax = func_size(condid)
		end if
	end do
	
	! 格子点のインデックス格納用の配列と、関数型境界条件の格納用変数のメモリを確保
	allocate(condindices(bctestcount, 2, indexlenmax))
	allocate(func_param(bctestcount, funcsizemax), func_value(bctestcount, funcsizemax))
	allocate(func_withname_param(bctestcount, funcsizemax), func_withname_value(bctestcount, funcsizemax))
	allocate(func_real_param(bctestcount, funcsizemax), func_real_value(bctestcount, funcsizemax))

	! インデックスと、境界条件 を読み込み
	do condid = 1, bctestcount
		call cg_iric_read_bc_indices_f('bctest', condid, &
			condindices(condid:condid, 1:2, 1:condindexlen(condid)), ier)
		call cg_iric_read_bc_integer_f('bctest', condid, &
			'intparam', intparam(condid), ier)
		call cg_iric_read_bc_real_f('bctest', condid, &
			'realparam', realparam(condid), ier)
		call cg_iric_read_bc_realsingle_f('bctest', condid, &
			'realparam', realsingleparam(condid), ier)
		call cg_iric_read_bc_string_f('bctest', condid, &
			'stringparam', tmpstr, ier)
		stringparam(condid) = tmpstr
		call cg_iric_read_bc_functional_f('bctest', condid, 'funcparam', &
			func_param(condid:condid, 1:func_size(condid)), &
			func_value(condid:condid, 1:func_size(condid)), ier)
		call cg_iric_read_bc_functional_realsingle_f( &
			'bctest', condid, 'funcparam', &
			func_real_param(condid:condid, 1:func_size(condid)),  &
			func_real_value(condid:condid, 1:func_size(condid)), ier)
		call cg_iric_read_bc_functionalwithname_f( &
			'bctest', condid, 'funcparam', 'Param', &
			func_withname_param(condid:condid, 1:func_size(condid)), ier)
		call cg_iric_read_bc_functionalwithname_f( &
			'bctest', condid, 'funcparam', 'Value', &
			func_withname_value(condid:condid, 1:func_size(condid)), ier)
	end do

	do condid = 1, bctestcount
		do indexid = 1, condindexlen(condid)
			print *, 'condindices: ', condindices(condid:condid,:,indexid:indexid)
		end do
		print *, 'intparam: ', intparam(condid:condid)
		print *, 'realparam: ', realparam(condid:condid)
		print *, 'stringparam: ', stringparam(condid)
		print *, 'funcparam X: '
		do i = 1, func_size(condid)
			print *, i, ": ",func_param(condid, i)
		end do
		print *, 'funcparam Y: '
		do i = 1, func_size(condid)
			print *, i, ": ",func_value(condid, i)
		end do
		print *, 'funcparam_real X: '
		do i = 1, func_size(condid)
			print *, i, ": ",func_real_param(condid, i)
		end do
		print *, 'funcparam_real Y: '
		do i = 1, func_size(condid)
			print *, i, ": ",func_real_value(condid, i)
		end do

		print *, 'funcparam_withname X: '
		do i = 1, func_size(condid)
			print *, i, ": ",func_withname_param(condid, i)
		end do
		print *, 'funcparam_withname Y: '
		do i = 1, func_size(condid)
			print *, i, ": ",func_withname_value(condid, i)
		end do
	end do

	! allocate で確保したメモリを開放
	deallocate(node_real, node_int, cell_real, cell_int)

	! 計算結果を出力
	allocate(result_int(isize, jsize), result_real(isize, jsize))
	do i = 1, isize
		do j = 1, jsize
			result_int(i, j) = i + j
			result_real(i, j) = i * j
		end do
	end do
	do iter = 1, 5
		t = iter * 2.1
		call cg_iric_write_sol_time_f(t, ier)
		result_biter_int = t * 3
		result_biter_real = t * 0.1
		call cg_iric_write_sol_baseiterative_integer_f('result_biter_int', result_biter_int, ier)
		call cg_iric_write_sol_baseiterative_real_f('result_biter_real', result_biter_real, ier)
		grid_x(1, 1) = grid_x(1, 1) + 0.1
		call cg_iric_write_sol_gridcoord2d_f(grid_x, grid_y, ier)
		call cg_iric_write_sol_integer_f('result_int', result_int, ier)
		call cg_iric_write_sol_real_f('result_real', result_real, ier)
	end do

	call cg_iric_write_errorcode_f(2, ier)

	deallocate(grid_x, grid_y)

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Test1
