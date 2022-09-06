#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "GameEngine.h"

class OneLoneCoder_UltimateFPS : public olcConsoleGameEngine
{
public:
	OneLoneCoder_UltimateFPS()
	{
		m_sAppName = L"Ultimate First Person Shooter";
	}

private:
	int nMapWidth = 32;				
	int nMapHeight = 32;

	float fPlayerX = 14.7f;			
	float fPlayerY = 8;
	float fPlayerA = -3.14159f / 2.0f;			
	float fFOV = 3.14159f / 4.0f;	
	float fDepth = 16.0f;			
	float fSpeed = 5.0f;			
	wstring map;

	olcSprite	*spriteWall;
	olcSprite	*spriteLamp;
	olcSprite	*spriteFireBall;

	float *fDepthBuffer = nullptr;

	struct sObject
	{
		float x;
		float y;
		float vx;
		float vy;
		bool bRemove;
		olcSprite *sprite;
	};

	list<sObject> listObjects;

protected:
	virtual bool OnUserCreate()
	{
		map += L"#########.......#########.......";
		map += L"#...............#...............";
		map += L"#.......#########.......########";
		map += L"#..............##..............#";
		map += L"#......##......##......##......#";
		map += L"#......##..............##......#";
		map += L"#..............##..............#";
		map += L"###............####............#";
		map += L"##.............###.............#";
		map += L"#............####............###";
		map += L"#..............................#";
		map += L"#..............##..............#";
		map += L"#..............##..............#";
		map += L"#...........#####...........####";
		map += L"#..............................#";
		map += L"###..####....########....#######";
		map += L"####.####.......######..........";
		map += L"#...............#...............";
		map += L"#.......#########.......##..####";
		map += L"#..............##..............#";
		map += L"#......##......##.......#......#";
		map += L"#......##......##......##......#";
		map += L"#..............##..............#";
		map += L"###............####............#";
		map += L"##.............###.............#";
		map += L"#............####............###";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............##..............#";
		map += L"#...........##..............####";
		map += L"#..............##..............#";
		map += L"################################";


		spriteWall = new olcSprite(L"FPSSprites/fps_wall1.spr");
		spriteLamp = new olcSprite(L"FPSSprites/fps_lamp1.spr");
		spriteFireBall = new olcSprite(L"FPSSprites/fps_fireball1.spr");
		fDepthBuffer = new float[ScreenWidth()];

		listObjects = { 
			{ 8.5f, 8.5f, 0.0f, 0.0f, false, spriteLamp },
			{ 7.5f, 7.5f, 0.0f, 0.0f, false, spriteLamp },
			{ 10.5f, 3.5f, 0.0f, 0.0f, false, spriteLamp },			
		};
		return true;
	}


	virtual bool OnUserUpdate(float fElapsedTime)
	{
		
		if (m_keys[L'A'].bHeld)
			fPlayerA -= (fSpeed * 0.5f) * fElapsedTime;

		
		if (m_keys[L'D'].bHeld)
			fPlayerA += (fSpeed * 0.5f) * fElapsedTime;

		
		if (m_keys[L'W'].bHeld)
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		
		if (m_keys[L'S'].bHeld)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		
		if (m_keys[L'E'].bHeld)
		{
			fPlayerX += cosf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY -= sinf(fPlayerA) * fSpeed * fElapsedTime;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= cosf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY += sinf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		
		if (m_keys[L'Q'].bHeld)
		{
			fPlayerX -= cosf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY += sinf(fPlayerA) * fSpeed * fElapsedTime;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += cosf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY -= sinf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		
		if (m_keys[VK_SPACE].bReleased)
		{
			sObject o;
			o.x = fPlayerX;
			o.y = fPlayerY;
			float fNoise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
			o.vx = sinf(fPlayerA + fNoise) * 8.0f;
			o.vy = cosf(fPlayerA + fNoise) * 8.0f;
			o.sprite = spriteFireBall;
			o.bRemove = false;
			listObjects.push_back(o);
		}

		for (int x = 0; x < ScreenWidth(); x++)
		{
			
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

			
			float fStepSize = 0.01f;	  
			float fDistanceToWall = 0.0f; 

			bool bHitWall = false;		
			bool bBoundary = false;		

			float fEyeX = sinf(fRayAngle); 
			float fEyeY = cosf(fRayAngle);

			float fSampleX = 0.0f;

			bool bLit = false;

			
			
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			
					fDistanceToWall = fDepth;
				}
				else
				{
					
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						
						bHitWall = true;

						
						
						float fBlockMidX = (float)nTestX + 0.5f;
						float fBlockMidY = (float)nTestY + 0.5f;

						float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
						float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;

						float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

						if (fTestAngle >= -3.14159f * 0.25f && fTestAngle < 3.14159f * 0.25f)
							fSampleX = fTestPointY - (float)nTestY;
						if (fTestAngle >= 3.14159f * 0.25f && fTestAngle < 3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle < -3.14159f * 0.25f && fTestAngle >= -3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle >= 3.14159f * 0.75f || fTestAngle < -3.14159f * 0.75f)
							fSampleX = fTestPointY - (float)nTestY;
					}
				}
			}

			
			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nFloor = ScreenHeight() - nCeiling;

			
			fDepthBuffer[x] = fDistanceToWall;

			for (int y = 0; y < ScreenHeight(); y++)
			{
				
				if (y <= nCeiling)
					Draw(x, y, L' ');
				else if (y > nCeiling && y <= nFloor)
				{
					
					if (fDistanceToWall < fDepth)
					{
						float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
						Draw(x, y, spriteWall->SampleGlyph(fSampleX, fSampleY), spriteWall->SampleColour(fSampleX, fSampleY));
					}
					else
						Draw(x, y, PIXEL_SOLID, 0);
				}
				else 
				{
					Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
				}
			}
		}

		
		for (auto &object : listObjects)
		{
			
			object.x += object.vx * fElapsedTime;
			object.y += object.vy * fElapsedTime;

			
			if (map.c_str()[(int)object.x * nMapWidth + (int)object.y] == '#')
				object.bRemove = true;
						
			
			float fVecX = object.x - fPlayerX;
			float fVecY = object.y - fPlayerY;
			float fDistanceFromPlayer = sqrtf(fVecX*fVecX + fVecY*fVecY);

			float fEyeX = sinf(fPlayerA);
			float fEyeY = cosf(fPlayerA);

			
			
			float fObjectAngle = atan2f(fEyeY, fEyeX) - atan2f(fVecY, fVecX);
			if (fObjectAngle < -3.14159f)
				fObjectAngle += 2.0f * 3.14159f;
			if (fObjectAngle > 3.14159f)
				fObjectAngle -= 2.0f * 3.14159f;

			bool bInPlayerFOV = fabs(fObjectAngle) < fFOV / 2.0f;

			if (bInPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < fDepth && !object.bRemove)
			{
				float fObjectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceFromPlayer);
				float fObjectFloor = ScreenHeight() - fObjectCeiling;
				float fObjectHeight = fObjectFloor - fObjectCeiling;
				float fObjectAspectRatio = (float)object.sprite->nHeight / (float)object.sprite->nWidth;
				float fObjectWidth = fObjectHeight / fObjectAspectRatio;
				float fMiddleOfObject = (0.5f * (fObjectAngle / (fFOV / 2.0f)) + 0.5f) * (float)ScreenWidth();

				
				for (float lx = 0; lx < fObjectWidth; lx++)
				{
					for (float ly = 0; ly < fObjectHeight; ly++)
					{
						float fSampleX = lx / fObjectWidth;
						float fSampleY = ly / fObjectHeight;
						wchar_t c = object.sprite->SampleGlyph(fSampleX, fSampleY);
						int nObjectColumn = (int)(fMiddleOfObject + lx - (fObjectWidth / 2.0f));
						if (nObjectColumn >= 0 && nObjectColumn < ScreenWidth())
							if (c != L' ' && fDepthBuffer[nObjectColumn] >= fDistanceFromPlayer)
							{							
								Draw(nObjectColumn, fObjectCeiling + ly, c, object.sprite->SampleColour(fSampleX, fSampleY));
								fDepthBuffer[nObjectColumn] = fDistanceFromPlayer;
							}
					}
				}
			}
		}

		
		listObjects.remove_if([](sObject &o) {return o.bRemove; });

		
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
				Draw(nx+1, ny+1, map[ny * nMapWidth + nx]);
		Draw(1 + (int)fPlayerY, 1 + (int)fPlayerX, L'P');
		
		return true;
	}

};

int main()
{
	OneLoneCoder_UltimateFPS game;
	game.ConstructConsole(320, 240,4,4);
	game.Start();
	return 0;
}