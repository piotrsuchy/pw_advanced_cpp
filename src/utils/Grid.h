#ifndef PACMAN_GRID_H
#define PACMAN_GRID_H

#include <vector>
#include <stdexcept>

/**
 * @brief Template class for managing 2D grid-based data
 * @tparam T The type of elements stored in the grid
 *
 * This class provides a 2D grid implementation that can store any type of data.
 * It's used primarily for managing the game maze and entity positions.
 */
template <typename T>
class Grid
{
public:
    /**
     * @brief Construct a new Grid
     * @param width Width of the grid
     * @param height Height of the grid
     * @param defaultValue Default value for grid cells
     */
    Grid(size_t width, size_t height, const T &defaultValue = T())
        : width_(width), height_(height), data_(width * height, defaultValue)
    {
    }

    /**
     * @brief Get the value at specified coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return Reference to the value at (x,y)
     * @throws std::out_of_range if coordinates are invalid
     */
    T &at(size_t x, size_t y)
    {
        if (x >= width_ || y >= height_)
        {
            throw std::out_of_range("Grid coordinates out of range");
        }
        return data_[y * width_ + x];
    }

    /**
     * @brief Get the const value at specified coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return Const reference to the value at (x,y)
     * @throws std::out_of_range if coordinates are invalid
     */
    const T &at(size_t x, size_t y) const
    {
        if (x >= width_ || y >= height_)
        {
            throw std::out_of_range("Grid coordinates out of range");
        }
        return data_[y * width_ + x];
    }

    /**
     * @brief Get the width of the grid
     * @return Grid width
     */
    size_t getWidth() const { return width_; }

    /**
     * @brief Get the height of the grid
     * @return Grid height
     */
    size_t getHeight() const { return height_; }

    /**
     * @brief Clear the grid with a default value
     * @param value Value to fill the grid with
     */
    void clear(const T &value = T())
    {
        std::fill(data_.begin(), data_.end(), value);
    }

private:
    size_t width_;        ///< Grid width
    size_t height_;       ///< Grid height
    std::vector<T> data_; ///< Grid data storage
};

#endif // PACMAN_GRID_H