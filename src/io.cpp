#include <CHIP-8/io.h>
#include <stdexcept>

using namespace std;

IO::IO (String title, Pixels width, Pixels height, Scale scale) :
    title(title), width(width), height(height), scale(scale)
{
    init_SDL();

    pixels.resize(width * height);
    for (unsigned i=0; i<pixels.capacity(); ++i)
        pixels[i] = 0;

    key_pressed = false;
    clear();
}

IO::~IO ()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

IO::Scale::Scale (float value)
{
    x = value;
    y = value;
}

IO::Scale::Scale (float x_scale, float y_scale)
{
    x = x_scale;
    y = y_scale;
}

IO::RGBA::RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) :
    r(red), g(green), b(blue), a(alpha) {}

void IO::clear ()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);

    for (unsigned i=0; i<pixels.capacity(); ++i)
        pixels[i] = 0;
}

void IO::draw (const Sprite& sprite, Coord x, Coord y)
{
    for (unsigned col=0; col<sprite.size(); ++col) {
        unsigned index = (width * y) + (x + col);
        pixels[index] = sprite[col];
    }
    SDL_UpdateTexture(
        texture,
        nullptr,
        &pixels[0],
        int(width * 4));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void IO::assert(bool expr, string error_msg)
{
    if (!expr) {
        error_msg += string("\nSDL_Error: ") + SDL_GetError();
        throw runtime_error(error_msg);
    }
}

void IO::init_SDL()
{
    assert( SDL_Init(SDL_INIT_VIDEO) >= 0,
            "No pudo inicializarse la libreria SDL.");

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        int(width * scale.x),
        int(height * scale.y),
        SDL_WINDOW_SHOWN);
    assert(window, "No pudo crearse la ventana.");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    assert(renderer, "No pudo crearse el renderizador.");
    SDL_RenderSetScale( renderer, scale.x, scale.y );

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC,
        int(width),
        int(height));
}

uint8_t IO::wait_key()
{
    while (!key_pressed)
        update();

    return key_value;
}

uint8_t IO::last_key()
{
    if (key_pressed)
        return key_value;
    else
        return 0xFF;
}

void IO::update()
{
    SDL_PollEvent( &event );

    if (event.type == SDL_QUIT)
        exit(EXIT_SUCCESS);

    if (event.key.type == SDL_KEYDOWN && !key_pressed) {
        switch (event.key.keysym.sym) {
            case SDLK_1: key_value = 0x1; break;
            case SDLK_2: key_value = 0x2; break;
            case SDLK_3: key_value = 0x3; break;
            case SDLK_4: key_value = 0xC; break;
            case SDLK_q: key_value = 0x4; break;
            case SDLK_w: key_value = 0x5; break;
            case SDLK_e: key_value = 0x6; break;
            case SDLK_r: key_value = 0xD; break;
            case SDLK_a: key_value = 0x7; break;
            case SDLK_s: key_value = 0x8; break;
            case SDLK_d: key_value = 0x9; break;
            case SDLK_f: key_value = 0xE; break;
            case SDLK_z: key_value = 0xA; break;
            case SDLK_x: key_value = 0x0; break;
            case SDLK_c: key_value = 0xB; break;
            case SDLK_v: key_value = 0xF; break;
        }
        key_pressed = true;
    }
    if (event.key.type == SDL_KEYUP && key_pressed)
        key_pressed = false;
}
