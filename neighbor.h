
PROCTYPE inline int UL(int i, int * res){
	switch(i){
		case 1: case 2: case 3:
		case 9: case 10: case 11:
		case 17: case 18: case 19:
		case 25: case 26: case 27:
			*res = i +3;
			return 1;
		case 4: case 5: case 6: case 7:
		case 12: case 13: case 14: case 15:
		case 20: case 21: case 22: case 23:
			*res = i+4;
			return 1;
		default: 
			return 0;
	}
}
PROCTYPE inline int UR(int i, int * res){
	switch(i){
		case 0: case 1: case 2: case 3:
		case 8: case 9: case 10: case 11:
		case 16: case 17: case 18: case 19:
		case 24: case 25: case 26: case 27:
			*res = i+4;
			return 1;
		case 4: case 5: case 6:
		case 12: case 13: case 14:
		case 20: case 21: case 22:
			*res = i+5;
			return 1;
		default:
			return 0;
	}
}
PROCTYPE inline int DL(int i, int * res){
	switch(i){
		case 4: case 5: case 6: case 7:
		case 12: case 13: case 14: case 15:
		case 20: case 21: case 22: case 23:
		case 28: case 29: case 30: case 31:
			*res = i-4;
			return 1;
		case 9: case 10: case 11:
		case 17: case 18: case 19:
		case 25: case 26: case 27:
			*res = i-5;
			return 1;
		default:
			return 0;
	}
}
PROCTYPE inline int DR(int i, int * res){
	switch(i){
		case 4: case 5: case 6:
		case 12: case 13: case 14:
		case 20: case 21: case 22:
		case 28: case 29: case 30:
			*res = i-3;
			return 1;
		case 8: case 9: case 10: case 11:
		case 16: case 17: case 18: case 19:
		case 24: case 25: case 26: case 27:
			*res = i-4;
			return 1;
		default:
			return 0;
	}
}
