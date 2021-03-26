#include "Texture.h"

#include <stdexcept>
#include <SDL2/SDL_image.h>
#include <vector>

#include <cmath>
constexpr float eps = 0.001;
constexpr int numberOfCellsInWidth = 12, numberOfCellsInHeight = 15;

const char map[] =
"############"
"#   #  #   #"
"#    #     #"
"#    #     #"
"#          #"
"#   ###    #"
"#   #      #"
"#   ##     #"
"#   ###    #"
"#          #"
"#   ###    #"
"#   #      #"
"#    #     #"
"#   ###    #"
"############";

float x_r, y_r,
	  x_d, y_d, Dh, Dv, D,
	  cosB, sinB,
	  alf = 45, alfRad, gamRad, betRad,
	  d, x = 2, y = 5,
	  X_Step, Y_Step,
	  WalkingSpeed = 0.03;

  int h,
  	  Column, TextureOfColumn,
	  TextureWidth, TextureHeight;

void get_wall(float &x_r, float &y_r, float x_d, float y_d){
	do {
		x_r += x_d;
		y_r += y_d;
	} while
		(map[(int)x_r+(int)y_r*numberOfCellsInWidth] !='#' &&
		((0<=y_r) && (y<=numberOfCellsInHeight) &&
		(0<=x_r) && (x_r<=numberOfCellsInWidth)));
}


Texture::Texture(int width, int height) :
		Window(width, height),
		fov(120), d0(width/(2*tan(fov/2 * 3.14 / 180))),

		MapRect(),
		MapWidth((width/9)/numberOfCellsInWidth),
		MapHeight((height/9)/numberOfCellsInHeight),
		ColumnInTexture(width / numberOfCellsInWidth*4){
			wall = std::shared_ptr<SDL_Texture>(
			IMG_LoadTexture(_renderer.get(), "wall.png"), SDL_DestroyTexture);
			if (wall == nullptr)
				throw std::runtime_error(
						std::string("Не удалось загрузить текстуру: ")
						+ std::string(SDL_GetError()));
		SDL_QueryTexture(wall.get(), nullptr, nullptr, &TextureWidth, &TextureHeight);
}

void Texture::render() {
	SDL_SetRenderDrawColor(_renderer.get(), 10, 15, 16, 50);
	SDL_RenderClear(_renderer.get());
	SDL_SetRenderDrawBlendMode(_renderer.get(), SDL_BLENDMODE_BLEND);

	MapRect.w = width();
	MapRect.h = height()/2;
	MapRect.x = 0;
	MapRect.y = 0;

	SDL_SetRenderDrawColor(_renderer.get(), 255, 180, 250, 40);
	SDL_RenderFillRect(_renderer.get(), &MapRect);

	for (Column = 0;  Column < width(); ++Column)
	{
		gamRad = atan2(Column-width()/2, d0);

		alfRad = alf * 3.14 / 180;
		betRad = gamRad + alfRad;

		cosB = cos(betRad);
		sinB = sin(betRad);

		if (cosB > eps)
		{
			x_r = floor(x) + eps;
			x_d = 1;
			y_d = tan(betRad);
			y_r = y- (x-x_r) * y_d;
		} else if (cosB < -eps){
			x_r = ceil(x) - eps;
			x_d = -1;
			y_d = tan(-betRad);
			y_r = y - (x_r - x) * y_d;
		}

		get_wall(x_r, y_r, x_d, y_d);

		if  (((-eps <= cosB) && (cosB <= eps)) ||
			(x_r < 0 || x_r > numberOfCellsInWidth || y_r < 0 || y_r >numberOfCellsInHeight))
		{
			Dv = width()*3;
		} else {
			Dv = hypot (x_r-x, y_r-y);
		}

		if (sinB > eps)
		{
			y_d = 1;
			y_r = floor(y) + eps;
			x_d = 1/tan(betRad);
			x_r = x - (y - y_r) * x_d;
		} else if (sinB < -eps){
			y_d = -1;
			y_r = ceil(y) - eps;
			x_d = 1/tan(-betRad);
			x_r = x - (y_r - y) * x_d;
		}
		get_wall(x_r, y_r, x_d, y_d);
		if  (((-eps <= sinB) && (sinB <= eps)) ||
			(x_r < 0 || x_r > numberOfCellsInWidth || y_r < 0 || y_r > numberOfCellsInHeight))
		{
			Dh = height()*3;
		} else {
			Dh = hypot (x_r-x, y_r-y);
		}

		D = std::min(Dh, Dv);
		d = d0/cos(gamRad);
		h = d/D;

		TextureOfColumn = Column % ColumnInTexture;

		imgPartRect.x = TextureOfColumn * (TextureWidth / ColumnInTexture);
		imgPartRect.y = 0;
		imgPartRect.w = 2;
		imgPartRect.h = TextureHeight;

		SDL_Rect wall_rect{Column, height()/2 - h/2, 1, h};
		SDL_RenderCopy(_renderer.get(), wall.get(), &imgPartRect, &wall_rect);
	}

	for (unsigned a = 0; a < numberOfCellsInHeight; a++) {
		for (unsigned b = 0; b < numberOfCellsInWidth; b++) {

			MapRect.w = std::max(MapWidth, MapHeight);
			MapRect.h = std::max(MapWidth, MapHeight);
			MapRect.x = 300 + (MapRect.w * b);
			MapRect.y = 500 + (MapRect.h * a);
			SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 255, 50);

			if (b == (unsigned)x &&
				a == (unsigned)y &&
				map[b+a*numberOfCellsInWidth]==' ')
			{
				SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 150);
				MapRect.w -= MapRect.w / 2;
				MapRect.h -= MapRect.h / 2;
				MapRect.x += MapRect.w / 2;
				MapRect.y += MapRect.h / 2;

			} else if (map[b+a*numberOfCellsInWidth]==' ') continue;

			SDL_RenderFillRect(_renderer.get(), &MapRect);
		}
	}
}

void Texture::handle_keys(const Uint8 *keys) {
	if (keys[SDL_SCANCODE_W]){
		X_Step = cos(alf * 3.14/180)*WalkingSpeed;
		Y_Step = sin(alf * 3.14/180)*WalkingSpeed;

		if(map[(int)(floor(x + X_Step))+
			   (int)(floor(y + Y_Step)) * numberOfCellsInWidth] == ' ')
		{
			x += X_Step;
			y += Y_Step;
		}
	}
	if (keys[SDL_SCANCODE_S]){
		X_Step = cos(alf * 3.14/180)*WalkingSpeed;
		Y_Step = sin(alf * 3.14/180)*WalkingSpeed;

		if(map[(int)(floor(x - X_Step))
			  +(int)(floor(y - Y_Step)) * numberOfCellsInWidth] == ' ')
		{
			x -= X_Step;
			y -= Y_Step;
		}
	}
	if (keys[SDL_SCANCODE_A]){
		alf -=1;
	}
	if (keys[SDL_SCANCODE_D]){
		alf +=1;
	}

}



