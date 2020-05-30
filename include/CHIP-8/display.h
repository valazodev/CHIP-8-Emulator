#ifndef DISPLAY_CPP
#define DISPLAY_CPP

#include <string>
#include <vector>
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
    using Sprite = std::vector <bool>;

     Display (String title, Pixels width, Pixels height, Scale scale);
    ~Display ();

    void clear  ();
    void draw   (const Sprite& sprite, Coord x, Coord y, unsigned height);

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

    void init_SDL ();
    void render   (SDL_Rect area, RGBA color);
};


#endif // DISPLAY_CPP
