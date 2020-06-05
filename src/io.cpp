#include <CHIP-8/io.h>

#include <stdexcept>

IO::IO (String title, Pixels width, Pixels height, Scale scale) :
    title(title), width(width), height(height), scale(scale)
{
    init_SDL();
    clear();
    key_pressed = false;
}

IO::~IO ()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
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
    SDL_RenderSetViewport(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void IO::draw (const Sprite& sprite, Coord x, Coord y)
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

void IO::init_SDL()
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

void IO::render(SDL_Rect area, RGBA color)
{
    auto& r = color.r;
    auto& g = color.g;
    auto& b = color.b;
    auto& a = color.a;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &area);
}

uint8_t IO::wait_key()
{
    while (!key_pressed)
        update();

    return key_value;
}

uint8_t IO::last_key()
{
    return key_value;
}

void IO::update()
{
    SDL_PollEvent( &event );

    if (event.type == SDL_QUIT)
        exit(EXIT_FAILURE);

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
