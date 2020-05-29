#ifndef DISPLAY_CPP
#define DISPLAY_CPP

#include <string>
#include <vector>
#include <SDL2/SDL.h>


class Display
{

public:

    struct Scale {
        float x;
        float y;
        Scale (float value);
        Scale (float x_scale, float y_scale);
    };

    using Coord  = unsigned int;
    using Pixels = unsigned int;
    using String = std::string;
    using Sprite = std::vector <bool>;

     Display (String title, Pixels width, Pixels height, Scale scale);
    ~Display ();

    void clear ();
    void draw  (const Sprite& sprite, Coord x, Coord y, unsigned height);

private:

    using RectVector = std::vector <SDL_Rect>;

    String title;
    Pixels width;
    Pixels height;
    Scale scale;

    RectVector pixels;
    SDL_Window* window;
    SDL_Renderer* renderer;

    void init_pixels ();
    void init_SDL    ();
};


#endif // DISPLAY_CPP
