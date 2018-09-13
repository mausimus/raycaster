// floating-point implementation for testing/comparison

#define _USE_MATH_DEFINES

#include "RayCasterFloat.h"
#include <math.h>

void RayCasterFloat::HitLocation(float sx, float sy, float a, float *hx, float *hy, int *dx, int *dy)
{
	// sx == 0..1, sy == 0..1, a == 0..2pi

	if (a <= M_PI_2)
	{
		if (sx == 1)
		{
			sx = 0;
			*dx = 1;
		}
		if (sy == 1)
		{
			sy = 0;
			*dy = 1;
		}
		HitLocation1(sx, sy, a, hx, hy);
	}
	else if (a <= M_PI)
	{
		if (sx == 1)
		{
			sx = 0;
			*dx = 1;
		}
		if (sy == 0)
		{
			sy = 1;
			*dy = -1;
		}
		// flip vertical
		sy = 1 - sy;
		a = M_PI - a;
		HitLocation1(sx, sy, a, hx, hy);
		a = M_PI - a;
		sy = 1 - sy;
		*hy = 1 - *hy;
	}
	else if (a <= 3 * M_PI_2)
	{
		if (sx == 0)
		{
			sx = 1;
			*dx = -1;
		}
		if (sy == 0)
		{
			sy = 1;
			*dy = -1;
		}
		// flip both
		sx = 1 - sx;
		a = 2 * M_PI - a;
		sy = 1 - sy;
		a = M_PI - a;
		HitLocation1(sx, sy, a, hx, hy);
		a = M_PI - a;
		sy = 1 - sy;
		*hy = 1 - *hy;
		a = 2 * M_PI - a;
		sx = 1 - sx;
		*hx = 1 - *hx;
	}
	else
	{
		if (sx == 0)
		{
			sx = 1;
			*dx = -1;
		}
		if (sy == 1)
		{
			sy = 0;
			*dy = 1;
		}
		// flip horizontal
		sx = 1 - sx;
		a = 2 * M_PI - a;
		HitLocation1(sx, sy, a, hx, hy);
		a = 2 * M_PI - a;
		sx = 1 - sx;
		*hx = 1 - *hx;
	}
}

void RayCasterFloat::HitLocation1(float sx, float sy, float a, float *hx, float *hy)
{
	if (a == 0)
	{
		*hx = sx;
		*hy = 1;
		return;
	}
	// a is less than pi/2
	// all other numbers 0..1
	if (/*a > M_PI_2 ||*/ sx < 0 || sy < 0 || sx > 1 || sy > 1)
	{
		throw;
	}

	// tan(a) = w/h
	float h = 1.0f - sy;
	float w = h * tan(a);
	if (sx + w <= 1.0f)
	{
		// hit top
		*hy = 1;
		*hx = sx + w;
	}
	else {
		// hit right
		*hx = 1;
		w = 1.0f - sx;
		h = w / tan(a);
		*hy = sy + h;
	}
}

float RayCasterFloat::Distance(float px, float py, float ra)
{
	Start(static_cast<uint16_t>(px * 256), static_cast<uint16_t>(py * 256), static_cast<int16_t>(ra * 1024 / (2.0 * M_PI)));

	float rx, ry;

	rx = px;
	ry = py;

	while (ra < 0)
	{
		ra += 2.0f * M_PI;
	}

	while (rx >= 0 && ry >= 0 && rx < MAP_X && ry < MAP_Y)
	{
		int dx = 0;
		int dy = 0;
		float hx = 0;
		float hy = 0;

		float bx = 0;
		float by = 0;
		float sx = modff(rx, &bx);
		float sy = modff(ry, &by);

		if (IsWall(rx, ry, ra))
		{
			break;
		}

		HitLocation(sx, sy, ra, &hx, &hy, &dx, &dy);

		rx = bx + hx + dx;
		ry = by + hy + dy;
	}

	float dx = rx - px;
	float dy = ry - py;

	Finish(static_cast<uint16_t>(rx * 256), static_cast<uint16_t>(ry * 256), static_cast<int16_t>(dx * 256), static_cast<int16_t>(dy * 256));

	float d = sqrt(dx * dx + dy * dy);

	return d;
}

bool RayCasterFloat::IsWall(float rx, float ry, float ra)
{
	float x = 0;
	float y = 0;
	float sx = modff(rx, &x);
	float sy = modff(ry, &y);

	int ibx = static_cast<int>(x);
	int iby = static_cast<int>(y);

	Step(static_cast<uint16_t>(rx * 256), static_cast<uint16_t>(ry * 256));

	if (ibx <= 1 || iby <= 1 || ibx >= MAP_X - 1 || iby >= MAP_Y - 1)
	{
		return true;
	}

	if (_map[ibx + MAP_X * iby] == 0)
	{/*
		if (sx == 0)
		{
			*isRight = 1;
			ibx--;
		}
		if (sy == 0)
		{
			*isTop = 1;
			iby--;
		}

		if (ibx <= 1 || iby <= 1 || ibx >= MAP_X - 1 || iby >= MAP_Y - 1)
		{
			return true;
		}
		*/
		return _map[ibx + MAP_X * iby] == 1;
	}
	return true;
}

float RayCasterFloat::Distance2(float px, float py, float ra)
{
	Start(static_cast<uint16_t>(px * 256), static_cast<uint16_t>(py * 256), static_cast<int16_t>(ra * 1024 / (2.0 * M_PI)));

	float rx, ry;

	rx = px;
	ry = py;

	while (ra < 0)
	{
		ra += 2.0f * M_PI;
	}
	while (ra >= 2.0f * M_PI)
	{
		ra -= 2.0f * M_PI;
	}

	// xStep/yStep=tan(a)

	int tileStepX = 1;
	int tileStepY = 1;
	if (ra > M_PI)
	{
		tileStepX = -1;
	}
	if (ra > M_PI_2 && ra < 3 * M_PI_2)
	{
		tileStepY = -1;
	}

	//float yIntercept = ry + yStep; // next intercept with vertical line
	//float xIntercept = rx + xStep; // next intercept with horizontal line

	float x = 0;
	float y = 0;
	float sx = modff(rx, &x);
	float sy = modff(ry, &y);

	float sdx, sdy;
	if (ra <= M_PI_2)
	{
		sdx = (1 - sy) * tan(ra);
		sdy = (1 - sx) / tan(ra);
	}
	else if (ra <= M_PI)
	{
		if (sy == 0)
		{
			sdx = (1) * fabs(tan(ra));
		}
		else
		{
			sdx = (sy) * fabs(tan(ra));
		}
		sdy = -(1 - sx) / fabs(tan(ra));
	}
	else if (ra < 3 * M_PI_2)
	{
		if (sy == 0)
		{
			sdx = -(1) * fabs(tan(ra));
		}
		else
		{
			sdx = -(sy)* fabs(tan(ra));
		}
		if (sx == 0)
		{
			sdy = -(1) / fabs(tan(ra));
		}
		else
		{
			sdy = -(sx) / fabs(tan(ra));
		}
	}
	else
	{
		sdx = -(1 - sy) * fabs(tan(ra));
		if (sx == 0)
		{
			sdy = (1) / fabs(tan(ra));
		}
		else
		{
			sdy = (sx) / fabs(tan(ra));
		}
	}

	float xIntercept = rx + sdx;
	float yIntercept = ry + sdy;

	float xStep = fabs(tan(ra)) * tileStepX;
	float yStep = fabs(1 / tan(ra)) * tileStepY;

	bool hitVert = false;
	bool hitHoriz = false;
	bool somethingDone = false;

	if (yStep * tileStepY < 0 || xStep * tileStepX < 0)
	{
		int f = 4;
	}

	int isTop = 0;
	int isRight = 0;

	do
	{
		somethingDone = false;
		while ((
			(tileStepY == 1 && (yIntercept <= y + 1)) ||
			(tileStepY == -1 && (yIntercept >= y))
			))
		{
			somethingDone = true;
			x += tileStepX;
			if (IsWall(x, yIntercept, ra))
			{
				hitVert = true;
				rx = x + (tileStepX == -1 ? 1 : 0);
				ry = yIntercept;
				break;
			}
			yIntercept += yStep;
		}
		while (!hitVert && (
			(tileStepX == 1 && (xIntercept <= x + 1)) ||
			(tileStepX == -1 && (xIntercept >= x))
			))
		{
			somethingDone = true;
			y += tileStepY;
			if (IsWall(xIntercept, y, ra))
			{
				hitHoriz = true;
				rx = xIntercept;
				ry = y + (tileStepY == -1 ? 1 : 0);
				break;
			}
			xIntercept += xStep;
		}
	} while ((!hitHoriz && !hitVert) && somethingDone);

	if (!somethingDone)
	{
		return 0;
	}

	float dx = rx - px;
	float dy = ry - py;

	Finish(static_cast<uint16_t>(rx * 256), static_cast<uint16_t>(ry * 256), static_cast<int16_t>(dx * 256), static_cast<int16_t>(dy * 256));

	float d = sqrt(dx * dx + dy * dy);

	return d;
}

void RayCasterFloat::Viewport(float px, float py, float pa, float *wd)
{
	pa *= 2.0f * M_PI;
	px *= 4.0f;
	py *= 4.0f;
	for (int c = 0; c < SCREEN_WIDTH; c++)
	{
		float da = (c - SCREEN_WIDTH / 2) * M_PI * FOV / (SCREEN_WIDTH * 4);
		float d2 = Distance2(px, py, pa + da);
		float ad = d2 * cos(da);
		if (ad > 0)
		{
			wd[c] = INV_FACTOR / ad;
		}
		else
		{
			wd[c] = 0;
		}
	}
}

void RayCasterFloat::Trace(uint16_t px, uint16_t py, int16_t pa, uint8_t * wh)
{
	Viewport(px / 1024.0f, py / 1024.0f, pa / 1024.0f, _wd);
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		wh[x] = static_cast<unsigned char>(_wd[x]);
	}
}

RayCasterFloat::RayCasterFloat(uint8_t *map) : RayCaster()
{
	_wd = new float[SCREEN_WIDTH];
	_map = map;
}

RayCasterFloat::~RayCasterFloat()
{
}