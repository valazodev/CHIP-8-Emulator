#ifndef DISPLAY_CPP
#define DISPLAY_CPP

#include <array>
#include <string>
#include <SDL2/SDL.h>


class Display
{

public:

    struct Scale {
        float x, y;
        Scale (float value);
        Scale (float x_scale, float y_scale);
    };

    using Coord  = unsigned int;
    using Pixels = unsigned int;
    using String = std::string;
    using Sprite = std::array <bool,8>;

     Display (String title, Pixels width, Pixels height, Scale scale);
    ~Display ();

    void    clear    ();
    void    draw     (const Sprite& sprite, Coord x, Coord y);
    uint8_t get_key  ();
    uint8_t last_key ();

private:

    struct RGBA {
        uint8_t r, g, b, a;
        RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    };

    String title;
    Pixels width;
    Pixels height;
    Scale scale;

    SDL_Window* window;
    SDL_Renderer* renderer;
    uint8_t last_key_value;

    void    init_SDL ();
    void    render   (SDL_Rect area, RGBA color);
};


#endif // DISPLAY_CPP
