#include <turbo/Font.hpp>
#include <allegro5/allegro_ttf.h>
#include <stdexcept>

namespace turbo {
    Font::Font(const Path& path, unsigned short size) {
        this->font = al_load_ttf_font(path, -1 * size, 0);
        if (!this->font) {
            throw std::runtime_error(std::string("Cannot find font at \"") + ((const char*) path) + "\"");
        }
    }

    void* Font::__get_font() const {
        return this->font;
    }
}