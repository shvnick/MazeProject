#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Window.h"
#include <vector>

class Texture: public Window
{
protected:
	int fov;
	float d0;
	std::shared_ptr<SDL_Texture> wall;

	SDL_Rect MapRect, imgPartRect;
	unsigned MapWidth, MapHeight, ColumnInTexture;

public:
	Texture(
			int width = DEFAULT_WIDTH,
			int height = DEFAULT_HEIGHT
			);
	virtual ~Texture() = default;

	virtual void render() override;
	virtual void handle_keys(const Uint8 *keys) override;
};

#endif /* TEXTURE_H_ */




