
#ifndef __TURBO_ENGINE_TILESET_HPP__
#define __TURBO_ENGINE_TILESET_HPP__

#include "../Path.hpp"
#include "../Vector2.hpp"
#include "Texture.hpp"
#include "Sprite.hpp"
#include <stdexcept>
#include <vector>

namespace turbo {
    class Tileset {
    public:
        /**
         * Create a tileset
         * @param path path to the texture file
         * @param tile_size size of the tile
         * @param tiles_nbr amount of tiles
         * @param margin margin between the tiles
         * @param border_offset offset around all the textures
         */
        explicit Tileset(Path* path, const Vector2<unsigned short>& tile_size, const Vector2<unsigned short> &tiles_nbr, const unsigned short& margin = 0, const unsigned short& border_offset = 0);
        ~Tileset();

        Sprite* get_sprite(const Vector2<unsigned short>& coords) const noexcept(false);
        Sprite* get_sprite(unsigned short x, unsigned short y) const noexcept(false);
        Sprite* operator() (const Vector2<unsigned short>& coords) const noexcept(false);
        Sprite* operator() (unsigned short x, unsigned short y) const noexcept(false);
        const Vector2<unsigned short>& get_size() const;

    private:
        void init_sprites();

        Vector2<unsigned short> size = Vector2<unsigned short>(0,0);
        Vector2<unsigned short> tile_amount = Vector2<unsigned short>(0,0);
        Texture* texture = nullptr;
        unsigned short margin = 0;
        unsigned short border_offset = 0;
        std::vector<std::vector<Sprite*>> sprites {};
    };
}

#endif
