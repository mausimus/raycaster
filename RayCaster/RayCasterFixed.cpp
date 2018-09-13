// fixed-point implementation

#include "RayCasterFixed.h"

#define INVERT(x) (uint8_t)((x ^ 255) + 1)

#define ABS(x) (x < 0 ? -x : x)

// (v * f) >> 8
uint16_t RayCasterFixed::Mul(uint8_t v, uint16_t f)
{
	uint8_t f_h = f >> 8;
	uint8_t f_l = f % 256;
	uint16_t hm = v * f_h;
	uint16_t lm = v * f_l;
	return hm + (lm >> 8);
}

uint8_t RayCasterFixed::Height(uint16_t d)
{
	if (d >= 256)
	{
		uint16_t ds = d >> 3;
		if (ds >= 256)
		{
			return _fard[255] - 1;
		}
		return _fard[ds];
	}
	else
	{
		return _neard[d];
	}
}

int16_t RayCasterFixed::MulS(uint8_t v, int16_t f)
{
	uint16_t uf = Mul(v, static_cast<uint16_t>(ABS(f)));
	if (f < 0)
	{
		return ~uf;
	}
	return uf;
}

int16_t RayCasterFixed::MulTan(uint8_t v, uint8_t i, uint8_t qtr, uint8_t a, uint16_t* fun)
{
	uint8_t iv = v;
	if (i == 1)
	{
		if (v == 0)
		{
			if (qtr % 2 == 1)
			{
				return -AbsTan(qtr, a, fun);
			}
			return AbsTan(qtr, a, fun);
		}
		iv = INVERT(v);
	}
	if (iv == 0)
	{
		return 0;
	}
	if (qtr % 2 == 1)
	{
		return -Mul(iv, fun[INVERT(a)]);
	}
	return Mul(iv, fun[a]);
}

int16_t RayCasterFixed::AbsTan(uint8_t qtr, uint8_t a, uint16_t* fun)
{
	if (qtr % 2 == 1)
	{
		return fun[INVERT(a)] >> 0;
	}
	return fun[a] >> 0;
}

inline bool RayCasterFixed::IsWall(uint8_t bx, uint8_t by, uint8_t sx, uint8_t sy)
{
#ifdef _DEBUG
	Step((bx << 8) + sx, (by << 8) + sy);
#endif

	return _map[bx + (by << MAP_XS)] == 1;
}

void RayCasterFixed::Distance(uint16_t rx, uint16_t ry, uint16_t _ra, int16_t *dx, int16_t *dy)
{
#ifdef _DEBUG
	Start(rx, ry, _ra);
#endif

	register int8_t tileStepX;
	register int8_t tileStepY;
	register int16_t xIntercept = rx;
	register int16_t yIntercept = ry;

	uint8_t x = rx >> 8;
	uint8_t y = ry >> 8;
	uint8_t qtr = _ra >> 8;
	uint8_t qra = _ra % 256;
	uint8_t _sx = rx % 256;
	uint8_t _sy = ry % 256;
	int16_t hx;
	int16_t hy;

	if (qra == 0)
	{
		switch (qtr % 2)
		{
		case 0:
			tileStepX = 0;
			tileStepY = qtr == 0 ? 1 : -1;
			if (tileStepY == 1)
			{
				yIntercept -= 256;
			}
			for (;;)
			{
				y += tileStepY;
				if (IsWall(x, y, x, 0))
				{
					goto hithoriz;
				}
			}
			break;
		case 1:
			tileStepY = 0;
			tileStepX = qtr == 1 ? 1 : -1;
			if (tileStepX == 1)
			{
				xIntercept -= 256;
			}
			for (;;)
			{
				x += tileStepX;
				if (IsWall(x, y, 0, y))
				{
					goto hitvert;
				}
			}
			break;
		}
	}
	else
	{
		int16_t xStep;
		int16_t yStep;

		switch (qtr)
		{
		case 0:
		case 1:
			tileStepX = 1;
			yIntercept += MulTan(_sx, 1, qtr, qra, _cotan);
			xIntercept -= 256;
			xStep = AbsTan(qtr, qra, _tan);
			break;
		case 2:
		case 3:
			tileStepX = -1;
			yIntercept -= MulTan(_sx, 0, qtr, qra, _cotan);
			xStep = -AbsTan(qtr, qra, _tan);
			break;
		}

		switch (qtr)
		{
		case 0:
		case 3:
			tileStepY = 1;
			xIntercept += MulTan(_sy, 1, qtr, qra, _tan);
			yIntercept -= 256;
			yStep = AbsTan(qtr, qra, _cotan);
			break;
		case 1:
		case 2:
			tileStepY = -1;
			xIntercept -= MulTan(_sy, 0, qtr, qra, _tan);
			yStep = -AbsTan(qtr, qra, _cotan);
			break;
		}

		for (;;)
		{
			while ((tileStepY == 1 && (yIntercept >> 8 < y)) || (tileStepY == -1 && (yIntercept >> 8 >= y)))
			{
				x += tileStepX;
				if (IsWall(x, y, 0, yIntercept % 256))
				{
					goto hitvert;
				}
				yIntercept += yStep;
			}
			while ((tileStepX == 1 && (xIntercept >> 8 < x)) || (tileStepX == -1 && (xIntercept >> 8 >= x)))
			{
				y += tileStepY;
				if (IsWall(x, y, xIntercept % 256, 0))
				{
					goto hithoriz;
				}
				xIntercept += xStep;
			}
		}
	}

hithoriz:
	hx = xIntercept + (tileStepX == 1 ? 256 : 0);
	hy = (y << 8) + (tileStepY == -1 ? 256 : 0);
	goto wallhit;

hitvert:
	hx = (x << 8) + (tileStepX == -1 ? 256 : 0);
	hy = yIntercept + (tileStepY == 1 ? 256 : 0);
	goto wallhit;

wallhit:
	*dx = hx - rx;
	*dy = hy - ry;

#ifdef _DEBUG
	Finish(rx, ry, *dx, *dy);
#endif
}

// (px, py) is 8 box coordinate bits, 8 inside coordinate bits
// (pa) is full circle as 1024
void RayCasterFixed::Viewport(uint16_t _sx, uint16_t _sy, int16_t pa, uint8_t *wd)
{
	uint8_t qtr = pa >> 8;
	uint8_t qa = pa % 256;

	for (int c = 0; c < SCREEN_WIDTH; c++)
	{
		uint16_t _ra = static_cast<uint16_t>(pa + _da[c]);

		// neutralize artefacts around edges	
		switch (_ra % 256)
		{
		case 1:
		case 254:
			_ra--;
			break;
		case 2:
		case 255:
			_ra++;
			break;
		}
		_ra %= 1024;
		
		int16_t dx;
		int16_t dy;
		Distance(_sx, _sy, _ra, &dx, &dy);

		// d = dy * cos(pa) + dx * sin(pa)
		int16_t d = 0;
		if (pa == 0)
		{
			d += dy;
		}
		else if (pa == 512)
		{
			d -= dy;
		}
		else switch (qtr)
		{
		case 0:
			d += MulS(_cos[qa], dy);
			break;
		case 1:
			d -= MulS(_cos[INVERT(qa)], dy);
			break;
		case 2:
			d -= MulS(_cos[qa], dy);
			break;
		case 3:
			d += MulS(_cos[INVERT(qa)], dy);
			break;
		}

		if (pa == 256)
		{
			d += dx;
		}
		else if (pa == 768)
		{
			d -= dx;
		}
		else switch (qtr)
		{
		case 0:
			d += MulS(_sin[qa], dx);
			break;
		case 1:
			d += MulS(_sin[INVERT(qa)], dx);
			break;
		case 2:
			d -= MulS(_sin[qa], dx);
			break;
		case 3:
			d -= MulS(_sin[INVERT(qa)], dx);
			break;
		}
		if (d >= MIN_DIST)
		{
			wd[c] = Height((d - MIN_DIST) >> 2);
		}
		else
		{
			wd[c] = SCREEN_HEIGHT >> 1;
		}
	}
}

void RayCasterFixed::Trace(uint16_t px, uint16_t py, int16_t pa, uint8_t * wh)
{
	Viewport(px, py, pa, wh);
}

RayCasterFixed::RayCasterFixed(uint8_t *map) : RayCaster()
{
	_map = map;
	//_log = true;
}

void RayCasterFixed::Precalculate()
{
	/*
	// replace precalculated tables with these results if you change any constants

	for (int i = 0; i < 256; i++)
	{
	_tan[i] = static_cast<uint16_t>((256.0f * tan(i * M_PI_2 / 256.0f)));
	_cotan[i] = static_cast<uint16_t>((256.0f / tan(i * M_PI_2 / 256.0f)));
	}
	for (int i = 0; i < 256; i++)
	{
	_sin[i] = static_cast<uint8_t>(256.0f * sin(i / 1024.0f * 2 * M_PI));
	_cos[i] = static_cast<uint8_t>(256.0f * cos(i / 1024.0f * 2 * M_PI));
	}
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
	int16_t da = static_cast<int16_t>((i - SCREEN_WIDTH / 2) * M_PI * FOV / (SCREEN_WIDTH * 4) / M_PI_2 * 256.0f);
	if (da < 0)
	{
	da += 1024;
	}
	_da[i] = static_cast<uint16_t>(da);
	}
	for (int i = 0; i < 256; i++)
	{
	_neard[i] = static_cast<uint8_t>((INV_FACTOR_INT / (((i << 2) + MIN_DIST) >> 2)) >> 2);
	_fard[i] = static_cast<uint8_t>((INV_FACTOR_INT / (((i << 5) + MIN_DIST) >> 5)) >> 5);
	}*/
}

RayCasterFixed::~RayCasterFixed()
{
}