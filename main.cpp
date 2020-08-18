#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <stdio.h>

using namespace std;

//Window dimensions
const int wWidth = 900;
const int wHeight = 400;

//Button dimensions
const int bBigHeight = 300;
const int bBigWidth = 60;
const int bSmallHeight = 182;
const int bSmallWidth = 30;
const int totalBigButtons = 15;
const int totalSmallButtons = 10;

//Button sprites
enum ButtonSprite
{
    BUTTON_BACKGROUND = 0,
    BUTTON_BIG_MOUSE_OUT = 1,
    BUTTON_SMALL_MOUSE_OUT = 2,
    BUTTON_BIG_MOUSE_DOWN = 3,
    BUTTON_SMALL_MOUSE_DOWN = 4,
    BUTTON_BIG_MOUSE_OVER = 5,
    BUTTON_SMALL_MOUSE_OVER = 6,
    BUTTON_SPRITE_TOTAL = 7
};

//Texture
class Texture
{
public:
    //Constructor
    Texture();

    //Destructor
    ~Texture();

    //Load image from specific path
    bool loadImage( string path );

    //Destroy texture;
    void destroy();

    //Render texture at specific point
    void render( int x, int y, SDL_Rect* sourceRectangle = NULL );

    //Get image dimensions
    int getWidth();
    int getHeight();

private:
    //The actual hardware texture
    SDL_Texture* hTexture;

    //Image dimensions
    int width;
    int height;

    //Transparency of the texture
    int transparency;
};

//Mouse button big
class BigButton
{
public:
    //Constructor
    BigButton();

    //Set top left position
    void setTopLeft( int x, int y );

    //Handle mouse events
    void mouseEvents( SDL_Event* e );

    //Render button sprite
    void render();

    //Prevent to press big button
    //with small button simultaneously
    void releaseWhenSmall();

private:
    //Top left position
    SDL_Point bPosition;

    //Current sprite
    ButtonSprite bCurrentSprite;
};

//Mouse button small
class SmallButton
{
public:
    //Constructor
    SmallButton();

    //Set top left position
    void setTopLeft( int x, int y );

    //Handle mouse events
    void mouseEvents( SDL_Event* e );

    //Render button sprite
    void render();

private:
    //Top left position
    SDL_Point bPosition;

    //Current sprite
    ButtonSprite bCurrentSprite;
};

//Window and renderer
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

//Mouse button sprites
SDL_Rect spriteClip[ BUTTON_SPRITE_TOTAL ];

//Sprite sheet
Texture spriteSheetTexture;

//Button objects
BigButton bigButtons[ totalBigButtons ];
SmallButton smallButtons[ totalSmallButtons ];

//Constructor
Texture::Texture()
{
    hTexture = NULL;
    width = 0;
    height = 0;
}

//Destructor
Texture::~Texture()
{
    destroy();
}

//Load image from specific path
bool Texture::loadImage( string path)
{
    //Success flag
    bool success = true;

    //Destroy the existing texture
    destroy();

    //Final texture
    SDL_Texture* fTexture = NULL;

    //Load image from a path
    SDL_Surface* loadedImage = IMG_Load( path.c_str() );
    if( !loadedImage )
    {
        printf( "IMG_Load error!\n", IMG_GetError() );
        success = false;
    }
    else
    {
        //Set white color as transparent
        SDL_SetColorKey( loadedImage, SDL_TRUE, SDL_MapRGB( loadedImage->format, 0xFF, 0xFF, 0xFF ) );

        //Create texture from loaded image
        fTexture = SDL_CreateTextureFromSurface( renderer, loadedImage );
        if( !fTexture )
        {
            printf( "SDL_CreateTextureFromSurface error!\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Get image dimensions
            width = loadedImage->w;
            height = loadedImage->h;
        }

        //Destroy the loaded surface
        SDL_FreeSurface( loadedImage );
    }

    //Assign final texture
    hTexture = fTexture;

    //Return true
    return success;
}

//Destroy texture
void Texture::destroy()
{
    //Destroy existing texture
    if( hTexture != NULL )
    {
        SDL_DestroyTexture( hTexture );
        hTexture = NULL;
        width = 0;
        height = 0;
    }
}

void Texture::render( int x, int y, SDL_Rect* sourceRectangle )
{
    //Set rendering space and render to screen
    SDL_Rect destinationRectangle = { x, y, width, height };

    //Set clip rendering dimensions
    if ( sourceRectangle != NULL )
    {
        destinationRectangle.w = sourceRectangle->w;
        destinationRectangle.h = sourceRectangle->h;

    }

    SDL_RenderCopy( renderer, hTexture, sourceRectangle, &destinationRectangle );
}

//Get image width
int Texture::getWidth()
{
    return width;
}

//Get image height
int Texture::getHeight()
{
    return height;
}

//BigButton Constructor
BigButton::BigButton()
{
    bPosition.x = 0;
    bPosition.y = 0;

    bCurrentSprite = BUTTON_BIG_MOUSE_OUT;
}

//BigButton top left position
void BigButton::setTopLeft( int x, int y )
{
    bPosition.x = x;
    bPosition.y = y;
}

//BigButton handle mouse events
void BigButton::mouseEvents( SDL_Event* e )
{
    //If mouse event happened
    if( e->type == SDL_MOUSEMOTION ||
            e->type == SDL_MOUSEBUTTONDOWN ||
            e->type == SDL_MOUSEBUTTONUP )
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );

        //Check if mouse is in button
        bool inside = true;

        //Mouse is left of the button
        if( x < bPosition.x )
        {
            inside = false;
        }
        //Mouse is right of the button
        else if( x > bPosition.x + bBigWidth )
        {
            inside = false;
        }
        //Mouse above the button
        else if( y < bPosition.y )
        {
            inside = false;
        }
        //Mouse below the button
        else if( y > bPosition.y + bBigHeight )
        {
            inside = false;
        }

        //Mouse is outside button
        if( !inside )
        {
            bCurrentSprite = BUTTON_BIG_MOUSE_OUT;
        }
        //Mouse is inside button
        else
        {
            //Set mouse over sprite
            switch( e->type )
            {
            case SDL_MOUSEMOTION:
                bCurrentSprite = BUTTON_BIG_MOUSE_OVER;
                break;

            case SDL_MOUSEBUTTONDOWN:
                bCurrentSprite = BUTTON_BIG_MOUSE_DOWN;
                break;

            case SDL_MOUSEBUTTONUP:
                bCurrentSprite = BUTTON_BIG_MOUSE_OUT;
                break;
            }
        }
    }
}

void BigButton::render()
{
    spriteSheetTexture.render( bPosition.x, bPosition.y, &spriteClip[ bCurrentSprite ] );
}

void BigButton::releaseWhenSmall()
{
    bCurrentSprite = BUTTON_BIG_MOUSE_OUT;
}

//SmallButton Constructor
SmallButton::SmallButton()
{
    bPosition.x = 0;
    bPosition.y = 0;

    bCurrentSprite = BUTTON_SMALL_MOUSE_OUT;
}

//SmallButton top left position
void SmallButton::setTopLeft( int x, int y )
{
    bPosition.x = x;
    bPosition.y = y;
}

//SmallButton handle mouse events
void SmallButton::mouseEvents( SDL_Event* e )
{
    //If mouse event happened
    if( e->type == SDL_MOUSEMOTION ||
            e->type == SDL_MOUSEBUTTONDOWN ||
            e->type == SDL_MOUSEBUTTONUP )
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );

        //Check if mouse is in button
        bool inside = true;

        //Mouse is left of the button
        if( x < bPosition.x )
        {
            inside = false;
        }
        //Mouse is right of the button
        else if( x > bPosition.x + bSmallWidth )
        {
            inside = false;
        }
        //Mouse above the button
        else if( y < bPosition.y )
        {
            inside = false;
        }
        //Mouse below the button
        else if( y > bPosition.y + bSmallHeight )
        {
            inside = false;
        }

        //Mouse is outside button
        if( !inside )
        {
            bCurrentSprite = BUTTON_SMALL_MOUSE_OUT;
        }
        //Mouse is inside button
        else
        {
            //Set mouse over sprite
            switch( e->type )
            {
            case SDL_MOUSEMOTION:
                bCurrentSprite = BUTTON_SMALL_MOUSE_OVER;
                for (int i = 0; i < totalBigButtons; i++)
                {
                    bigButtons[i].releaseWhenSmall();
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                bCurrentSprite = BUTTON_SMALL_MOUSE_DOWN;
                for (int i = 0; i < totalBigButtons; i++)
                {
                    bigButtons[i].releaseWhenSmall();
                }
                break;
                break;

            case SDL_MOUSEBUTTONUP:
                bCurrentSprite = BUTTON_SMALL_MOUSE_OUT;
                break;
            }
        }
    }
}

void SmallButton::render()
{
    spriteSheetTexture.render( bPosition.x, bPosition.y, &spriteClip[ bCurrentSprite ] );
}


int main( int argc, char* args[] )
{

    //SDL Initialize
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        printf( "SDL_Init error!\n", SDL_GetError() );
    }
    else
    {
        //Create window
        window = SDL_CreateWindow( "Keyboard", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wWidth, wHeight, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "SDL_CreateWindow error!\n", SDL_GetError() );
        }
        else
        {
            //Create renderer
            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
            if( renderer == NULL )
            {
                printf( "SDL_CreateRenderer error!\n", SDL_GetError() );
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG
                int img = IMG_INIT_PNG;
                if( !(IMG_Init( img ) && img) )
                {
                    printf( "IMG_Init error!\n", SDL_GetError() );
                }
                else
                {
                    //Load images
                    if( !spriteSheetTexture.loadImage( "images/board.png" ) )
                    {
                        printf( "Error with loading images!\n", SDL_GetError() );
                    }
                    else
                    {
                        //Set background sprite
                        spriteClip[ BUTTON_BACKGROUND ].x = 0;
                        spriteClip[ BUTTON_BACKGROUND ].y = 0;
                        spriteClip[ BUTTON_BACKGROUND ].w = wWidth;
                        spriteClip[ BUTTON_BACKGROUND ].h = wHeight;

                        //Set big button mouse out sprite
                        spriteClip[ BUTTON_BIG_MOUSE_OUT ].x = wWidth;
                        spriteClip[ BUTTON_BIG_MOUSE_OUT ].y = 0;
                        spriteClip[ BUTTON_BIG_MOUSE_OUT ].w = bBigWidth;
                        spriteClip[ BUTTON_BIG_MOUSE_OUT ].h = bBigHeight;

                        //Set small button mouse out sprite
                        spriteClip[ BUTTON_SMALL_MOUSE_OUT ].x = wWidth + bBigWidth;
                        spriteClip[ BUTTON_SMALL_MOUSE_OUT ].y = 0;
                        spriteClip[ BUTTON_SMALL_MOUSE_OUT ].w = bSmallWidth;
                        spriteClip[ BUTTON_SMALL_MOUSE_OUT ].h = bSmallHeight;

                        //Set big button mouse down sprite
                        spriteClip[ BUTTON_BIG_MOUSE_DOWN ].x = wWidth + bBigWidth + bSmallWidth;
                        spriteClip[ BUTTON_BIG_MOUSE_DOWN ].y = 0;
                        spriteClip[ BUTTON_BIG_MOUSE_DOWN ].w = bBigWidth;
                        spriteClip[ BUTTON_BIG_MOUSE_DOWN ].h = bBigHeight;

                        //Set small button mouse down sprite
                        spriteClip[ BUTTON_SMALL_MOUSE_DOWN ].x = wWidth + 2*bBigWidth + bSmallWidth;
                        spriteClip[ BUTTON_SMALL_MOUSE_DOWN ].y = 0;
                        spriteClip[ BUTTON_SMALL_MOUSE_DOWN ].w = bSmallWidth;
                        spriteClip[ BUTTON_SMALL_MOUSE_DOWN ].h = bSmallHeight;

                        //Set big button mouse over sprite
                        spriteClip[ BUTTON_BIG_MOUSE_OVER ].x = wWidth + 2*bBigWidth + 2*bSmallWidth;
                        spriteClip[ BUTTON_BIG_MOUSE_OVER ].y = 0;
                        spriteClip[ BUTTON_BIG_MOUSE_OVER ].w = bBigWidth;
                        spriteClip[ BUTTON_BIG_MOUSE_OVER ].h = bBigHeight;

                        //Set small button mouse over sprite
                        spriteClip[ BUTTON_SMALL_MOUSE_OVER ].x = wWidth + 3*bBigWidth + 2*bSmallWidth;
                        spriteClip[ BUTTON_SMALL_MOUSE_OVER ].y = 0;
                        spriteClip[ BUTTON_SMALL_MOUSE_OVER ].w = bSmallWidth;
                        spriteClip[ BUTTON_SMALL_MOUSE_OVER ].h = bSmallHeight;

                        //Set buttons
                        for ( int i = 0; i < totalBigButtons; i++ )
                        {
                            bigButtons[ i ].setTopLeft( 0+i*bBigWidth, 100 );
                        }
                        int z = 0;
                        for ( int i = 0; i < totalBigButtons-1; i++ )
                        {
                            if (i == 2 || i == 6 || i == 9 || i == 13)
                            {
                                continue;
                            }
                            smallButtons[ z ].setTopLeft( 45+i*bBigWidth, 100 );
                            z++;
                        }

                        //quit flag
                        bool quit = false;

                        //Event handler
                        SDL_Event e;

                        //Main loop
                        while( !quit )
                        {
                            //Handle events
                            while( SDL_PollEvent( &e ) != 0 )
                            {
                                //Quit request
                                if( e.type == SDL_QUIT )
                                {
                                    quit = true;
                                }

                                //Handle button events
                                for( int i = 0; i < totalBigButtons; ++i )
                                {
                                    bigButtons[ i ].mouseEvents( &e );
                                }
                                for( int i = 0; i < totalSmallButtons; ++i )
                                {
                                    smallButtons[ i ].mouseEvents( &e );
                                }

                            }

                            //Clear screen
                            SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
                            SDL_RenderClear( renderer );

                            //Render textures to screen
                            spriteSheetTexture.render( 0, 0, &spriteClip[ BUTTON_BACKGROUND ] );

                            for ( int i = 0; i < totalBigButtons; i++ )
                            {
                                bigButtons[ i ].render();
                            }
                            for ( int i = 0; i < totalSmallButtons; i++ )
                            {
                                smallButtons[ i ].render();
                            }

                            //Update screen
                            SDL_RenderPresent( renderer );
                        }
                    }

                }
            }
        }
    }

    //closing SDL
    spriteSheetTexture.destroy();
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );
    window = NULL;
    renderer = NULL;
    IMG_Quit();
    SDL_Quit();

    return 0;
}
