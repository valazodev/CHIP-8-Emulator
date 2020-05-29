#include <CHIP-8/display.h>

#include <stdexcept>

Display::Display (String title, Pixels width, Pixels height, PixelSize px_size)
{
    this->title = title;
    this->width = width;
    this->height = height;
    this->px_size = px_size;

    init_pixels();
    init_SDL();
    clear();
}

Display::~Display ()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Display::PixelSize::PixelSize (RealPixels width, RealPixels height)
{
    this->width = width;
    this->height = height;
}

Display::PixelSize::PixelSize (float scale, float aspect_ratio)
{
    width = RealPixels(scale * aspect_ratio);
    height = RealPixels(scale);
}

void Display::clear ()
{
    SDL_FillRect (
        screenSurface,
        nullptr,
        SDL_MapRGB (
            screenSurface->format,
            255, 255, 255
        )
    );
    SDL_UpdateWindowSurface(window);
}

void Display::draw (Coord x, Coord y, unsigned height)
{

}

void Display::init_pixels ()
{
    pixels.resize(width * height);
    for (size_t y=0; y<height; ++y) {
        for (size_t x=0; x<width; ++x)
        {
            size_t index = (width * y) + x;
            auto& pixel = pixels[index];

            pixel.w = int(px_size.width);
            pixel.h = int(px_size.height);
            pixel.x = int(x * px_size.width);
            pixel.y = int(y * px_size.height);
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
        int(width * px_size.width),
        int(height * px_size.height),
        SDL_WINDOW_SHOWN);

    if (!window) {
        auto error = std::string("No pudo crearse la ventana. SDL_Error: ");
        error += SDL_GetError();
        throw std::runtime_error(error);
    }
    screenSurface = SDL_GetWindowSurface(window);
}
