
#ifndef __TURBO_LOGGER_HPP__
#define __TURBO_LOGGER_HPP__

#include <string>
#include <iostream>

namespace turbo {
    /**
     * @brief The logger class is used to clarify the logging of the engine
     * 
     * If you need logger for a class, use Logger("name of the logger")
     * 
     * Otherwise, you can just use Logger::log(...) without creating a new instance
     * 
     * @note Multi argument is supported
     */
    class Logger {
    public:
        /**
         * @brief Create a new logger
         * 
         * @param name Name of the logger
         */
        Logger(std::string name): name(name) {}

        /**
         * @brief Log an info-level text
         */
        template<typename First, typename ... Rest>
        void info(First data, const Rest&... rest) {
            std::cout << "[" << this->name << "][info] " << data;
            Logger::print(rest...);
        }
        void info() { Logger::print(); }

        /**
         * @brief Log a warn-level text
         */
        template<typename First, typename ... Rest>
        void warn(First data, const Rest&... rest) {
            std::cout << "[" << this->name << "][warn] " << data;
            Logger::print(rest...);
        }
        void warn() { Logger::print(); }

        /**
         * @brief Log an error-level text
         */
        template<typename First, typename ... Rest>
        void error(First data, const Rest&... rest) {
            std::cout << "[" << this->name << "][error] " << data;
            Logger::print(rest...);
        }
        void error() { Logger::print(); }

        /**
         * @brief Log a generic-level text
         */
        template<typename First, typename ... Rest>
        static void log(First data, const Rest&... rest) {
            std::cout << "[Generic] " << data;
            Logger::print(rest...);
        }
        static void log() { std::cout << std::endl; }

    protected:
        template<typename First, typename ... Rest>
        static void print(First data, const Rest&... rest) {
            std::cout << " " << data;
            Logger::print(rest...);
        }
        static void print() { std::cout << std::endl; }
        std::string name;
    };
}

#endif