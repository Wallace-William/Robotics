//============================================================================
// Name        : TestComms.cpp
// Author      : Team 5
//============================================================================

#include <ev3.h>
#include <string>
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;


int mode = 0;
int curr_rb = 0;



int sonar, bumpL, bumpR;

//ORIENT:
//N - 0
//E - 1
//S - 2
//W - 3

int orient = 0;

class landmark {
public:
	pair<int, int> coord;
	vector<int> directionsInspected;
	//N, E, S, W
	//wall - (1), not inspected - (0), obstacle - (2)

	landmark(pair<int, int> c) {
		coord = c;
		directionsInspected.resize(4);
		for (int i = 0; i < 4; ++i) {
			directionsInspected[i] = 0;
		}
	}
	landmark() {
		pair<int, int> x(-100, -100);
		coord = x;
	}
};

vector<landmark> landmarks;



//MODES:
//0 - wander and look for closest obstacle
//

pair<int, int> current(0, 0);
int currentDir = 0;

int searchInLandmarks(pair<int, int> coord) {
	int find = -1;
	for (int i = 0; i < landmarks.size(); ++i) {
		if (landmarks[i].coord.first == coord.first
				&& landmarks[0].coord.second == coord.second) {
			find = i;
			break;
		}
	}
	return find;
}

void ReadSensors();

void Left(int count) {
	LcdPrintf(1, "L;");
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	while (MotorRotationCount(OUT_A) < 70) {
		if (ButtonIsDown(BTNCENTER))
			break;
		OnFwdSync(OUT_AB, 15);
	}
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	OnFwdReg(OUT_B, 15);
	OnRevReg(OUT_A, 15);
	while (MotorRotationCount(OUT_A) > -count) {
		if (ButtonIsDown(BTNCENTER))
			break;
	}
	OnFwdSync(OUT_AB, 0);
	Wait(750);

}

void Left() {
	LcdPrintf(1, "L;");
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	while (MotorRotationCount(OUT_A) < 70) {
		if (ButtonIsDown(BTNCENTER))
			break;
		OnFwdSync(OUT_AB, 15);
	}
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	OnFwdReg(OUT_B, 15);
	OnRevReg(OUT_A, 15);
	while (MotorRotationCount(OUT_A) > -235) {
		if (ButtonIsDown(BTNCENTER))
			break;
	}
	OnFwdSync(OUT_AB, 0);
	currentDir = (currentDir == 0 ? 3 : currentDir-1);
	Wait(750);

}

void Right(int count) {
	LcdPrintf(1, "R;");
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	while (MotorRotationCount(OUT_A) < 20) {
		if (ButtonIsDown(BTNCENTER))
			break;
		OnFwdSync(OUT_AB, 15);
	}
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	OnRevReg(OUT_B, 15);
	OnFwdReg(OUT_A, 15);
	while (MotorRotationCount(OUT_A) < count) {
		if (ButtonIsDown(BTNCENTER))
			break;
	}
	OnFwdSync(OUT_AB, 0);

	Wait(350);

}

void Right() {
	LcdPrintf(1, "R;");
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	while (MotorRotationCount(OUT_A) < 20) {
		if (ButtonIsDown(BTNCENTER))
			break;
		OnFwdSync(OUT_AB, 15);
	}
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	OnRevReg(OUT_B, 15);
	OnFwdReg(OUT_A, 15);
	while (MotorRotationCount(OUT_A) < 248) {
		if (ButtonIsDown(BTNCENTER))
			break;
	}
	OnFwdSync(OUT_AB, 0);
	currentDir = (currentDir < 3 ? currentDir+1 : 0);
	Wait(350);

}
int tileTrigger = 0;

void Forward(int count) {
	ResetRotationCount(OUT_A);
			ResetRotationCount(OUT_B);
	OnRevSync(OUT_AB, 25);
	while (MotorRotationCount(OUT_A) > -count) {
		ReadSensors();
	}
	if (MotorRotationCount(OUT_A) < -500) {
		tileTrigger = 1;
		ResetRotationCount(OUT_A);
		ResetRotationCount(OUT_B);
	}
	ReadSensors();
	OnFwdSync(OUT_AB, 0);
}

void Forward() {
	if (MotorRotationCount(OUT_A) < -500) {

		tileTrigger = 1;
		ResetRotationCount(OUT_A);
		ResetRotationCount(OUT_B);

	}
	ReadSensors();
	OnRevSync(OUT_AB, 25);

	//OnFwdSync(OUT_AB, 0);
}

void Reverse(int count) {
	ResetRotationCount(OUT_A);
	ResetRotationCount(OUT_B);
	OnFwdSync(OUT_AB, 20);
	while (MotorRotationCount(OUT_A) < count) {

	}
	OnFwdSync(OUT_AB, 0);
}

int nextDir = 0;

int adjustSonar() {
	if (sonar < 100) {
		Left();
		Forward(100);
		Right();
		return 1;

	}
	return 0;
}


void inspectedSonar(landmark l, int dir, int sDist) {
	if (sDist < 110 && sDist > 3)
		l.directionsInspected[dir] = 1;

	if (sDist > 110) {
		if (sDist < 400 && sDist > 340) {
			int x = current.first;
			int y = current.second;
			if (dir == 1) {
				x++;
			}
			if (dir == 2) {
				y--;
			}
			if (dir == 3) {
				x--;
			}
			if (dir == 0) {
				y++;
			}
			pair<int, int> looking(x, y);
			int landmarkVec = searchInLandmarks(looking);
			landmark l1;
			if (landmarkVec > -1)
				l1 = landmarks[landmarkVec];
			else
				l1 = landmark(looking);
			l1.directionsInspected[dir] = 2;
			landmarks.push_back(l1);

		}
	}
}

void headToDirection(int tiles, int dirHead) {

}

void SlowlyPickup(){
	Reverse(150);
	Wait(500);
	Reverse(50);
		Wait(500);
		Reverse(50);
			Wait(500);
			Reverse(50);
				Wait(500);
				Reverse(50);
					Wait(500);
					Reverse(050);
										Wait(100);
										Reverse(50);
															Wait(500);

															Wait(3000);



}

void SlowlyDrop(){
	Reverse(150);
	Wait(500);
	Reverse(50);
		Wait(500);
		Reverse(50);
			Wait(500);
			Reverse(50);
				Wait(500);
				Reverse(50);
Wait(3000);




}


pair<int, int> redSpot(1,1);
pair<int, int> blueSpot(0,1);



void BlockProcedure(){
	Reverse(150);
	bumpL = readSensor(IN_2);
		bumpR = readSensor(IN_3);
		sonar = readSensor(IN_4);

		adjustSonar();
	bumpL = readSensor(IN_2);
		bumpR = readSensor(IN_3);
		sonar = readSensor(IN_4);
	Right();
	bumpL = readSensor(IN_2);
		bumpR = readSensor(IN_3);
		sonar = readSensor(IN_4);
	adjustSonar();
	bumpL = readSensor(IN_2);
		bumpR = readSensor(IN_3);
		sonar = readSensor(IN_4);
	Right();

	redSpot.first = current.first;
	redSpot.second = current.second;
	LcdPrintf(0, "R:%d,%d;", redSpot.first, redSpot.second);
	//Wait(5000);
}


void ReadMorseCode() {
	int trig1 = readSensor(IN_1);
	if (trig1) {
		Off(OUT_AB);

		Wait(220);
		int trig2 = readSensor(IN_1);
		if (trig2 && mode == 0) {
			Wait(220);
			int trig3 = readSensor(IN_1);
			if (trig3) {
				curr_rb = 5;
				mode = 1;
			} else {
				curr_rb = 2;
				mode = 1;

			}
			//FindPathToColor(curr_rb);
			BlockProcedure();
			SlowlyPickup();
			LcdPrintf(1, "BP:%d;", curr_rb);
			Wait(300);

		} else {
			BlockProcedure();
			SlowlyDrop();
			LcdPrintf(1, "DB:%d;", curr_rb);
			//mode = 0;
		}

	}
}
void ReadSensors() {
	Wait(100);
	bumpL = readSensor(IN_2);
	bumpR = readSensor(IN_3);
	sonar = readSensor(IN_4);
	ReadMorseCode();
}

void Mode() {
	ReadMorseCode();
	if (tileTrigger) {
		tileTrigger = 0;
		int x = current.first;
		int dir = currentDir;
		int y = current.second;
		if (dir == 1) {
			x++;
		}
		if (dir == 2) {
			y--;
		}
		if (dir == 3) {
			x--;
		}
		if (dir == 0) {
			y++;
		}
		pair<int, int> looking(x, y);
		current = looking;
		//LcdPrintf(1, "(%d, %d);", current.first, current.second);
	}


	if (sonar > 700) {
					OnFwdSync(OUT_AB, 0);
					Forward(250);
					ReadSensors();
					Wait(1000);
					if(sonar < 700){
						Reverse(350);
					}
					ReadSensors();
					Right();
					ReadSensors();
					Forward(350);
					ReadSensors();
				}
	if (mode == 0) {
		//find lowest sonar value

		int landmarkVec = searchInLandmarks(current);
		landmark l;
		if (landmarkVec > -1)
			l = landmarks[landmarkVec];
		else
			l = landmark(current);
		int minimumSonar = 2550;
		int minimumDirection = -1;
		for (int j = 0; j < 4; ++j) {
			ReadSensors();
			ReadSensors();
			//LcdPrintf(1, "%d-Sonar:%d\n", j, sonar);
			if (adjustSonar()) {
				ReadSensors();
				--j;
				continue;
			}
			inspectedSonar(l, (j < 3 ? j + 1 : 0), sonar);
			if (sonar < minimumSonar && l.directionsInspected[(j < 3 ? j + 1 : 0)] == 0) {

				minimumSonar = sonar;
				minimumDirection = (j < 3 ? j + 1 : 0);
				ReadSensors();
			}
			Right();
			ReadSensors();
			LcdPrintf(1, "%d;", l.directionsInspected[(j < 3 ? j + 1 : 0)]);
		}

		if (landmarkVec > -1)
					landmarks[landmarkVec] = l;
		else
					landmarks.push_back(l);


		for (int i = 0; i < landmarks.size(); ++i) {
			//LcdPrintf(0, "(%d, %d);", landmarks[i].coord.first, landmarks[i].coord.second);
		//		break;

		}
		//Wait(10000);
		mode = 1;
		nextDir = minimumDirection;

		//		Wait(10000);

	}
	if (mode == 1) {

//		if(sonar > 110 && sonar < 150){
//			OnFwdSync(OUT_AB, 0);
//										Reverse(150);
//										ReadSensors();
//										Right(45);
//										ReadSensors();
//		}
		//go to good direction and check for color and bump
		if (nextDir == 1) {
			ReadSensors();
			adjustSonar();
			Right();
		}
		if (nextDir == 2) {
			ReadSensors();
			adjustSonar();
			Right();
			ReadSensors();
			adjustSonar();
			Right();
		}
		if (nextDir == 3) {
			ReadSensors();
			adjustSonar();
			Right();
			ReadSensors();
			adjustSonar();
			Right();
			ReadSensors();
			adjustSonar();
			Right();
		}
		if (nextDir == 0) {
			//ReadSensors();
			//adjustSonar();
			Forward();
		}
		//LcdPrintf(1, "H:%d;", currentDir);

		ReadSensors();

		if (bumpL || bumpR) {
			Wait(650);
			ReadSensors();
			Off(OUT_AB);
			if (bumpL && bumpR) {
				int landmarkVec = searchInLandmarks(current);
						landmark l;
						if (landmarkVec > -1)
							l = landmarks[landmarkVec];
						else
							l = landmark(current);
						l.directionsInspected[nextDir] = 1;
					for(int i = 0; i < 4; ++i){
						LcdPrintf(1, "[%d,%d];", i, l.directionsInspected[i]);
					}
				Reverse(100);
				ReadSensors();

				if(sonar < 600)
					Left();
				else
					Right();

				nextDir = 0;
				//mode = 0;
				return;
			}
			if (bumpL) {
				Reverse(100);
				ReadSensors();
				if(sonar < 150){
					Left(125);
				}
				else{
				Right(135);
				}
			}
			if (bumpR) {
				Reverse(100);

				Left(135);
			}
		}
		if (sonar < 40) {
									OnFwdSync(OUT_AB, 0);
									Reverse(150);
									ReadSensors();
									Left(10);
									ReadSensors();
								}
		nextDir = 0;
	}

	if(mode == 2){
		//Wall Follow
		ReadSensors();
		if(sonar > 100){
			Right(15);
		}
		if(sonar < 40){
			Left(15);
		}
		pair<int, int> looking(current.first, current.second);
		int landmarkVec = searchInLandmarks(looking);
		landmark l1;
		if (landmarkVec > -1)
			l1 = landmarks[landmarkVec];
		else
			l1 = landmark(looking);
		l1.directionsInspected[(currentDir < 3 ? currentDir + 1 : 0)] = 1;
		landmarks.push_back(l1);

	}
}

int main() {
	InitEV3();
	setAllSensorMode(TOUCH_PRESS, TOUCH_PRESS, TOUCH_PRESS, US_DIST_MM);

	mode = 0;

	while (1) {

		Mode();


		if (ButtonIsDown(BTNCENTER)) {
			break;
		}
	}

	FreeEV3();
}

