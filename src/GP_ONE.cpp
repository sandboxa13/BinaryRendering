#include "GP_ONE.h"

Sprite   GP_ONE::spriteMemory[MAX_SPRITE_COUNT];
uint16_t GP_ONE::frameBuffer[FRAMEBUFFER_BUF_SIZE];


void GP_ONE::loadSprites(const Sprite* sprites, uint16_t spriteCount) 
{
	for (uint16_t i = 0; i < spriteCount; i++)
	{
		spriteMemory[i] = sprites[i];
	}
}


void GP_ONE::clearFrameBuffer(BackGroundColor bkgColor)
{
	uint16_t color = bkgColor == BackGroundColor::BLACK ? 0u : -1;

	for (uint16_t i = 0; i < FRAMEBUFFER_BUF_SIZE; i++)
	{
		frameBuffer[i] = color;
	}
}


void GP_ONE::drawSpriteInstances(const SpriteInstance* instances, uint16_t instanceCount)
{
	Sprite sprite = {};

	uint16_t spriteStartX = 0; // Sprite start location by X
	uint16_t spriteStartY = 0; // Sprite start location by Y
	
	// Variables for helping finding color coordinate inside a flat color array
	uint16_t colorX = 0;  
	uint16_t colorY = 0;

	uint16_t frameBufferCoord = 0; // FrameBuffer element index
	uint16_t colorCoord = 0; // Color array element index

	uint16_t spriteColor = 0; // Color from sprite color array
	uint16_t spriteAlphaColor = 0; // Alpha color from sprite color array
	uint16_t frameBufferValue = 0; // Value from FrameBuffer

	uint16_t spriteTileOffsetX = 0;

	// Runs trough all sprites
	for (uint16_t i = 0; i < instanceCount; i++)
	{
		sprite = spriteMemory[instances[i].ind];
		
		// Getting X coordinate and right shifting they by 4 (divide by 16)
		// Because width of FrameBuffer is 32, we need to scale this coordinate from 512 to 32 (32 * 16 == 512)
		spriteStartX = instances[i].x >> 4;
		// Y left the same, because Height of FrameBuffer is 512
		spriteStartY = instances[i].y;

		colorX = 0;
		colorY = 0;

		spriteTileOffsetX = instances[i].x - (spriteStartX << 4);

		for (uint16_t spriteY = spriteStartY; spriteY < spriteStartY + SPRITE_TILES_Y; spriteY++)
		{
			for (uint16_t spriteX = spriteStartX; spriteX < spriteStartX + SPRITE_TILES_X; spriteX++)
			{
				// Calculating index in FrameBufer by left shifting Y by 5 (2^5 = 32, 32 is FRAMEBUFFER_TILES_X, equals to 32 * Y)
				frameBufferCoord = (spriteY << 5) + spriteX;

				// Calculatng index in color array by left shifting Y by 2 (2^2 = 4, 4 is SPRITE_TILES_X, equals to 4 * Y)
				colorCoord = (colorY << 2) + colorX;

				spriteColor = sprite.color[colorCoord] >> spriteTileOffsetX;
				spriteAlphaColor = sprite.alpha[colorCoord] >> spriteTileOffsetX;
				
				frameBufferValue = frameBuffer[frameBufferCoord];

				if (spriteTileOffsetX && colorCoord)
				{
					spriteColor |= sprite.color[colorCoord - 1u] << (16 - spriteTileOffsetX);
					spriteAlphaColor |= sprite.alpha[colorCoord - 1u] << (16 - spriteTileOffsetX);
				}

				// Calculating finally color by formula from lecture
				frameBuffer[frameBufferCoord] = (spriteColor & spriteAlphaColor) | (frameBufferValue & ~spriteAlphaColor);

				colorX++;
			}
			colorY++;
			colorX = 0;
		}
	}
}


void GP_ONE::saveFrameBuffer(FrameBuffer& outFrameBuffer)
{
	for (uint16_t i = 0; i < FRAMEBUFFER_BUF_SIZE; i++)
	{
		outFrameBuffer.color[i] = frameBuffer[i];
	}
}