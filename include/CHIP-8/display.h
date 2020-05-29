#ifndef DISPLAY_CPP
#define DISPLAY_CPP

#include <string>
#include <vector>
#include <SDL2/SDL.h>


class Display
{

public:

    /* Types */
    struct PixelSize
    {
        using RealPixels = unsigned int;
        RealPixels width;
        RealPixels height;
        PixelSize () : width(1), height(1) {}
        PixelSize (RealPixels width, RealPixels height);
        PixelSize (float scale, float aspect_ratio);
    };

    /* Aliases */
    using Coord  = unsigned int;
    using Pixels = unsigned int;
    using String = std::string;

    /* Constructors & destructors */
     Display (String title, Pixels width, Pixels height, PixelSize px_size);
    ~Display ();

    /* Public methods */
    void clear ();
    void draw  (Coord x, Coord y, unsigned height);

private:

    /* Types */
    using RectVector = std::vector <SDL_Rect>;

    /* Size data */
    Pixels width;
    Pixels height;
    PixelSize px_size;

    /* SDL components */
    SDL_Window* window;
    SDL_Surface* screenSurface;
    RectVector pixels;

    /* Misc info */
    std::string title;

    /* Methods */
    void init_pixels ();
    void init_SDL    ();
};


#endif // DISPLAY_CPP
