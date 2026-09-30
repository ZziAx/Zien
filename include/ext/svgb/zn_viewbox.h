/**
 * @brief Represents a rectangular view box with position and size.
 * 
 * This class stores the x/y coordinates of the top-left corner,
 * as well as the width and height of the rectangle.
 * 
 * Provides constructors for default and explicit initialization,
 * and a helper method to create a ZN_ViewBox from a whitespace-separated string.
 */
class ZN_ViewBox
{
public:
    float x;      ///< X-coordinate of the top-left corner
    float y;      ///< Y-coordinate of the top-left corner
    float width;  ///< Width of the view box
    float height; ///< Height of the view box

    ZN_ViewBox() : x(0), y(0), width(0), height(0) {}
    ZN_ViewBox(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

    //  ZN_ViewBox()=default;

    /**
     * @brief Create a ZN_ViewBox from a whitespace-separated string "x y width height".
     * @param str Input string
     * @return ZN_ViewBox Initialized view box or default if parsing fails
     */
    static ZN_ViewBox from_string(const std::string &str)
    {
        std::vector<std::string> vec = ZN_UtfHelper::split(str);

        if (vec.size() != 4)
        {
            return ZN_ViewBox();
        }

        float x = std::stof(vec[0]);
        float y = std::stof(vec[1]);
        float width = std::stof(vec[2]);
        float height = std::stof(vec[3]);

        return ZN_ViewBox(x, y, width, height);
    }
};
