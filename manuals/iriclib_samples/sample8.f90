program Sample8
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, isize, jsize, ksize, i, j, k, aret
	integer:: condid, indexid
	integer:: condcount, indexlenmax, funcsizemax
	integer:: tmplen
	integer, dimension(:), allocatable:: condindexlen
	integer, dimension(:,:,:), allocatable:: condindices
	integer, dimension(:), allocatable:: intparam
	double precision, dimension(:), allocatable:: realparam
	character(len=200), dimension(:), allocatable:: stringparam
	character(len=200):: tmpstr
	integer, dimension(:), allocatable:: func_size
	double precision, dimension(:,:), allocatable:: func_param;
	double precision, dimension(:,:), allocatable:: func_value;

	! CGNS ファイルのオープン
	call cg_open_f('bctest.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

	! 流入口の数を取得する
	call cg_iric_read_bc_count_f('inflow', condcount)
	! 流入口の数に従って、パラメータの保存用のメモリを確保する。
	allocate(condindexlen(condcount), intparam(condcount), realparam(condcount))
	allocate(stringparam(condcount), func_size(condcount))
	print *, 'condcount ', condcount

	! 境界条件が設定された格子点の数と、関数型の境界条件の最大サイズを調べる
	indexlenmax = 0
	funcsizemax = 0
	do condid = 1, condcount
		call cg_iric_read_bc_indicessize_f('inflow', condid, condindexlen(condid), ier)
		if (indexlenmax < condindexlen(condid)) then
			indexlenmax = condindexlen(condid)
		end if
		call cg_iric_read_bc_functionalsize_f('inflow', condid, 'funcparam', func_size(condid), ier);
		if (funcsizemax < func_size(condid)) then
			funcsizemax = func_size(condid)
		end if
	end do
	
	! 格子点のインデックス格納用の配列と、関数型境界条件の格納用変数のメモリを確保
	allocate(condindices(condcount, 2, indexlenmax))
	allocate(func_param(condcount, funcsizemax), func_value(condcount, funcsizemax))
	! インデックスと、境界条件 を読み込み
	do condid = 1, condcount
		call cg_iric_read_bc_indices_f('inflow', condid, condindices(condid:condid,:,:), ier)
		call cg_iric_read_bc_integer_f('inflow', condid, 'intparam', intparam(condid:condid), ier)
		call cg_iric_read_bc_real_f('inflow', condid, 'realparam', realparam(condid:condid), ier)
		call cg_iric_read_bc_string_f('inflow', condid, 'stringparam', tmpstr, ier)
		stringparam(condid) = tmpstr
		call cg_iric_read_bc_functional_f('inflow', condid, 'funcparam', func_param(condid:condid,:), func_value(condid:condid,:), ier)
	end do

	do condid = 1, condcount
		do indexid = 1, condindexlen(condid)
			print *, 'condindices ', condindices(condid:condid,:,indexid:indexid)
		end do
		print *, 'intparam ', intparam(condid:condid)
		print *, 'realparam ', realparam(condid:condid)
		print *, 'stringparam ', stringparam(condid)
		print *, 'funcparam X ', func_param(condid:condid, 1:func_size(condid))
		print *, 'funcparam Y ', func_value(condid:condid, 1:func_size(condid))
	end do
	
	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample8
