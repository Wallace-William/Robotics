//============================================================================
// Name        : SendComm.cpp
// Author      : Team 5
//============================================================================

#include <ev3.h>
#include <string>


void PickupBlock(int c){
	if(c == 5){
		OnFwdReg(OUT_A, 100);
		Wait(800);
	}
	if(c == 2){
		OnFwdReg(OUT_A, 100);
		Wait(400);
	}
	Off(OUT_A);
	Wait(500);
	OnRevReg(OUT_B, 100);
	Wait(8000);
	OnFwdReg(OUT_B, 100);
}

void DropBlock(){
	OnFwdReg(OUT_A, 100);
	Wait(200);

	Off(OUT_A);
	OnFwdReg(OUT_B, 100);
	Wait(8000);
	OnRevReg(OUT_B, 100);
	Wait(2000);
	OnRevReg(OUT_B, 0);
}

int main()
{
	InitEV3();
	OnRevReg(OUT_B, 50);
	Wait(2000);
	OnRevReg(OUT_B, 0);
	setAllSensorMode(COL_COLOR, COL_COLOR, NO_SEN, NO_SEN);
	int mode = 0;
	int cBlock = 0;
	while(1){
		//LcdPrintf(1, "M%d;", mode);

		if(mode == 0){
			int rd = readSensor(IN_1);
			if(rd == 5 || rd == 2){
				LcdPrintf(1, "PF %d;", rd);
				PickupBlock(rd);
				cBlock = rd;
				mode = 1;
			}
		}
		if(mode == 1){
			int rd = readSensor(IN_2);
			if(rd == 5 && cBlock == 5){
				LcdPrintf(1, "DP %d;", rd);
				DropBlock();
				mode = 0;
				cBlock = 0;
			}
			if(rd == 2 && cBlock == 2){
				LcdPrintf(1, "DP %d;", rd);
				DropBlock();
				mode = 0;
				cBlock = 0;
			}
		}

		//  LcdPrintf(1, "On;");
		//  OnRevReg(OUT_A, 100);
		//    Wait(1000);
		//    OnRevReg(OUT_A, 0);
		//  PickupBlock();
		//  DropBlock();
		if(ButtonIsDown(BTNCENTER)){
			break;
		}
	}
	FreeEV3();

}

