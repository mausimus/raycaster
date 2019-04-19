#pragma once
#include "RayCaster.h"
class RayCasterFixed :
	public RayCaster
{
protected:
	uint8_t* _map;
	uint16_t _px;
	uint16_t _py;
	int16_t _pa;
	uint8_t _qtr;
	uint8_t _qa;

	uint16_t _tan[256] = { 0,1,3,4,6,7,9,11,12,14,15,17,18,20,22,23,25,26,28,29,31,33,34,36,37,39,41,42,44,46,47,49,50,52,54,55,57,59,60,62,64,65,67,69,70,72,74,75,77,79,81,82,84,86,88,89,91,93,95,96,98,100,102,104,106,107,109,111,113,115,117,119,121,123,124,126,128,130,132,134,136,138,140,142,145,147,149,151,153,155,157,159,162,164,166,168,171,173,175,177,180,182,185,187,189,192,194,197,199,202,204,207,210,212,215,218,220,223,226,229,232,234,237,240,243,246,249,252,255,259,262,265,268,272,275,278,282,285,289,293,296,300,304,308,311,315,319,323,328,332,336,340,345,349,354,358,363,368,373,378,383,388,393,398,404,409,415,421,427,433,439,445,451,458,465,471,478,486,493,500,508,516,524,532,541,549,558,568,577,587,597,607,618,628,640,651,663,675,688,701,715,729,744,759,774,791,808,825,843,862,882,903,925,947,971,996,1022,1049,1077,1108,1140,1173,1209,1246,1286,1329,1374,1423,1475,1531,1591,1655,1725,1801,1884,1975,2075,2185,2308,2445,2599,2773,2972,3202,3470,3787,4166,4631,5210,5956,6950,8341,10428,13905,20859,41720 };

	uint16_t _cotan[256] = { 0,41720,20859,13905,10428,8341,6950,5956,5210,4631,4166,3787,3470,3202,2972,2773,2599,2445,2308,2185,2075,1975,1884,1801,1725,1655,1591,1531,1475,1423,1374,1329,1286,1246,1209,1173,1140,1108,1077,1049,1022,996,971,947,925,903,882,862,843,825,808,791,774,759,744,729,715,701,688,675,663,651,640,628,618,607,597,587,577,568,558,549,541,532,524,516,508,500,493,486,478,471,465,458,451,445,439,433,427,421,415,409,404,398,393,388,383,378,373,368,363,358,354,349,345,340,336,332,328,323,319,315,311,308,304,300,296,293,289,285,282,278,275,272,268,265,262,259,256,252,249,246,243,240,237,234,232,229,226,223,220,218,215,212,210,207,204,202,199,197,194,192,189,187,185,182,180,177,175,173,171,168,166,164,162,159,157,155,153,151,149,147,145,142,140,138,136,134,132,130,128,126,124,123,121,119,117,115,113,111,109,107,106,104,102,100,98,96,95,93,91,89,88,86,84,82,81,79,77,75,74,72,70,69,67,65,64,62,60,59,57,55,54,52,50,49,47,46,44,42,41,39,37,36,34,33,31,29,28,26,25,23,22,20,18,17,15,14,12,11,9,7,6,4,3,1 };

	uint8_t _sin[256] = { 0,1,3,4,6,7,9,10,12,14,15,17,18,20,21,23,25,26,28,29,31,32,34,36,37,39,40,42,43,45,46,48,49,51,53,54,56,57,59,60,62,63,65,66,68,69,71,72,74,75,77,78,80,81,83,84,86,87,89,90,92,93,95,96,97,99,100,102,103,105,106,108,109,110,112,113,115,116,117,119,120,122,123,124,126,127,128,130,131,132,134,135,136,138,139,140,142,143,144,146,147,148,149,151,152,153,155,156,157,158,159,161,162,163,164,166,167,168,169,170,171,173,174,175,176,177,178,179,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,217,218,219,220,221,221,222,223,224,225,225,226,227,227,228,229,230,230,231,232,232,233,234,234,235,235,236,237,237,238,238,239,239,240,241,241,242,242,243,243,244,244,244,245,245,246,246,247,247,247,248,248,249,249,249,250,250,250,251,251,251,251,252,252,252,252,253,253,253,253,254,254,254,254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255 };

	uint8_t _cos[256] = { 0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,254,254,254,254,253,253,253,253,252,252,252,252,251,251,251,251,250,250,250,249,249,249,248,248,247,247,247,246,246,245,245,244,244,244,243,243,242,242,241,241,240,239,239,238,238,237,237,236,235,235,234,234,233,232,232,231,230,230,229,228,227,227,226,225,225,224,223,222,221,221,220,219,218,217,217,216,215,214,213,212,211,211,210,209,208,207,206,205,204,203,202,201,200,199,198,197,196,195,194,193,192,191,190,189,188,187,186,185,184,183,182,181,179,178,177,176,175,174,173,171,170,169,168,167,166,164,163,162,161,159,158,157,156,155,153,152,151,149,148,147,146,144,143,142,140,139,138,136,135,134,132,131,130,128,127,126,124,123,122,120,119,117,116,115,113,112,110,109,108,106,105,103,102,100,99,97,96,95,93,92,90,89,87,86,84,83,81,80,78,77,75,74,72,71,69,68,66,65,63,62,60,59,57,56,54,53,51,49,48,46,45,43,42,40,39,37,36,34,32,31,29,28,26,25,23,21,20,18,17,15,14,12,10,9,7,6,4,3,1 };

	uint8_t _neard[256] = { 100,98,96,95,94,92,91,90,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,71,70,69,68,68,67,66,65,65,64,64,63,62,62,61,60,60,59,59,58,58,57,57,56,56,55,55,54,54,53,53,52,52,52,51,51,50,50,50,49,49,48,48,48,47,47,47,46,46,46,45,45,45,44,44,44,43,43,43,42,42,42,42,41,41,41,41,40,40,40,40,39,39,39,39,38,38,38,38,37,37,37,37,36,36,36,36,36,35,35,35,35,35,34,34,34,34,34,34,33,33,33,33,33,32,32,32,32,32,32,32,31,31,31,31,31,31,30,30,30,30,30,30,30,29,29,29,29,29,29,29,28,28,28,28,28,28,28,28,27,27,27,27,27,27,27,27,27,26,26,26,26,26,26,26,26,26,25,25,25,25,25,25,25,25,25,25,24,24,24,24,24,24,24,24,24,24,23,23,23,23,23,23,23,23,23,23,23,23,22,22,22,22,22,22,22,22,22,22,22,22,21,21,21,21,21,21,21,21,21,21,21,21,21,21,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20 };

	uint8_t _fard[256] = { 100,88,80,72,66,61,57,53,50,47,44,42,40,38,36,34,33,32,30,29,28,27,26,25,25,24,23,22,22,21,21,20,20,19,19,18,18,17,17,17,16,16,16,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,12,11,11,11,11,11,11,10,10,10,10,10,10,10,10,9,9,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3 };

	uint16_t _da[SCREEN_WIDTH] = { 912,913,914,915,915,916,917,917,918,919,919,920,921,922,922,923,924,924,925,926,926,927,928,929,929,930,931,931,932,933,934,934,935,936,936,937,938,938,939,940,941,941,942,943,943,944,945,945,946,947,947,948,949,950,950,951,952,952,953,954,954,955,956,957,957,958,959,959,960,961,961,962,963,964,964,965,966,966,967,968,968,969,970,971,971,972,973,973,974,975,975,976,977,978,978,979,980,980,981,982,983,983,984,985,985,986,987,987,988,989,989,990,991,992,992,993,994,994,995,996,996,997,998,999,999,1000,1001,1001,1002,1003,1004,1004,1005,1006,1006,1007,1008,1008,1009,1010,1010,1011,1012,1013,1013,1014,1015,1015,1016,1017,1017,1018,1019,1020,1020,1021,1022,1022,1023,0,0,0,1,2,2,3,4,4,5,6,7,7,8,9,9,10,11,11,12,13,14,14,15,16,16,17,18,18,19,20,20,21,22,23,23,24,25,25,26,27,28,28,29,30,30,31,32,32,33,34,35,35,36,37,37,38,39,39,40,41,41,42,43,44,44,45,46,46,47,48,49,49,50,51,51,52,53,53,54,55,56,56,57,58,58,59,60,60,61,62,63,63,64,65,65,66,67,67,68,69,70,70,71,72,72,73,74,74,75,76,77,77,78,79,79,80,81,81,82,83,83,84,85,86,86,87,88,88,89,90,90,91,92,93,93,94,95,95,96,97,98,98,99,100,100,101,102,102,103,104,105,105,106,107,107,108,109,109,110,111 };

	void Distance(uint16_t _sx, uint16_t _sy, uint16_t ra, int16_t *dx, int16_t *dy);

	bool IsWall(uint8_t bx, uint8_t by, uint8_t sx, uint8_t sy);

	int16_t MulTan(uint8_t v, uint8_t i, uint8_t qtr, uint8_t a, uint16_t* fun);

	int16_t AbsTan(uint8_t qtr, uint8_t a, uint16_t* fun);

	uint16_t Mul(uint8_t v, uint16_t f);

	int16_t MulS(uint8_t v, int16_t f);

	uint8_t Height(uint16_t d);

	void Precalculate();

public:
	void Start(uint16_t px, uint16_t py, int16_t pa);

	void Trace(uint16_t x, uint8_t * sso, uint8_t * tso, uint16_t * tst);

	RayCasterFixed(uint8_t* map);
	~RayCasterFixed();
};

