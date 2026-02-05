#include "HtGraphics.h"
#include "ErrorLogger.h"
#include "SDL_Image.h"
#include "HornetApp.h"
#include "HtCamera.h"
#include "sdl.h"




HtGraphics HtGraphics::instance;

const Colour HtGraphics::BLACK = Colour(255,0,0,0);
const Colour HtGraphics::RED = Colour(255, 255, 0, 0);
const Colour HtGraphics::GREEN = Colour(255, 0, 255, 0);
const Colour HtGraphics::BLUE = Colour(255, 0, 0, 255);
const Colour HtGraphics::DARKRED = Colour(255, 128, 0, 0);
const Colour HtGraphics::DARKGREEN = Colour(255, 0, 128, 0);
const Colour HtGraphics::DARKBLUE = Colour(255, 0, 0, 128);
const Colour HtGraphics::LIGHTRED = Colour(255, 255, 128, 128);
const Colour HtGraphics::LIGHTGREEN = Colour(255, 128, 255, 128);
const Colour HtGraphics::LIGHTBLUE = Colour(255, 128, 128, 255);
const Colour HtGraphics::WHITE = Colour(255, 255, 255, 255);
const Colour HtGraphics::YELLOW = Colour(255, 255, 255, 0);
const Colour HtGraphics::CYAN = Colour(255, 0, 255, 255);
const Colour HtGraphics::PURPLE = Colour(255, 255, 0, 255);
const Colour HtGraphics::GREY = Colour(255, 128, 128, 128);
const Colour HtGraphics::BROWN = Colour(255, 150, 75, 0);
const Colour HtGraphics::SKYBLUE = Colour(255, 135, 206, 235);


PictureIndex HtGraphics::LoadPicture(const std::string filename)
{
    // See if file is already loaded
    auto it = m_filenameMap.find(filename);
    if (it != m_filenameMap.end())
    {
        // If it is, return the existing picture index
        return it->second;
    }

    if (!m_pRenderer)
    {
        ErrorLogger::Write("Could run HtGraphics::AddPicture():"+filename);
        ErrorLogger::Write("m_pRenderer is not initialised.Run \"Initialise()\" first.");
    }

    SDL_Surface* pImageSurface = IMG_Load(filename.c_str());
    if (pImageSurface == nullptr)
    {
        // Try again with "assets/" in front
        pImageSurface = IMG_Load((std::string("assets/")+filename).c_str());
    }
    SDL_Texture* pTexture = nullptr;
    if (pImageSurface == nullptr)
    {
        ErrorLogger::Write("Could not load texture image in HtGraphics::AddPicture():"+filename);
        ErrorLogger::Write(SDL_GetError());
        return NO_PICTURE_INDEX;
    }

    SDL_SetColorKey(pImageSurface, SDL_TRUE, SDL_MapRGB(pImageSurface->format, 
        m_transparentColour.r, m_transparentColour.g, m_transparentColour.b));

    //Create texture from surface pixels
    Texture texture;
    texture.pTexture= SDL_CreateTextureFromSurface(m_pRenderer, pImageSurface);
    if (texture.pTexture == nullptr)
    {
        ErrorLogger::Write("Could not create texture image in HtGraphics::AddPicture():"+filename);
        ErrorLogger::Write(SDL_GetError());
        return NO_PICTURE_INDEX;
    }
    texture.width = pImageSurface->w;
    texture.height = pImageSurface->h;

    //Get rid of old loaded surface
    SDL_FreeSurface(pImageSurface);

    // Add the texture to the map
    m_pictureMap.insert(std::pair<PictureIndex, Texture>(m_nextPictureIndex, texture));

    // Add the filename to the map
    m_filenameMap.insert(std::pair<std::string, PictureIndex>(filename, m_nextPictureIndex));

    PictureIndex oldIndex = m_nextPictureIndex;
    m_nextPictureIndex = (PictureIndex)(int(m_nextPictureIndex)+1);
    return oldIndex;
    // Caution. Early returns in this function.
}

int HtGraphics::GetWidthOfPicture(PictureIndex pic)
{
    std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.find(pic);
    if (picit == m_pictureMap.end())
    {
        return 0;
    }
    else
    {
        return picit->second.width;
    }
}

int HtGraphics::GetHeightOfPicture(PictureIndex pic)
{
    std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.find(pic);
    if (picit == m_pictureMap.end())
    {
        return 0;
    }
    else
    {
        return picit->second.height;
    }
}

PictureIndex HtGraphics::CreatePictureFromText(const std::string text, FontIndex fontIndex, Colour textColour)
{
    Texture texture = CreateTextureFromText(text, fontIndex, textColour);

    if (texture.pTexture)
    {
        // Add the texture to the map
        m_pictureMap.insert(std::pair<PictureIndex, Texture>(m_nextPictureIndex, texture));
        PictureIndex oldIndex = m_nextPictureIndex;
        m_nextPictureIndex = (PictureIndex)(int(m_nextPictureIndex) + 1);
        return oldIndex;
    }
    else
    {
        ErrorLogger::Write("Texture not created in  HtGraphics::CreateTextureFromText().");
        return NO_PICTURE_INDEX;
    }

}

Result HtGraphics::DrawAt(Vector2D centre, PictureIndex picture, double scale, double angle, double transparency, FlipType flip)
{


    std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.find(picture);

    if (picit == m_pictureMap.end())
    {
        HtGraphics::instance.WriteTextCentered(centre, "Image not loaded", HtGraphics::instance.LIGHTGREEN,
            0, 0, scale);
        ErrorLogger::Write("PictureIndex not found in HtGraphics::DrawAt().");
        return Result::FAILURE;
    }

    centre = HtCamera::instance.NativeTransform(centre);
    scale = HtCamera::instance.NativeTransform(scale);

    //Render texture to screen
    SDL_Rect targetArea;
    targetArea.h = int(picit->second.height*scale);
    targetArea.w = int(picit->second.width*scale);
    targetArea.x = int(centre.XValue-targetArea.w /2);
    targetArea.y = int(centre.YValue-targetArea.h / 2);

    // Render. Parameters are: the renderer, the texture, the area of the texture (NULL for all),
    // the area of the target (calculated above), the rotation angle in degrees, the centre of rotation
    // (NULL for image centre) and FLIP flags.

    // Do I need to do this every time? Or just do it when I set up the engine?
    SDL_SetTextureBlendMode(picit->second.pTexture, SDL_BLENDMODE_BLEND);


    SDL_SetTextureAlphaMod(picit->second.pTexture, Uint8(255 * (1.0 - transparency)));

    SDL_RendererFlip f = SDL_FLIP_NONE;
    if (flip == FlipType::HORIZONTAL) f = SDL_FLIP_HORIZONTAL;
    else if (flip == FlipType::VERTICAL) f = SDL_FLIP_VERTICAL;
    else if (flip == FlipType::VERTICAL) f = SDL_FLIP_VERTICAL;

    if (SDL_RenderCopyEx(m_pRenderer, picit->second.pTexture, NULL, &targetArea, angle, NULL, f) < 0)
    {
        ErrorLogger::Write("Could not render image in HtGraphics::DrawAt().");

#ifdef _DEBUG
        for (std::pair<const std::string, PictureIndex>& p : m_filenameMap)
        {
            if (p.second == picture)
            {
                ErrorLogger::Write("Image: " + p.first);
            }
        }
#endif // DEBUG

        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    return Result::SUCCESS;
}

Result HtGraphics::FillRect(Rectangle2D rect, Colour colour)
{
    rect = HtCamera::instance.NativeTransform(rect);

    SDL_Rect fillRect;
    fillRect.x = int(rect.GetBottomLeft().XValue);
    fillRect.y = int(rect.GetBottomLeft().YValue);
    fillRect.w = int(rect.GetTopRight().XValue - rect.GetBottomLeft().XValue);
    fillRect.h = int(rect.GetTopRight().YValue - rect.GetBottomLeft().YValue);

    SDL_SetRenderDrawColor(m_pRenderer, colour.r, colour.g, colour.b, colour.a );
    if (SDL_RenderFillRect(m_pRenderer, &fillRect) < 0)
    {
        ErrorLogger::Write("Could not fill rectangle in HtGraphics::FillRect().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }

    return Result::SUCCESS;
}

// Fills the specified rectangle with the specified image, using the current camera settings
Result HtGraphics::FillRect(Rectangle2D rect, PictureIndex texture, double transparency)
{
    // Find the right image
    std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.find(texture);

    // Can't find the image
    if (picit == m_pictureMap.end())
    {
        HtGraphics::instance.WriteTextCentered(rect.GetTopLeft(), "Image not loaded", HtGraphics::instance.LIGHTGREEN, 0, 0, 1.0);
        ErrorLogger::Write("PictureIndex not found in HtGraphics::DrawAt().");
        return Result::FAILURE;
    }

    rect = HtCamera::instance.NativeTransform(rect);

    SDL_Rect fillRect;
    fillRect.x = int(rect.GetBottomLeft().XValue);
    fillRect.y = int(rect.GetBottomLeft().YValue);
    fillRect.w = int(rect.GetTopRight().XValue - rect.GetBottomLeft().XValue);
    fillRect.h = int(rect.GetTopRight().YValue - rect.GetBottomLeft().YValue);

    // Render. Parameters are: the renderer, the texture, the area of the texture (NULL for all),
    // the area of the target (calculated above), the rotation angle in degrees, the centre of rotation
    // (NULL for image centre) and FLIP flags.

    // Do I need to do this every time? Or just do it when I set up the engine?
    SDL_SetTextureBlendMode(picit->second.pTexture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureAlphaMod(picit->second.pTexture, Uint8(255 * (1.0 - transparency)));

    SDL_RendererFlip f = SDL_FLIP_NONE;

    if (SDL_RenderCopyEx(m_pRenderer, picit->second.pTexture, NULL, &fillRect, 0, NULL, f) < 0)
    {
        ErrorLogger::Write("Could not render image in HtGraphics::FillRect().");

#ifdef _DEBUG
        for (std::pair<const std::string, PictureIndex>& p : m_filenameMap)
        {
            if (p.second == texture)
            {
                ErrorLogger::Write("Image: " + p.first);
            }
        }
#endif // DEBUG

        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    return Result::SUCCESS;
}

Result HtGraphics::FillAngledRect(AngledRectangle2D rect, Colour colour)
{
    rect = HtCamera::instance.NativeTransform(rect);

    Vector2D corner1off(-rect.GetWidth() / 2, rect.GetHeight() / 2);
    Vector2D corner2off(rect.GetWidth() / 2, rect.GetHeight() / 2);

    Vector2D corner1 = rect.GetCentre() + corner1off.rotatedBy(rect.GetAngle());
    Vector2D corner2 = rect.GetCentre() + corner2off.rotatedBy(rect.GetAngle());
    Vector2D corner3 = rect.GetCentre() + corner1off.rotatedBy(rect.GetAngle()+180);
    Vector2D corner4 = rect.GetCentre() + corner2off.rotatedBy(rect.GetAngle()+180);
    SDL_Color col = colour.ToSDLColor();

    SDL_Vertex vertex_1 = { {(float)corner1.XValue, (float)corner1.YValue}, col, {1, 1} };
    SDL_Vertex vertex_2 = { {(float)corner2.XValue, (float)corner2.YValue}, col, {1, 1} };
    SDL_Vertex vertex_3 = { {(float)corner3.XValue, (float)corner3.YValue}, col, {1, 1} };
    SDL_Vertex vertex_4 = { {(float)corner4.XValue, (float)corner4.YValue}, col, {1, 1} };

    SDL_Vertex vertices[] = {
    vertex_1,
    vertex_2,
    vertex_3,
    vertex_1,
    vertex_3,
    vertex_4,
    };


    SDL_SetRenderDrawColor(m_pRenderer, colour.r, colour.g, colour.b, colour.a);
  //  SDL_RenderClear(m_pRenderer);


    if (SDL_RenderGeometry(m_pRenderer, nullptr, vertices, 6, NULL, 0) < 0)
    {
        ErrorLogger::Write("Could not fill rectangle in HtGraphics::FillAngledRect().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    SDL_RenderPresent(m_pRenderer);
    return Result::SUCCESS;
}

Result HtGraphics::FillAngledRect(AngledRectangle2D rect, PictureIndex texture, double transparency)
{
    // Find the right image
    std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.find(texture);

    // Can't find the image
    if (picit == m_pictureMap.end())
    {
        HtGraphics::instance.WriteTextCentered(rect.GetCentre(), "Image not loaded", HtGraphics::instance.LIGHTGREEN, 0, 0, 1.0);
        ErrorLogger::Write("PictureIndex not found in HtGraphics::DrawAt().");
        return Result::FAILURE;
    }

    rect = HtCamera::instance.NativeTransform(rect);

    SDL_Rect fillRect;
    fillRect.x = int(rect.GetCentre().XValue);
    fillRect.y = int(rect.GetCentre().YValue);
    fillRect.w = int(rect.GetWidth());
    fillRect.h = int(rect.GetHeight());

    // Render. Parameters are: the renderer, the texture, the area of the texture (NULL for all),
    // the area of the target (calculated above), the rotation angle in degrees, the centre of rotation
    // (NULL for image centre) and FLIP flags.

    // Do I need to do this every time? Or just do it when I set up the engine?
    SDL_SetTextureBlendMode(picit->second.pTexture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureAlphaMod(picit->second.pTexture, Uint8(255 * (1.0 - transparency)));

    SDL_RendererFlip f = SDL_FLIP_NONE;

    if (SDL_RenderCopyEx(m_pRenderer, picit->second.pTexture, NULL, &fillRect, -rect.GetAngle(), NULL, f) < 0)
    {
        ErrorLogger::Write("Could not render image in HtGraphics::FillAngledRect().");

#ifdef _DEBUG
        for (std::pair<const std::string, PictureIndex>& p : m_filenameMap)
        {
            if (p.second == texture)
            {
                ErrorLogger::Write("Image: " + p.first);
            }
        }
#endif // DEBUG

        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    return Result::SUCCESS;
}

Result HtGraphics::FillCircle(Circle2D circle, Colour colour)
{
    return FillCircle(circle.GetCentre(), circle.GetRadius(), colour);
}

Result HtGraphics::FillCircle(Vector2D centre, double radius, Colour colour)
{
    centre = HtCamera::instance.NativeTransform(centre);
    radius = HtCamera::instance.NativeTransform(radius);
    SDL_SetRenderDrawColor(m_pRenderer, colour.r, colour.g, colour.b, colour.a);

    int x = int(centre.XValue);
    int y = int(centre.YValue);
    int r = int(radius);
   
    int offsetx = 0;
    int offsety = r;
    int d = r - 1;
    int error = 0;

    while (offsety >= offsetx)
    {
        error += SDL_RenderDrawLine(m_pRenderer, x - offsety, y + offsetx,
            x + offsety, y + offsetx);
        error += SDL_RenderDrawLine(m_pRenderer, x - offsetx, y + offsety,
            x + offsetx, y + offsety);
        error += SDL_RenderDrawLine(m_pRenderer, x - offsetx, y - offsety,
            x + offsetx, y - offsety);
        error += SDL_RenderDrawLine(m_pRenderer, x - offsety, y - offsetx,
            x + offsety, y - offsetx);

        if (error < 0)
        {
            break;
        }

        if (d >= 2 * offsetx)
        {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (r - offsety))
        {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else
        {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    if (error < 0)
    {
        ErrorLogger::Write("Could not fill circle in HtGraphics::FillCircle().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    else
    {
        return Result::SUCCESS;
    }
}

Result HtGraphics::DrawSegment(Segment2D segment, Colour colour)
{
    return DrawSegment(segment.GetStart(), segment.GetEnd(), colour);
}

Result HtGraphics::DrawSegment(Vector2D start, Vector2D end, Colour colour)
{
    start=HtCamera::instance.NativeTransform(start);
    end = HtCamera::instance.NativeTransform(end);
    SDL_SetRenderDrawColor(m_pRenderer, colour.r, colour.g, colour.b, colour.a);

    if (SDL_RenderDrawLine(m_pRenderer,
        int(start.XValue), int(start.YValue), int(end.XValue), int(end.YValue)) < 0)
    {
        ErrorLogger::Write("Could not draw line in HtGraphics::DrawSegment().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    else
    {
        return Result::SUCCESS;
    }
}

Result HtGraphics::DrawPoint(Vector2D point, Colour colour)
{
    point = HtCamera::instance.NativeTransform(point);
    SDL_SetRenderDrawColor(m_pRenderer, colour.r, colour.g, colour.b, colour.a);
    if (SDL_RenderDrawPoint(m_pRenderer, int(point.XValue), int(point.YValue)) < 0)
    {
        ErrorLogger::Write("Could not draw point in HtGraphics::DrawPoint().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;

    }
    return Result::SUCCESS;
}

Result HtGraphics::DrawPointList(const Vector2D points[], Colour colour, const int numPoints)
{
    // Hmm. Would be nice to do this in hardware
    SDL_Point* pList = nullptr;
    pList = new SDL_Point[numPoints];
    if (!pList)
    {
        ErrorLogger::Write("Could not draw points in HtGraphics::DrawPointList().");
        ErrorLogger::Write("numPoints invalid?");
        return Result::FAILURE;
    }
    for (int i = 0; i < numPoints; ++i)
    {
        Vector2D point = HtCamera::instance.NativeTransform(points[i]);
        pList[i].x = int(point.XValue);
        pList[i].y = int(point.YValue);
    }

    SDL_SetRenderDrawColor(m_pRenderer, colour.r, colour.g, colour.b, colour.a);
    if (SDL_RenderDrawPoints(m_pRenderer, pList, numPoints) < 0)
    {
        ErrorLogger::Write("Could not draw points in HtGraphics::DrawPointList().");
        ErrorLogger::Write(SDL_GetError());
        delete[] pList;
        return Result::FAILURE;

    }
    delete[] pList;
    return Result::SUCCESS;
}

Result HtGraphics::WriteTextCentered(Vector2D centre, std::string text, Colour colour, FontIndex font, double angle, double scale)
{
    centre = HtCamera::instance.NativeTransform(centre);
    scale = HtCamera::instance.NativeTransform(scale);

    if (text.size() > 0)
    {
        Texture texture = CreateTextureFromText(text, font, colour);
        if (texture.pTexture)
        {
            SDL_Rect targetArea;
            targetArea.x = int(centre.XValue - texture.width * scale/2);
            targetArea.y = int(centre.YValue- texture.height * scale/2);
            targetArea.h = int(texture.height * scale);
            targetArea.w = int(texture.width * scale);

            SDL_SetTextureBlendMode(texture.pTexture, SDL_BLENDMODE_BLEND);

            if (SDL_RenderCopyEx(m_pRenderer, texture.pTexture, NULL, &targetArea, angle, NULL, SDL_FLIP_NONE) < 0)
            {
                ErrorLogger::Write("Could not render text in HtGraphics::WriteTextScreen().");
                ErrorLogger::Write(SDL_GetError());
                return Result::FAILURE;
            }

            SDL_DestroyTexture(texture.pTexture);
        }
        else
        {
            ErrorLogger::Write("Failed to create texture for text in HtGraphics::WriteTextScreen().");
            return Result::FAILURE;
        }
    }
    return Result::SUCCESS;
}

Result HtGraphics::WriteTextAligned(Vector2D topLeft, std::string text, Colour colour, FontIndex font, double scale)
{
    topLeft = HtCamera::instance.NativeTransform(topLeft);
    scale = HtCamera::instance.NativeTransform(scale);

    if (text.size() > 0)
    {
        Texture texture = CreateTextureFromText(text, font, colour);
        if (texture.pTexture)
        {
            SDL_Rect targetArea;
            targetArea.x = int(topLeft.XValue);
            targetArea.y = int(topLeft.YValue);
            targetArea.h = int(texture.height * scale);
            targetArea.w = int(texture.width * scale);

            SDL_SetTextureBlendMode(texture.pTexture, SDL_BLENDMODE_BLEND);

            if (SDL_RenderCopy(m_pRenderer, texture.pTexture, NULL, &targetArea) < 0)
            {
                ErrorLogger::Write("Could not render text in HtGraphics::WriteTextScreen().");
                ErrorLogger::Write(SDL_GetError());
                return Result::FAILURE;
            }

            SDL_DestroyTexture(texture.pTexture);
        }
        else
        {
            ErrorLogger::Write("Failed to create texture for text in HtGraphics::WriteTextScreen().");
            return Result::FAILURE;
        }
    }
    return Result::SUCCESS;
}

Result HtGraphics::WriteTextAligned(int topLeftX, int topLeftY, std::string text, Colour colour, FontIndex font, double scale)
{
    return WriteTextAligned(Vector2D(topLeftX, topLeftY), text, colour, font, scale);
}

Result HtGraphics::WriteIntCentered(Vector2D centre, int number, Colour colour, FontIndex font, double angle, double scale)
{
    return WriteTextCentered(centre, std::to_string(number), colour, font, scale);
}

Result HtGraphics::WriteIntAligned(Vector2D topLeft, int number, Colour colour, FontIndex font, double scale)
{
    return WriteTextAligned(topLeft, std::to_string(number), colour, font, scale);
}

Result HtGraphics::WriteIntAligned(int topLeftX, int topLeftY, int number, Colour colour, FontIndex font, double scale)
{
    return WriteIntAligned(Vector2D(topLeftX, topLeftY), number, colour, font, scale);
}

Result HtGraphics::WriteFloatCentered(Vector2D centre, double number, Colour colour, FontIndex font, double angle, double scale)
{
    return WriteTextCentered(centre, std::to_string(number), colour, font, scale);
}

Result HtGraphics::WriteFloatAligned(Vector2D topLeft, double number, Colour colour, FontIndex font, double scale)
{
    return WriteTextAligned(topLeft, std::to_string(number), colour, font, scale);
}

Result HtGraphics::WriteFloatAligned(int topLeftX, int topLeftY, double number, Colour colour, FontIndex font, double scale)
{
    return WriteFloatAligned(Vector2D(topLeftX, topLeftY), number, colour, font, scale);
}

Result HtGraphics::Present()
{
    //Update screen
    SDL_RenderPresent(m_pRenderer);

    //Clear screen
    SDL_SetRenderDrawColor(m_pRenderer, m_backgroundColour.r, m_backgroundColour.g, m_backgroundColour.b, m_backgroundColour.a);
    if (SDL_RenderClear(m_pRenderer) < 0)
    {
        ErrorLogger::Write("Could not clear back buffer in HtGraphics::Present().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }


    if ((int)m_backGroundTexture >= 0)
    {
        std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.find(m_backGroundTexture);

        if (picit == m_pictureMap.end())
        {
            ErrorLogger::Write("Background texture not found in HtGraphics::Present().");
            m_backGroundTexture = NO_PICTURE_INDEX;
        }

        // Switch off colour key for transparency?
        if (SDL_RenderCopy(m_pRenderer, picit->second.pTexture,nullptr, nullptr) < 0)
        {
            ErrorLogger::Write("Could not render background texture in HtGraphics::Present().");
            ErrorLogger::Write(SDL_GetError());
            return Result::FAILURE;
        }
    }

    return Result::SUCCESS;
}

Result HtGraphics::Initialise()
{

    // Initialise the main window
    m_pWindow = SDL_CreateWindow(APPNAME.c_str(), // Title on the window
        SDL_WINDOWPOS_CENTERED,        // X-coordinate of screen - centre it
        SDL_WINDOWPOS_CENTERED,         // Y Coordinate of screen - centre it
        600, 480,                       // Window dimensions. Don't matter because we are using fullscreen anyway
        SDL_WINDOW_FULLSCREEN_DESKTOP);  // Flags. Useful flags are WINDOW_FULLSCREEN
                                        // WINDOW_FULLSCREEN_DESKTOP, WINDOW_BORDERLESS,
                                        //  WINDOW_SHOWN (is actually ignored, since this is default).
                                        // If you want a hidden window use WINDOW_HIDDEN

    if (m_pWindow == nullptr)
    {
        // Could not actually create a window. That's not a good start.
        ErrorLogger::Write("Could not create window in HtGraphics::Initialise(). \n");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;       // Exit with a failure
    }

    // Request the dimensions of the window
    SDL_GetWindowSize(m_pWindow, &m_windowWidth, &m_windowHeight);

    //Create renderer for window. Use hardware acceleration. 
    // The second parameter is the index of the renderer, but -1 means use the first one.
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (m_pRenderer == NULL)
    {
        ErrorLogger::Write("Could not create renderer in HtGraphics::Initialise(). \n");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }

    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);

    // Set up image loading for PNG, JPG, TIF and WEBP
    int result = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    if ((result & IMG_INIT_JPG) == 0)
    {
        ErrorLogger::Write("Could not set up JPG file loading in HtGraphics::Initialise(). \n");
    }
    if ((result & IMG_INIT_PNG) == 0)
    {
        ErrorLogger::Write("Could not set up PNG file loading in HtGraphics::Initialise(). \n");
    }

    // Sets "black" as the colour key for transparency.
    SetTransparentColour(BLACK);
    
    // Set the default background colour
    SetBackgroundColour(BLACK);


    // Load the TTF library
    if (TTF_Init() == -1)
    {
        ErrorLogger::Write("Could not intialise TTF in HtGraphics::Initialise(). \n");
        ErrorLogger::Write("Continue without font support.....\n");
        ErrorLogger::Write(SDL_GetError());
    }

    // Load some default fonts
    LoadFont(FONTS + "fallingsky.otf", 48);
    LoadFont(FONTS + "librebaskerville.ttf", 48);
    LoadFont(FONTS + "mechanical.otf", 48);
    LoadFont(FONTS + "applestorm.otf", 48);
    LoadFont(FONTS + "blink.otf", 48);

    HtCamera::instance.Reset();

    return Result::SUCCESS;
}

void HtGraphics::Shutdown()
{
    // Release all texture resources
    ReleaseAllPictures();
    // Shut down the IMG subsystem
    IMG_Quit();
    // Release all font resources
    ReleaseAllFonts();
    // Shut down the TTF subsystem
    TTF_Quit();
    // Release the renderer
    SDL_DestroyRenderer(m_pRenderer);
    // Close the window
    SDL_DestroyWindow(m_pWindow);
    m_pWindow = nullptr;
}

void HtGraphics::ReleasePicture(PictureIndex picture)
{
    // Find the picture
    std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.find(picture);

    // If the picture is actually loaded
    if (picit != m_pictureMap.end())
    {
        if (picit->second.pTexture)
        {
            // Release the texture
            SDL_DestroyTexture(picit->second.pTexture);
        }
        m_pictureMap.erase(picit);
    }

    // Also remove from filemap.
    // We need to search rather than just directly delete for various reasons,
    // including the issue that some textures don't have a filename, and there may
    // be different pictures with the same filename, if one has been modified.
    // Slow, but should not be an issue when releasing assets
    for (auto it=m_filenameMap.begin(); it != m_filenameMap.end();)
    {
        if (it->second == picture)
        {
            // Release the texture, then iterate to the new next one
            it=m_filenameMap.erase(it);
        }
        else
        {
            ++it;       // Just iterate to the next one
        }
    }

}

void HtGraphics::ReleaseAllPictures()
{	
    // Start at the beginning
    std::map<PictureIndex, Texture>::iterator picit = m_pictureMap.begin();

    // Loop through all textures
    for (; picit != m_pictureMap.end(); picit++)
    {
        if (picit->second.pTexture)
        {
            // Release the texture
            SDL_DestroyTexture(picit->second.pTexture);
        }
    }

    // Empty the two lists
    m_pictureMap.clear();
    m_filenameMap.clear();
    m_nextPictureIndex = (PictureIndex)0;
}

void HtGraphics::ReleaseAllFonts()
{
    // Start at the beginning
    std::map<FontIndex, TTF_Font*>::iterator picit = m_fontList.begin();

    // Loop through all textures
    for (; picit != m_fontList.end(); picit++)
    {
        if (picit->second)
        {
            // Release the texture
            TTF_CloseFont(picit->second);
        }
    }

    // Empty the two lists
    m_fontList.clear();
    m_nextFontIndex = 0;
}

Texture HtGraphics::CreateTextureFromText(const std::string text, FontIndex fontIndex, Colour textColour)
{
    Texture answer;
    if (m_fontList.size() <= 0)
    {
        ErrorLogger::Write("Cannot load font in HtGraphics::CreateTextureFromText().");
        ErrorLogger::Write("No fonts are loaded.");
        answer.pTexture = nullptr;
        return answer;
    }
    std::map<FontIndex, TTF_Font*>::iterator fontit = m_fontList.find(fontIndex);

    if (fontit == m_fontList.end())
    {
        ErrorLogger::Write("Cannot find font in HtGraphics::CreateTextureFromText().");
        ErrorLogger::Write("No matching font for that index. Default to first font in list.");
        fontit = m_fontList.begin();
    }

    TTF_Font* pFont = fontit->second;

    if (!pFont)
    {
        ErrorLogger::Write("Invalid font in HtGraphics::CreateTextureFromText().");
        answer.pTexture = nullptr;
        return answer;
    }

    // Create a surface from the requested text
    SDL_Surface* pTextSurface = TTF_RenderText_Shaded(pFont, text.c_str(), textColour.ToSDLColor(), m_transparentColour.ToSDLColor());
    if (pTextSurface == nullptr)
    {
        ErrorLogger::Write("Cannot render text in HtGraphics::CreateTextureFromText().");
        ErrorLogger::Write(SDL_GetError());
        answer.pTexture = nullptr;
        return answer;
    }

    answer.height = pTextSurface->h;
    answer.width = pTextSurface->w;

    SDL_SetColorKey(pTextSurface, SDL_TRUE, SDL_MapRGB(pTextSurface->format, 
        m_transparentColour.r, m_transparentColour.g, m_transparentColour.b));

    //Create texture from surface pixels
    answer.pTexture = SDL_CreateTextureFromSurface(m_pRenderer, pTextSurface);
    if (answer.pTexture == nullptr)
    {
        ErrorLogger::Write("Cannot create render surface in HtGraphics::CreateTextureFromText().");
        ErrorLogger::Write(SDL_GetError());

    }

    // Free the surface
    SDL_FreeSurface(pTextSurface);
    return answer;
    // Caution. Early returns
}

void HtGraphics::SetBackgroundColour(Colour colour)
{
    m_backgroundColour = colour;
    m_backGroundTexture = NO_PICTURE_INDEX;
}

void HtGraphics::SetBackgroundTexture(PictureIndex picture)
{
    m_backGroundTexture = picture;
}

HtGraphics::HtGraphics()
{
    m_windowHeight = 0;
    m_windowWidth = 0;
    m_nextPictureIndex=(PictureIndex)0;
    m_nextFontIndex = 0;
    m_pWindow = nullptr;
    m_pRenderer=nullptr;
    m_backgroundColour = Colour(255, 0, 0, 0);
    m_backGroundTexture = NO_PICTURE_INDEX;
}

HtGraphics::~HtGraphics()
{
    // Should be done earlier, but this might work as a backup
    Shutdown();
}

Result HtGraphics::SetRenderColour(const Colour& colour)
{
    if (colour.r >= 0 && colour.r <= 255
        && colour.g >= 0 && colour.g <= 255
        && colour.b >= 0 && colour.b <= 255
        && colour.a >= 0 && colour.a <= 255
        )
    {
        if (SDL_SetRenderDrawColor(m_pRenderer, colour.r, colour.g, colour.b, colour.a) < 0)
        {
            ErrorLogger::Write("Could not set colour in HtGraphics::SetRenderColour().");
            ErrorLogger::Write(SDL_GetError());
            return Result::FAILURE;
        }
        else
        {
            return Result::SUCCESS;
        }
    }
    return Result::FAILURE;
}


int HtGraphics::GetWindowWidth()const
{
    return m_windowWidth;

}

int HtGraphics::GetWindowHeight()const
{
    return m_windowHeight;
}

void HtGraphics::SetTransparentColour(const Colour& colour)
{
    m_transparentColour = colour;
}

Colour HtGraphics::GetTransparentColour() const
{
    return m_transparentColour;
}

FontIndex HtGraphics::LoadFont(const std::string filename, int pointSize)
{
    TTF_Font* pFont = TTF_OpenFont(filename.c_str(), pointSize);

    if (pFont)
    {
        m_fontList.insert(std::pair<FontIndex, TTF_Font*>(m_nextFontIndex, pFont));
        m_nextFontIndex++;
        return m_nextFontIndex - 1;
    }
    else
    {
        ErrorLogger::Write("Could not load font in HtGraphics::LoadFont(): "+filename);
        ErrorLogger::Write(TTF_GetError());
        return -1;
    }
}
