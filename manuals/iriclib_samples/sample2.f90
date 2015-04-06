program Sample2
	implicit none
	include 'cgnslib_f.h'

	integer:: fin, ier, discharge_size, i
	double precision, dimension(:), allocatable:: discharge_time, discharge_value  ! discharge の時刻と値を保持する配列

	! CGNS ファイルのオープン
	call cg_open_f('test.cgn', CG_MODE_MODIFY, fin, ier)
	if (ier /=0) STOP "*** Open error of CGNS file ***"

	! iRIClib の初期化
	call cg_iric_init_f(fin, ier)
	if (ier /=0) STOP "*** Initialize error of CGNS file ***"

	! まず、関数型の入力条件のサイズを調べる
	call cg_iric_read_functionalsize_f('discharge', discharge_size, ier)
	! メモリを確保
	allocate(discharge_time(discharge_size), discharge_value(discharge_size))
	! 確保したメモリに値を読み込む
	call cg_iric_read_functional_f('discharge', discharge_time, discharge_value, ier)

	! （出力）
	if (ier ==0) then
		print *, 'discharge: discharge_size=', discharge_size
		do i = 1, min(discharge_size, 5)
			print *, ' i,time,value:', i, discharge_time(i), discharge_value(i)
		end do
	end if

	! allocate で確保したメモリを開放
	deallocate(discharge_time, discharge_value)

	! CGNS ファイルのクローズ
	call cg_close_f(fin, ier)
	stop
end program Sample2
