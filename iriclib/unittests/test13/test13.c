#include "cgnslib.h"
#include <iriclib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv) {
	int fin, ier;
	int locked;
	int canceled;
	char* fname = "test.cgn";
	printf("HELLO\n");

	// CGNS ファイルのオープン
	ier = cg_open(fname, CG_MODE_MODIFY, &fin);
	if (ier != 0){
		printf("*** Open error of CGNS file ***\n");
		return 1;
	}

	// iRIClib の初期化
	ier = cg_iRIC_Init(fin);

	cg_close(fin);

	// Cancel に対応していることを通知
	ier = iRIC_InitOption(IRIC_OPTION_CANCEL);

/*
	// 結果の書き込みを開始
	ier = iRIC_Write_Sol_Start(fname);
	printf("iRIC_Write_Sol_Start: %d", ier);

	// 結果の書き込みを終了
	ier = iRIC_Write_Sol_End(fname);
	printf("iRIC_Write_Sol_End: %d", ier);
*/
	// ロックされているかを確認
	locked = iRIC_Check_Lock(fname);

	// キャンセルされているかを確認
	canceled = iRIC_Check_Cancel();

	return 0;
}
