#pragma once

struct GLTexture;
struct GLTextureArray;

struct GPUTextureData
{
	GLTexture* generatedTexture = nullptr;

	//	When copied in a texture array
	GLTextureArray* registeredTextureArray = nullptr;
	int arrayIndex = -1;
	int posX = 0;
	int posY = 0;
	int width = 0;
	int height = 0;
};