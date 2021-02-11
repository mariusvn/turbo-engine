
#ifndef __TURBO_PATH_HPP__
#define __TURBO_PATH_HPP__

#include <allegro5/allegro.h>
#include <ostream>
#include <string>

namespace turbo {
    /**
     * @brief Path class is used to simplify the edition of paths
     */
    class Path final {
    public:
        /**
         * @brief Construct a new Path object
         * @note this constructor remove the . and .. components of the path
         * @see absolute
         * 
         * @param path initial path
         */
        Path(const char* path);
        ~Path();
        
        /**
         * @brief Get the drive of the path
         */
        const char* get_drive() const;

        /**
         * @brief Get the number of components in the path
         */
        int get_num_components() const;

        /**
         * @brief Get the component at a choosen position
         * 
         * @param i position
         */
        const char* get_component(int i) const;

        /**
         * @brief Get the last component of the path
         */
        const char* get_tail() const;

        /**
         * @brief Get the filename of the path (can be null if there is no file)
         */
        const char* get_filename() const;

        /**
         * @brief Get the basename of the file (can be null if there is no file)
         */
        const char* get_basename() const;

        /**
         * @brief Get the file extension of the path (can be null if there is no file)
         */
        const char* get_file_extension() const;


        /**
         * @brief Set the drive of the path
         * 
         * <b>Example:</b>
         * <code>
         * myPath.set_drive("C:");
         * </code>
         * @param drive target drive
         */
        void set_drive(const char* drive);

        /**
         * @brief Add a component to the path (add a folder to the end of the path)
         * 
         * <b>Example:</b>
         * <code>
         * myPath.append_component("src");
         * </code>
         * 
         * @param component component to add
         */
        void append_component(const char* component);

        /**
         * @brief Insert a component at a specified position
         * 
         * @param i posiiton
         * @param component component to insert
         */
        void insert_component(int i, const char* component);

        /**
         * @brief Replace a component at a specified position 
         * 
         * @param i position
         * @param component component to insert
         */
        void replace_component(int i, const char* component);

        /**
         * @brief Remove a compoent at a specified position
         * 
         * @param i position
         */
        void remove_component(int i);

        /**
         * @brief Removes the last component of the path
         */
        void drop_tail();

        /**
         * @brief Set the filename of the path
         * 
         * @param filename file name
         */
        void set_filename(const char* filename);

        /**
         * @brief Set the file extension of the path file
         * 
         * <b>Example:</b>
         * <code>
         * myPath.set_file_extension("png");
         * </code>
         * 
         * @param ext extension
         */
        void set_file_extension(const char* ext);


        /**
         * @brief Make the path canonical
         * removes the .. and . component and make it absolute
         */
        void absolute();

        /**
         * @brief Get the standard resource path
         * 
         * @return Path* new path that needs to be deleted after usage
         */
        static Path* get_resources_path();

        /**
         * @brief Get the path string
         * 
         * @return const char* path string
         */
        const char* c_str() const;
        operator const char* () const;
        operator std::string () const;
        friend std::ostream& operator<<(std::ostream& output, const Path& path);

    private:
        ALLEGRO_PATH* path = nullptr;
    };
}

#endif