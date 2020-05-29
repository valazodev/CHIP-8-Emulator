#include <CHIP-8/display.h>

#include <stdexcept>

Display::Display (String title, Pixels width, Pixels height, Scale scale)
    : title(title), width(width), height(height), scale(scale)
{
    init_pixels();
    init_SDL();
    clear();
}

Display::~Display ()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Display::Scale::Scale (float value)
{
    x = value;
    y = value;
}

Display::Scale::Scale (float x_scale, float y_scale)
{
    x = x_scale;
    y = y_scale;
}

void Display::clear ()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}

void Display::draw (const Sprite& sprite, Coord x, Coord y, unsigned height)
{
    auto width = unsigned(sprite.size());

    for (auto j=x; j < x+width; ++j)
    {
        if (sprite[j-x] == true)
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        else
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        for (auto i=y; i < y+height; ++i) {
            size_t index = (width*i) + j;
            auto& pixel = pixels[index];

            SDL_RenderSetViewport(renderer, &pixel);
            SDL_RenderPresent(renderer);
        }
    }
}

void Display::init_pixels ()
{
    pixels.resize(width * height);
    for (size_t y=0; y<height; ++y) {
        for (size_t x=0; x<width; ++x)
        {
            size_t index = (width * y) + x;
            auto& pixel = pixels[index];

            pixel.w = int(scale.x);
            pixel.h = int(scale.y);
            pixel.x = int(x * scale.x);
            pixel.y = int(y * scale.y);
        }
    }
}

void Display::init_SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("No pudo inicializarse la libreria SDL.\n");

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        int(width * scale.x),
        int(height * scale.y),
        SDL_WINDOW_SHOWN);

    if (!window) {
        auto error = std::string("No pudo crearse la ventana. SDL_Error: ");
        error += SDL_GetError();
        throw std::runtime_error(error);
    }
    screenSurface = SDL_GetWindowSurface(window);
}
