#include <CHIP-8/display.h>

#include <stdexcept>

Display::Display (String title, Pixels width, Pixels height, Scale scale) :
    title(title), width(width), height(height), scale(scale)
{
    init_SDL();
    clear();
}

Display::~Display ()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
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

Display::RGBA::RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) :
    r(red), g(green), b(blue), a(alpha) {}

void Display::clear ()
{
    SDL_RenderSetViewport(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void Display::draw (const Sprite& sprite, Coord x, Coord y)
{
    auto width = unsigned(8);

    SDL_Rect pixel;
    pixel.y = int(y * scale.y);
    pixel.h = int(height * scale.x);
    pixel.w = int(width * scale.y);

    const RGBA black(0x00,0x00,0x00,0x00);
    const RGBA white(0xFF,0xFF,0xFF,0xFF);

    for (unsigned col=0; col<width; ++col) {
        pixel.x = int((x + col) * scale.x);

        if (sprite[col])
            render(pixel, white);
        else
            render(pixel, black);
    }
    SDL_RenderPresent(renderer);
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        auto error = std::string("No pudo crearse el renderer. SDL_Error: ");
        error += SDL_GetError();
        throw std::runtime_error(error);
    }
}

void Display::render(SDL_Rect area, RGBA color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &area);
}

uint8_t Display::get_key()
{
    SDL_Event e;
    while (SDL_PollEvent( &e ) != 0) {
        if (e.key.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_1: last_key_value = 0x1; break;
                case SDLK_2: last_key_value = 0x2; break;
                case SDLK_3: last_key_value = 0x3; break;
                case SDLK_4: last_key_value = 0xC; break;
                case SDLK_q: last_key_value = 0x4; break;
                case SDLK_w: last_key_value = 0x5; break;
                case SDLK_e: last_key_value = 0x6; break;
                case SDLK_r: last_key_value = 0xD; break;
                case SDLK_a: last_key_value = 0x7; break;
                case SDLK_s: last_key_value = 0x8; break;
                case SDLK_d: last_key_value = 0x9; break;
                case SDLK_f: last_key_value = 0xE; break;
                case SDLK_z: last_key_value = 0xA; break;
                case SDLK_x: last_key_value = 0x0; break;
                case SDLK_c: last_key_value = 0xB; break;
                case SDLK_v: last_key_value = 0xF; break;
            }
            return last_key_value;
        }
    }
    // Impossible return
    return 0xFF;
}

uint8_t Display::last_key()
{
    return last_key_value;
}













