
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

    /**
     * Transform is a parent class used to make an object "movable"
     *
     * The objective is to create a 4x4 transformation matrix used to calculate the rendering
     */
    class Transform {
    public:
        Transform() = default;
        Transform(const Vector2<float> &position, float angle = 0, const Vector2<float> &scale = Vector2<float>(1, 1), const Vector2<int> &center = Vector2<int>());
        Transform(float x, float y = 0, int scaleX = 1, int scaleY = 1, float angle = 0, int centerX = 0, int centerY = 0);
        ~Transform();

        /**
         * @brief Translate the object in 2D
         * @param x X Axis
         * @param y Y Axis
         */
        void translate(float x, float y);

        /**
         * @brief Translate the object in 22
         * @param offset 2D vector
         */
        void translate(const Vector2<float> &offset);

        /**
         * @brief Set the position of the object
         * @param x X axis
         * @param y Y axis
         */
        void set_position(float x, float y);

        /**
         * @brief Set the position of the object
         * @param pos 2D vector
         */
        void set_position(const Vector2<float> &pos);

        /**
         * @brief Rotate the current object by `deg` degrees
         * @warning The rotation is added in this function. If you want to set the
         * rotation check set_rotation
         * @param deg Rotation in degrees
         */
        void rotate(float deg);

        /**
         * @brief Set the rotation of the object
         * @param deg Rotation in degrees [0 - 360]
         */
        void set_rotation(float deg);

        /**
         * @brief Set the center of rotation of the object (Start in 0,0 , position in pixel)
         * @param x X axis (pixels)
         * @param y Y axis (pixels)
         */
        void set_center(int x, int y);

        /**
         * @brief Set the center of rotation of the object (Start in 0,0 , position in pixel)
         * @param center 2D vector
         */
        void set_center(const Vector2<int> &center);

        /**
         * @brief Set the scale of the object (base scale == 1.0)
         * @param scale 2D scale
         */
        void set_scale(const Vector2<float> &scale);

        /**
         * @brief Set the scale of the object (base scale == 1.0)
         * @param scale_x Scale on x axis
         * @param scale_y Scale on Y axis
         */
        void set_scale(float scale_x, float scale_y);

        /**
         * Retrieves the position of the object
         * @return position
         */
        const Vector2<float> &get_position() const;

        /**
         * Retrieves the scale of the object
         * @return scale
         */
        const Vector2<float> &get_scale() const;

        /**
         * Retrieves the rotation center of the object
         * @return position
         */
        const Vector2<int> &get_center() const;

        /**
         * Retrieves the rotation of the object
         * @return angle in degrees
         */
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