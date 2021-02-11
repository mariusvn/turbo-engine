
#ifndef __TURBO_TRANSFORM_HPP__
#define __TURBO_TRANSFORM_HPP__

#include <allegro5/allegro.h>
#include "Vector2.hpp"

#define START_TRANSFORM(t)  {                               \
                                al_use_transform(t);        \
                            }

#define END_TRANSFORM(t)    {                               \
                                al_identity_transform(t);   \
                                al_use_transform(t);        \
                            }

namespace turbo {
    class Transform {
    public:
        Transform() = default;
        Transform(const Vector2<float> &position, float angle = 0, const Vector2<float> &scale = Vector2<float>(1, 1), const Vector2<int> &center = Vector2<int>());
        Transform(float x, float y = 0, int scaleX = 1, int scaleY = 1, float angle = 0, int centerX = 0, int centerY = 0);
        ~Transform();

        void translate(float x, float y);
        void translate(const Vector2<float> &offset);
        void set_position(float x, float y);
        void set_position(const Vector2<float> &pos);
        void rotate(float deg);
        void set_rotation(float deg);
        void set_center(int x, int y);
        void set_center(const Vector2<int> &center);
        void set_scale(const Vector2<float> &scale);
        void set_scale(float scale_x, float scale_y);

        const Vector2<float> &get_position() const;
        const Vector2<float> &get_scale() const;
        const Vector2<int> &get_center() const;
        float get_angle() const;

    protected:
        /**
         * @brief angle of the object
         * @warning The angle should always be in [0 - 360]
         */
        float angle = 0;
        Vector2<float> position = Vector2<float>();
        Vector2<float> scale = Vector2<float>(1, 1);
        Vector2<int> center = Vector2<int>();
        ALLEGRO_TRANSFORM *transform = new ALLEGRO_TRANSFORM();
        bool property_has_changed = true;
        ALLEGRO_TRANSFORM *get_transformer();
        static inline ALLEGRO_TRANSFORM default_transform = ALLEGRO_TRANSFORM();
    };
}

#endif