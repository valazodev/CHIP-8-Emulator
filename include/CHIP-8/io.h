#ifndef IO_CPP
#define IO_CPP

#include <SDL2/SDL.h>
#include <array>
#include <string>
#include <vector>

class IO
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
    using Sprite = std::array <uint32_t,8>;

     IO (String title, Pixels width, Pixels height, Scale scale);
    ~IO ();

    void    clear    ();
    void    draw     (const Sprite& sprite, Coord x, Coord y);
    uint8_t last_key ();
    uint8_t wait_key ();
    void    update   ();

private:
    struct RGBA {
        uint8_t r, g, b, a;
        RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    };
    // Display
    String title;
    Pixels width;
    Pixels height;
    Scale  scale;
    std::vector <uint32_t> pixels;

    // Keyboard
    uint8_t key_value = 0xFF;
    bool    key_pressed = false;

    // SDL instances
    SDL_Event     event;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;
    SDL_Window*   window;

    // Methods
    void assert   (bool expr, std::string error_msg);
    void init_SDL ();
};


#endif // IO_CPP
