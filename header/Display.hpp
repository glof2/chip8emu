#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <vector>

class Display
{
private:
    std::vector<std::vector<bool>> m_data{};
public:
    // --- Constructors ---

    //  Description:    A Display class constructor
    //  Arguments:      width - the width of the display (px)
    //                  height - the height of the display (px)
    Display(unsigned short width, unsigned short height);

    // --- Member functions ---

    //  Name:           setPixel
    //  Description:    sets the state of the pixel at provided coordinates
    //  Arguments:      x - the X coordinate of the pixel
    //                  y - the Y coordinate of the pixel
    //                  state - the state which the pixel will be set to
    void setPixel(unsigned short x, unsigned short y, bool state);

    //  Name:            getPixel
    //  Description:     returns the state of the pixel at provided coordinates
    //  Arguments:       x - the X coordinate of the pixel
    //                   y - the Y coordinate of the pixel
    //  Return value:    The state of the pixel at (x, y) coordinates
    bool getPixel(unsigned short x, unsigned short y);

    //  Name:           flipPixel
    //  Description:    flips the pixel at the provided coordinates
    //  Arguments:      x - the X coordinate of the pixel
    //                  y - the Y coordinate of the pixel
    void flipPixel(unsigned short x, unsigned short y);

    //  Name:           setAll
    //  Description:    sets all pixels on display to provided state
    //  Arguments:      state - the state to set the pixels to
    void setAll(bool state);

    //  Name:           getWidth
    //  Description:    returns the width of the Display, assumes the height is > 0
    //  Return:         the width of the display
    unsigned short getWidth();   

    //  Name:           getHeight
    //  Description:    returns the height of the Display
    //  Return:         the height of the display
    unsigned short getHeight();
};

#endif