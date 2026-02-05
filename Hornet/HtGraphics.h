// Modified 19/04/2024
// Added function to draw angledrectangles
// Set alpha blending from initialisation
// Added some comments to FillCircle to note limitations
// Modified 3/5/24 Altered Present so it always clears with background colour
// then uses the texture if there is one. This avoids issues
// with texture including colour key for transparency.
// Modified 5/6/24
// Added FillAngledRect
// Corrected bug in DrawTextCentred to make it actually centred
// Changed PictureIndex to prevent free casting with int. I was making that mistake too often
// Added BROWN
// Modified 6/6/24
// Corrected DrawPointList
// Modified 7/6/24
// LoadImage will try to load from "assets\"+file if it fails,
// in case programmer just forgot to add the folder path.
// Modified 10/6/24
// Fixed WriteTextCentered angle.

// 02/06/2025 Added ability to flip images in DrawAt().

// 16/09/2025    Added FillRect and FillAngledRect using pictures.

#pragma once
#include "Vector2D.h"
#include "Shapes.h"
#include "Result.h"
#include <map>
#include "SDL.h"
#include <string>
#include "SDL_ttf.h"
#pragma comment(lib, "SDL2_image")
#pragma comment(lib, "SDL2_ttf")



enum class PictureIndex : int {};
const PictureIndex NO_PICTURE_INDEX = (PictureIndex)-1;
typedef int FontIndex;
enum class FlipType { NONE, HORIZONTAL, VERTICAL };     // Want both? Just rotate by 180 degrees!

// This struct stores information about a texture, used internally
// by the HtGraphics class. There is no reason to use this
// outside the class at the moment. So you can pretty much ignore
// this unless you plan to modify HtGraphics
struct Texture
{
    SDL_Texture* pTexture;
    int height;
    int width;
};

// Stores a colour used by HtGraphics in various fuctions.
// ARGB values are all 0-255
struct Colour
{
    int a;      // The alpha value 0-255. Used for transparency
    int r;      // The red value 0-255
    int g;      // The green value 0-255
    int b;      // The blue value 0-255

    // Constructs a cololur using supplied values of alpha, red, greem, blue.
    // There is no bounds checking, so make sure the values are all 0-255
    Colour(int alpha, int red, int green, int blue)
            :a(alpha), r(red), g(green), b(blue)
    {
    }

    // Constructs a default colour value, with pure opaque black.
    Colour(): a(255), r(0), g(0), b(0)
    {
    }

    // Returns an equivalent SDL_Color for the Colour.
    SDL_Color ToSDLColor()
    {
        SDL_Color ret;
        ret.a = a;
        ret.r = r;
        ret.g = g;
        ret.b = b;
        return ret;
    }

    // Compares two colours. True if and only if the a,r,g and b values are identical
    bool operator==(const Colour& other)const
    {
        return (a == other.a && r == other.r && g == other.g && b == other.b);
    }

    // Compares two colours true if any of the four values (a,r,g,b) are different
    bool operator!=(const Colour& other)const
    {
        return !(*this == other);
    }
};


// Class to manage graphics functions in the hornet engine
class HtGraphics
{
public:
    // Singleton instance. You can use the instance in any part of your code
    // with HtGraphics::instance.DoSomething()
    static HtGraphics instance;

    // Initialises the instance. Result is SUCCESS if the SDL system initialised
    // correctly, FAILURE otherwise
    Result Initialise();

    // Sets the background colour to the specified colour. This will be drawn
    // on the back buffer during Present() and will be the background to the game
    // By default the background is black.
    void SetBackgroundColour(Colour colour);

    // Sets a background texture instead of a colour during Present().
    // "picture" is the PictureIndex of the desired background image, and should
    // have been previously loaded using LoadPicture()
    // The image will be stretched to fill the screen, which will cause some distortion
    // if the aspect ratio of the image does not match the aspect ratio of the screen
    void SetBackgroundTexture(PictureIndex picture);

    // This sets the colour that will be used to indicate "transparent" on images.
    // By default this is black.
    // This only affects images that are loaded after this function is called.
    void SetTransparentColour(const Colour& colour);

    // Returns the colour currently uses as the colour key for transparency
    // (By default, BLACK)
    Colour GetTransparentColour() const;

    // Loads a specified font for use with some text functions.
    // "filename" is the name of the font (usually combined with a filepath
    // such as "fonts\\guardian.ttf")
    // "pointsize" is the size of the font. For many functions, the image
    // can be scaled when drawing so it is usually best to pick a large
    // font size such as 24 or 48 and scale later.
    // Returns a FontIndex (effectivly just a number) that is used to refer to this
    // font in other functions.
    FontIndex LoadFont(const std::string filename, int pointSize);

    // Loads a specified picture and adds it to an internal list of images.
    // The function returns a PictureIndex (really just a number) that can be used
    // to access the picture later for drawing.
    // If the code loads a picture filename that has already been loaded, the function
    // will not re-load the image and will instead return the index of the picture previously 
    // loaded.
    // There is normally no need to unload images - this will be handled when Shutdown()
    // is called when the program exits. You can, however, manually release an image
    // using ReleasePicture() if you need to, or all pictures using ReleaseAllPictures().
    // "filename" - the filename of the image, including path if needed, such as 
    //   "assets\\spaceship.bmp"
    // Supported file types are .bmp, .png, .jpg
    // .jpg can produce unwanted effects when scaling due to the lossy nature of JPEG files.
    // .png supports transparency
    // .bmp and .png are usually the best file types to use
    PictureIndex LoadPicture(const std::string filename);

    int GetWidthOfPicture(PictureIndex pic);
    int GetHeightOfPicture(PictureIndex pic);


    // Creates a picture from text. This can be used to create an image from text that will be used
    // frequently in the game, which is more efficient than using the other text functions, which
    // usually need to create an image each time. The image can be rendered using the same drawing functions
    // that are used for images loaded from file
    // CAUTION CAUTION CAUTION - This function will create a new image each time it is called, even
    // if an identical image has been created previously. If you call this each frame, it will
    // create a lot of images, rapidly using up system resources.
    // "text" - The text to appear in the image
    // "fontindex" - The index of the font (previously loaded using LoadFont)
    // "textColour" - The colour of the text rendered
    // Note that the background colour of the text will be whatever is currently set as the "transparency" 
    // colour. Normally this is black, but can be changed using SetTransparentColour()
    PictureIndex CreatePictureFromText(const std::string text, FontIndex fontIndex, Colour textColour);

    // Draws the specified picture in the specified location, using the current camera settings
    // "centre" - The coordinates of the centre of the image
    // "picture" - The index of the requested picture, normally previously loaded using LoadImage()
    // "scale" - The scale of the image. Default is 1.0. This will be also scaled by the camera settings
    // "angle" - The rotation angle of the image, in degrees, rotated clockwise. NOTE: Uses degrees, not radians
    // "transparency" - How transparent you want the entire image to be. 0 is fully opaque. 1.0 is invisible
    Result DrawAt(Vector2D centre, PictureIndex picture, double scale=1.0, double angle=0, double transparency=0, FlipType flip = FlipType::NONE);

    // Fills the specified rectangle with the specified colour, using the current camera settings
    Result FillRect(Rectangle2D rect, Colour colour);

    // Fills the specified rectangle with the specified image, using the current camera settings
    Result FillRect(Rectangle2D rect, PictureIndex texture, double transparency = 0.0);

    // Fills the specified angled rectangle with the specified colour
    // using the current camera settings.
    // NOTE: This function is slightly slow, so should not be used extensively
    // (i.e. more than about 20 times per frame)
    Result FillAngledRect(AngledRectangle2D rect, Colour colour);

    // Fills the specified angled rectangle with the specified image
    // using the current camera settings.
    // NOTE: This function is slightly slow, so should not be used extensively
    // (i.e. more than about 20 times per frame)
    Result FillAngledRect(AngledRectangle2D rect, PictureIndex texture, double transparency = 0.0);

    // Fills the specified circle with the specified colour, using the current camera settings
    // NOTE: FillCircle is relatively slow. It may be faster to use a bitmap of a circle and render that
    // NOTE: Alpha Transparency will be unsatisfactory when used with FillCircle
    //      and should be avoided
    Result FillCircle(Circle2D circle, Colour colour);
    Result FillCircle(Vector2D centre, double radius, Colour colour);

    // Draws the specified segment with the specified colour, using the current camera settings
    Result DrawSegment(Segment2D segment, Colour colour);
    Result DrawSegment(Vector2D start, Vector2D end, Colour colour);

    // Fills a single pixel with the specified colour, using the current camera settings
    Result DrawPoint(Vector2D point, Colour colour);
    // Fills and array of pixel with the specified colour, using the current camera settings
    Result DrawPointList(const Vector2D points[], Colour colour, const int numPoints);

    // Writes text at the specified location. The picture will be centred at the specified location.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "centre" - The coordinates of the centre of the text, using the current camera settings
    // "text" - The text
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteTextCentered(Vector2D centre, std::string text, Colour colour, FontIndex font=0, double angle = 0, double scale = 1.0);

    // Writes text at the specified location. The top left of the text will be the specified location,
    // which makes this function more useful for GUIs.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "topleft" - The coordinates of the top left of the text, using the current camera settings
    // "text" - The text
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteTextAligned(Vector2D topLeft, std::string text, Colour colour, FontIndex font = 0, double scale = 1.0);

    // Writes text at the specified location. The top left of the text will be the specified location,
    // which makes this function more useful for GUIs.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "topLeftX", "topLeftY" - The coordinates of the top left of the text, using the current camera settings
    // "text" - The text
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteTextAligned(int topLeftX, int topLeftY, std::string text, Colour colour, FontIndex font = 0, double scale = 1.0);

    // Writes an integer at the specified location. The picture will be centred at the specified location.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "centre" - The coordinates of the centre of the text, using the current camera settings
    // "number" - The number
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteIntCentered(Vector2D centre, int number, Colour colour, FontIndex font=0, double angle = 0, double scale = 1.0);

    // Writes an integer at the specified location. The top left of the text will be the specified location,
    // which makes this function more useful for GUIs.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "topleft" - The coordinates of the top left of the text, using the current camera settings
    // "number" - The number
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteIntAligned(Vector2D topLeft, int number, Colour colour, FontIndex font = 0, double scale = 1.0);

    // Writes an integer at the specified location. The top left of the text will be the specified location,
    // which makes this function more useful for GUIs.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "topLeftX", "topLeftY" - The coordinates of the top left of the text, using the current camera settings
    // "number" - The number
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteIntAligned(int topLeftX, int topLeftY, int number, Colour colour, FontIndex font = 0, double scale = 1.0);

    // Writes a floating point number at the specified location. The picture will be centred at the specified location.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "centre" - The coordinates of the centre of the text, using the current camera settings
    // "number" - The number
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteFloatCentered(Vector2D centre, double number, Colour colour, FontIndex font=0, double angle = 0, double scale = 1.0);

    // Writes a floating point number at the specified location. The top left of the text will be the specified location,
    // which makes this function more useful for GUIs.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "topleft" - The coordinates of the top left of the text, using the current camera settings
    // "number" - The number
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteFloatAligned(Vector2D topLeft, double number, Colour colour, FontIndex font = 0, double scale = 1.0);

    // Writes a floating point number at the specified location. The top left of the text will be the specified location,
    // which makes this function more useful for GUIs.
    // Note that this function is a bit slow. If you are drawing hundreds of samples of text each
    // frame, this may cause some slowdown
    // "topLeftX", "topLeftY" - The coordinates of the top left of the text, using the current camera settings
    // "number" - The number
    // "colour" - The colour used for the text (the background will be transparent)
    // "font" - The index of the font to be used (previously loaded using LoadFont())
    // "angle" - The angle of rotation of the text, clockwise in degrees (not radians)
    // "scale" - The scale of the text. This will also be scaled by current camera settings.
    Result WriteFloatAligned(int topLeftX, int topLeftY, double number, Colour colour, FontIndex font = 0, double scale = 1.0);

    // Returns the width of the window (usually also the screen) in pixel units.
    // If the resolution is 1920 x 1080, this function will return 1920.
    // This is different to the in-game units. See the HtCamera class for functions 
    // that give you in-game dimensions
    int GetWindowWidth() const;

    // Returns the height of the window (usually also the screen) in pixel units.
    // If the resolution is 1920 x 1080, this function will return 1080.
    // This is different to the in-game units. See the HtCamera class for functions 
    // that give you in-game dimensions
    int GetWindowHeight() const;

    // Presents the back buffer to the screen at the end of each frame of animation
    // This is normally called once in each game loop, once all drawing is complete.
    // Typically this will be called in Game::Update()
    // The back buffer will be drawn (flipped) to the screen, and a new back buffer 
    // will be prepared. The new back buffer will be cleared, using the current
    // background colour (or texture), which is black by default.
    Result Present();

    // Closes down the class intance. This releases all resources.
    void Shutdown();

    // This function can be slow if there are a lot of images.
    // Generally best to just let the system release everything at the of the program,
    // since duplicate files are not actually loaded.
    void ReleasePicture(PictureIndex picture);

    // Releases the resources for all pictures. This is called from Shutdown()
    // and you normally don't need to call this manually.
    void ReleaseAllPictures();

    // Releases the resources for all fonts. This is called from Shutdown()
    // and you normally don't need to call this manually.
    void ReleaseAllFonts();

    // Some standard colours
    static const Colour BLACK;
    static const Colour RED;
    static const Colour GREEN;
    static const Colour BLUE;
    static const Colour DARKRED;
    static const Colour DARKGREEN;
    static const Colour DARKBLUE;
    static const Colour LIGHTRED;
    static const Colour LIGHTGREEN;
    static const Colour LIGHTBLUE;
    static const Colour WHITE;
    static const Colour YELLOW;
    static const Colour CYAN;
    static const Colour PURPLE;
    static const Colour GREY;
    static const Colour SKYBLUE;
    static const Colour BROWN;
private:
    HtGraphics();
    HtGraphics(HtGraphics& h) = delete;
    ~HtGraphics();
    std::map<PictureIndex, Texture> m_pictureMap;	        // Map of textures
    std::map<std::string, PictureIndex> m_filenameMap;	    // Map of filenames
    std::map<FontIndex, TTF_Font*> m_fontList;	            // Map of textures
    PictureIndex m_nextPictureIndex;        // The index of the next font to be added
    FontIndex m_nextFontIndex;		        // The index of the next font to be added
    SDL_Renderer* m_pRenderer;              // Pointer to the SDL renderer
    SDL_Window* m_pWindow;                  // Pointer to the SDL window
    Colour m_backgroundColour;              // The current colour used as the background during Present(). Black by default
    Colour m_transparentColour;             // The current colour key used for transparency
    PictureIndex m_backGroundTexture;       // The texture used as a background during Present. If less than 0, Present() will use the background colour instead
    int m_windowWidth;                      // The height of the window in pixels.
    int m_windowHeight;                     // The width of the window in pixels.

    // Function used to set the current colour to be used by the SDL renderer
    Result SetRenderColour(const Colour& colour);

    // Creates a texture from text. Called by CreatePictureFromText()
    Texture CreateTextureFromText(const std::string text, FontIndex fontIndex, Colour textColour);
};

