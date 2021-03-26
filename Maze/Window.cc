#include <stdexcept>
#include "Window.h"

Window::Window(int width, int height)
: _width(width), _height(height)
{
	_window = std::shared_ptr<SDL_Window>(
				SDL_CreateWindow("MyMaze",
						SDL_WINDOWPOS_CENTERED,
						SDL_WINDOWPOS_CENTERED,
						width, height,
						SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
				SDL_DestroyWindow);
		if (_window == nullptr)
			throw std::runtime_error(
					std::string("�� ������� ������� ����: ") +
					std::string(SDL_GetError()));

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	_renderer = std::shared_ptr<SDL_Renderer>(
			SDL_CreateRenderer(
					_window.get(), -1,
					SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
			SDL_DestroyRenderer);
	if (_renderer == nullptr)
		throw std::runtime_error(
				std::string("��� �������� ��������� "
						"��������� ������: ") +
				std::string(SDL_GetError()));
}

void Window::main_loop()
{
	setup();

	auto keys = SDL_GetKeyboardState(nullptr);
	SDL_Event event;
	for(;;) {
		//��������� �������
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				return;
			handle_event(event);
		}
		//��������� ������
		if (keys[SDL_SCANCODE_ESCAPE]) return;
		handle_keys(keys);

		// ���������
		render();

		//����������� ����������
		SDL_RenderPresent(_renderer.get());
	}
}


