#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_mixer.h>
#include <windows.h>

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

    //Set blending
    void blending( SDL_BlendMode b );

    //Set transparency
    void transparency( Uint8 a );

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

    //Set current sprite when key pressed
    void keyPressed( bool z );

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

    //Set current sprite when key pressed
    void keyPressed( bool z );

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

//Button sprites
SDL_Rect spriteClip[ BUTTON_SPRITE_TOTAL ];

//Sprite sheet
Texture frontLayerTexture;
Texture backgroundTexture;

//Button objects
BigButton bigButtons[ totalBigButtons ];
SmallButton smallButtons[ totalSmallButtons ];

//Sounds
Mix_Chunk *c4 = NULL;
Mix_Chunk *d4 = NULL;
Mix_Chunk *e4 = NULL;
Mix_Chunk *f4 = NULL;
Mix_Chunk *g4 = NULL;
Mix_Chunk *a4 = NULL;
Mix_Chunk *h4 = NULL;
Mix_Chunk *c5 = NULL;
Mix_Chunk *d5 = NULL;
Mix_Chunk *e5 = NULL;
Mix_Chunk *f5 = NULL;
Mix_Chunk *g5 = NULL;
Mix_Chunk *a5 = NULL;
Mix_Chunk *h5 = NULL;
Mix_Chunk *c6 = NULL;
Mix_Chunk *cis4 = NULL;
Mix_Chunk *dis4 = NULL;
Mix_Chunk *fis4 = NULL;
Mix_Chunk *gis4 = NULL;
Mix_Chunk *ais4 = NULL;
Mix_Chunk *cis5 = NULL;
Mix_Chunk *dis5 = NULL;
Mix_Chunk *fis5 = NULL;
Mix_Chunk *gis5 = NULL;
Mix_Chunk *ais5 = NULL;


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
//Set blending
void Texture::blending( SDL_BlendMode b )
{
    SDL_SetTextureBlendMode( hTexture, b );
}

//Set transparency
void Texture::transparency( Uint8 a )
{
    SDL_SetTextureAlphaMod( hTexture, a );
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

//BigButton handle events
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

void BigButton::keyPressed( bool z )
{
    if ( z == true )
    {
        bCurrentSprite = BUTTON_BIG_MOUSE_DOWN;
    }
    else if ( z == false )
    {
        bCurrentSprite = BUTTON_BIG_MOUSE_OUT;
    }
}

void BigButton::render()
{
    frontLayerTexture.render( bPosition.x, bPosition.y, &spriteClip[ bCurrentSprite ] );
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

            case SDL_MOUSEBUTTONUP:
                bCurrentSprite = BUTTON_SMALL_MOUSE_OUT;
                break;
            }
        }
    }
}

void SmallButton::keyPressed( bool z )
{
    if ( z == true )
    {
        bCurrentSprite = BUTTON_SMALL_MOUSE_DOWN;
    }
    else if ( z == false )
    {
        bCurrentSprite = BUTTON_SMALL_MOUSE_OUT;
    }
}

void SmallButton::render()
{
    frontLayerTexture.render( bPosition.x, bPosition.y, &spriteClip[ bCurrentSprite ] );
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
                //Initialize SDL_mixer
                else if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer error!\n", Mix_GetError() );
                }
                else
                {
                    //Allocate channels
                    Mix_AllocateChannels( totalBigButtons + totalSmallButtons + 1);

                    //Load sound files
                    c4 = Mix_LoadWAV( "sound/c4.wav" );
                    d4 = Mix_LoadWAV( "sound/d4.wav" );
                    e4 = Mix_LoadWAV( "sound/e4.wav" );
                    f4 = Mix_LoadWAV( "sound/f4.wav" );
                    g4 = Mix_LoadWAV( "sound/g4.wav" );
                    a4 = Mix_LoadWAV( "sound/a4.wav" );
                    h4 = Mix_LoadWAV( "sound/h4.wav" );
                    c5 = Mix_LoadWAV( "sound/c5.wav" );
                    d5 = Mix_LoadWAV( "sound/d5.wav" );
                    e5 = Mix_LoadWAV( "sound/e5.wav" );
                    f5 = Mix_LoadWAV( "sound/f5.wav" );
                    g5 = Mix_LoadWAV( "sound/g5.wav" );
                    a5 = Mix_LoadWAV( "sound/a5.wav" );
                    h5 = Mix_LoadWAV( "sound/h5.wav" );
                    c6 = Mix_LoadWAV( "sound/c6.wav" );
                    cis4 = Mix_LoadWAV( "sound/cis4.wav" );
                    dis4 = Mix_LoadWAV( "sound/dis4.wav" );
                    fis4 = Mix_LoadWAV( "sound/fis4.wav" );
                    gis4 = Mix_LoadWAV( "sound/gis4.wav" );
                    ais4 = Mix_LoadWAV( "sound/ais4.wav" );
                    cis5 = Mix_LoadWAV( "sound/cis5.wav" );
                    dis5 = Mix_LoadWAV( "sound/dis5.wav" );
                    fis5 = Mix_LoadWAV( "sound/fis5.wav" );
                    gis5 = Mix_LoadWAV( "sound/gis5.wav" );
                    ais5 = Mix_LoadWAV( "sound/ais5.wav" );
                    if ( c4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( d4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( e4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( f4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( g4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( a4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( h4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( c5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( d5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( e5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( f5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( g5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( a5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( h5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( c6 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( cis4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( dis4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( fis4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( gis4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( ais4 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( cis5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( dis5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( fis5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( gis5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }
                    else if ( ais5 == NULL )
                    {
                        printf( "Mix_LoadWAV error!\n", Mix_GetError() );
                    }

                    //Load images
                    else if( !frontLayerTexture.loadImage( "images/board.png" ) )
                    {
                        printf( "Error with loading images!\n", SDL_GetError() );
                    }
                    else if( !backgroundTexture.loadImage( "images/board.png" ) )
                    {
                        printf( "Error with loading images!\n", SDL_GetError() );
                    }
                    else
                    {
                        //Set standard blending
                        frontLayerTexture.blending( SDL_BLENDMODE_BLEND );

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

                                //Handle button sprite mouse events
                                for( int i = 0; i < totalBigButtons; ++i )
                                {
                                    bigButtons[ i ].mouseEvents( &e );
                                }
                                for( int i = 0; i < totalSmallButtons; ++i )
                                {
                                    smallButtons[ i ].mouseEvents( &e );
                                }

                                //Handle remaining mouse events
                                if( e.type == SDL_MOUSEBUTTONDOWN
                                        || e.type == SDL_MOUSEBUTTONUP )
                                {
                                    int x, y;
                                    SDL_GetMouseState( &x, &y );


                                    //Transparency
                                    if( y < 100 )
                                    {
                                        if( e.type == SDL_MOUSEBUTTONDOWN )
                                        {
                                            frontLayerTexture.transparency( 0 );
                                        }
                                    }
                                    //Button sounds
                                    else
                                    {
                                        //Small buttons
                                        if( y <= 100 + bSmallHeight )
                                        {
                                            if( x >= 45+0*bBigWidth && x <= 45+0*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, cis4, -1 );
                                                }
                                            }
                                            if( x >= 45+1*bBigWidth && x <= 45+1*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, dis4, -1 );
                                                }
                                            }
                                            if( x >= 45+3*bBigWidth && x <= 45+3*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, fis4, -1 );
                                                }
                                            }
                                            if( x >= 45+4*bBigWidth && x <= 45+4*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, gis4, -1 );
                                                }
                                            }
                                            if( x >= 45+5*bBigWidth && x <= 45+5*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, ais4, -1 );
                                                }
                                            }
                                            if( x >= 45+7*bBigWidth && x <= 45+7*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, cis5, -1 );
                                                }
                                            }
                                            if( x >= 45+8*bBigWidth && x <= 45+8*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, dis5, -1 );
                                                }
                                            }
                                            if( x >= 45+10*bBigWidth && x <= 45+10*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, fis5, -1 );
                                                }
                                            }
                                            if( x >= 45+11*bBigWidth && x <= 45+11*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, gis5, -1 );
                                                }
                                            }
                                            if( x >= 45+12*bBigWidth && x <= 45+12*bBigWidth+bSmallWidth)
                                            {
                                                if ( Mix_Playing( 25 ) == 0 )
                                                {
                                                    Mix_PlayChannel( 25, ais5, -1 );
                                                }
                                            }
                                        }

                                        //Big buttons
                                        if( x >= 0*bBigWidth && x < 1*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, c4, -1 );
                                            }
                                        }
                                        if( x >= 1*bBigWidth && x < 2*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, d4, -1 );
                                            }
                                        }
                                        if( x >= 2*bBigWidth && x < 3*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, e4, -1 );
                                            }
                                        }
                                        if( x >= 3*bBigWidth && x < 4*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, f4, -1 );
                                            }
                                        }
                                        if( x >= 4*bBigWidth && x < 5*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, g4, -1 );
                                            }
                                        }
                                        if( x >= 5*bBigWidth && x < 6*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, a4, -1 );
                                            }
                                        }
                                        if( x >= 6*bBigWidth && x < 7*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, h4, -1 );
                                            }
                                        }
                                        if( x >= 7*bBigWidth && x < 8*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, c5, -1 );
                                            }
                                        }
                                        if( x >= 8*bBigWidth && x < 9*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, d5, -1 );
                                            }
                                        }
                                        if( x >= 9*bBigWidth && x < 10*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, e5, -1 );
                                            }
                                        }
                                        if( x >= 10*bBigWidth && x < 11*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, f5, -1 );
                                            }
                                        }
                                        if( x >= 11*bBigWidth && x < 12*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, g5, -1 );
                                            }
                                        }
                                        if( x >= 12*bBigWidth && x < 13*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, a5, -1 );
                                            }
                                        }
                                        if( x >= 13*bBigWidth && x < 14*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, h5, -1 );
                                            }
                                        }
                                        if( x >= 14*bBigWidth && x < 15*bBigWidth )
                                        {
                                            if ( Mix_Playing( 25 ) == 0 )
                                            {
                                                Mix_PlayChannel( 25, c6, -1 );
                                            }
                                        }
                                    }

                                    if ( e.type == SDL_MOUSEBUTTONUP )
                                    {
                                        frontLayerTexture.transparency( 255 );
                                        Mix_ExpireChannel( 25, 50 );
                                    }
                                }

                                //Handle keyboard events
                                bool keyDown = NULL;
                                if( e.type == SDL_KEYDOWN)
                                {
                                    keyDown = true;

                                    switch( e.key.keysym.sym )
                                    {
                                    case SDLK_1:
                                        frontLayerTexture.transparency( 0 );
                                        break;

                                    case SDLK_z:
                                        bigButtons[0].keyPressed( keyDown );
                                        if( Mix_Playing( 0 ) == 0 )
                                        {
                                            Mix_PlayChannel( 0, c4, -1 );
                                        }
                                        break;

                                    case SDLK_x:
                                        bigButtons[1].keyPressed( keyDown );
                                        if( Mix_Playing( 1 ) == 0 )
                                        {
                                            Mix_PlayChannel( 1, d4, -1 );
                                        }
                                        break;

                                    case SDLK_c:
                                        bigButtons[2].keyPressed( keyDown );
                                        if( Mix_Playing( 2 ) == 0 )
                                        {
                                            Mix_PlayChannel( 2, e4, -1 );
                                        }
                                        break;

                                    case SDLK_v:
                                        bigButtons[3].keyPressed( keyDown );
                                        if( Mix_Playing( 3 ) == 0 )
                                        {
                                            Mix_PlayChannel( 3, f4, -1 );
                                        }
                                        break;

                                    case SDLK_b:
                                        bigButtons[4].keyPressed( keyDown );
                                        if( Mix_Playing( 4 ) == 0 )
                                        {
                                            Mix_PlayChannel( 4, g4, -1 );
                                        }
                                        break;

                                    case SDLK_n:
                                        bigButtons[5].keyPressed( keyDown );
                                        if( Mix_Playing( 5 ) == 0 )
                                        {
                                            Mix_PlayChannel( 5, a4, -1 );
                                        }
                                        break;

                                    case SDLK_m:
                                        bigButtons[6].keyPressed( keyDown );
                                        if( Mix_Playing( 6 ) == 0 )
                                        {
                                            Mix_PlayChannel( 6, h4, -1 );
                                        }
                                        break;

                                    case SDLK_t:
                                        bigButtons[7].keyPressed( keyDown );
                                        if( Mix_Playing( 7 ) == 0 )
                                        {
                                            Mix_PlayChannel( 7, c5, -1 );
                                        }
                                        break;

                                    case SDLK_y:
                                        bigButtons[8].keyPressed( keyDown );
                                        if( Mix_Playing( 8 ) == 0 )
                                        {
                                            Mix_PlayChannel( 8, d5, -1 );
                                        }
                                        break;

                                    case SDLK_u:
                                        bigButtons[9].keyPressed( keyDown );
                                        if( Mix_Playing( 9 ) == 0 )
                                        {
                                            Mix_PlayChannel( 9, e5, -1 );
                                        }
                                        break;

                                    case SDLK_i:
                                        bigButtons[10].keyPressed( keyDown );
                                        if( Mix_Playing( 10 ) == 0 )
                                        {
                                            Mix_PlayChannel( 10, f5, -1 );
                                        }
                                        break;

                                    case SDLK_o:
                                        bigButtons[11].keyPressed( keyDown );
                                        if( Mix_Playing( 11 ) == 0 )
                                        {
                                            Mix_PlayChannel( 11, g5, -1 );
                                        }
                                        break;

                                    case SDLK_p:
                                        bigButtons[12].keyPressed( keyDown );
                                        if( Mix_Playing( 12 ) == 0 )
                                        {
                                            Mix_PlayChannel( 12, a5, -1 );
                                        }
                                        break;

                                    case SDLK_LEFTBRACKET:
                                        bigButtons[13].keyPressed( keyDown );
                                        if( Mix_Playing( 13 ) == 0 )
                                        {
                                            Mix_PlayChannel( 13, h5, -1 );
                                        }
                                        break;

                                    case SDLK_RIGHTBRACKET:
                                        bigButtons[14].keyPressed( keyDown );
                                        if( Mix_Playing( 14 ) == 0 )
                                        {
                                            Mix_PlayChannel( 14, c6, -1 );
                                        }
                                        break;

                                    case SDLK_s:
                                        smallButtons[0].keyPressed( keyDown );
                                        if( Mix_Playing( 15 ) == 0 )
                                        {
                                            Mix_PlayChannel( 15, cis4, -1 );
                                        }
                                        break;

                                    case SDLK_d:
                                        smallButtons[1].keyPressed( keyDown );
                                        if( Mix_Playing( 16 ) == 0 )
                                        {
                                            Mix_PlayChannel( 16, dis4, -1 );
                                        }
                                        break;

                                    case SDLK_g:
                                        smallButtons[2].keyPressed( keyDown );
                                        if( Mix_Playing( 17 ) == 0 )
                                        {
                                            Mix_PlayChannel( 17, fis4, -1 );
                                        }
                                        break;

                                    case SDLK_h:
                                        smallButtons[3].keyPressed( keyDown );
                                        if( Mix_Playing( 18 ) == 0 )
                                        {
                                            Mix_PlayChannel( 18, gis4, -1 );
                                        }
                                        break;

                                    case SDLK_j:
                                        smallButtons[4].keyPressed( keyDown );
                                        if( Mix_Playing( 19 ) == 0 )
                                        {
                                            Mix_PlayChannel( 19, ais4, -1 );
                                        }
                                        break;

                                    case SDLK_6:
                                        smallButtons[5].keyPressed( keyDown );
                                        if( Mix_Playing( 20 ) == 0 )
                                        {
                                            Mix_PlayChannel( 20, cis5, -1 );
                                        }
                                        break;

                                    case SDLK_7:
                                        smallButtons[6].keyPressed( keyDown );
                                        if( Mix_Playing( 21 ) == 0 )
                                        {
                                            Mix_PlayChannel( 21, dis5, -1 );
                                        }
                                        break;

                                    case SDLK_9:
                                        smallButtons[7].keyPressed( keyDown );
                                        if( Mix_Playing( 22 ) == 0 )
                                        {
                                            Mix_PlayChannel( 22, fis5, -1 );
                                        }
                                        break;

                                    case SDLK_0:
                                        smallButtons[8].keyPressed( keyDown );
                                        if( Mix_Playing( 23 ) == 0 )
                                        {
                                            Mix_PlayChannel( 23, gis5, -1 );
                                        }
                                        break;

                                    case SDLK_MINUS:
                                        smallButtons[9].keyPressed( keyDown );
                                        if( Mix_Playing( 24 ) == 0 )
                                        {
                                            Mix_PlayChannel( 24, ais5, -1 );
                                        }
                                        break;
                                    }
                                }

                                if( e.type == SDL_KEYUP)
                                {
                                    keyDown = false;

                                    switch( e.key.keysym.sym )
                                    {
                                    case SDLK_1:
                                        frontLayerTexture.transparency( 255 );
                                        break;

                                    case SDLK_z:
                                        bigButtons[0].keyPressed( keyDown );
                                        Mix_ExpireChannel( 0, 50 );
                                        break;

                                    case SDLK_x:
                                        bigButtons[1].keyPressed( keyDown );
                                        Mix_ExpireChannel( 1, 50 );
                                        break;

                                    case SDLK_c:
                                        bigButtons[2].keyPressed( keyDown );
                                        Mix_ExpireChannel( 2, 50 );
                                        break;

                                    case SDLK_v:
                                        bigButtons[3].keyPressed( keyDown );
                                        Mix_ExpireChannel( 3, 50 );
                                        break;

                                    case SDLK_b:
                                        bigButtons[4].keyPressed( keyDown );
                                        Mix_ExpireChannel( 4, 50 );
                                        break;

                                    case SDLK_n:
                                        bigButtons[5].keyPressed( keyDown );
                                        Mix_ExpireChannel( 5, 50 );
                                        break;

                                    case SDLK_m:
                                        bigButtons[6].keyPressed( keyDown );
                                        Mix_ExpireChannel( 6, 50 );
                                        break;

                                    case SDLK_t:
                                        bigButtons[7].keyPressed( keyDown );
                                        Mix_ExpireChannel( 7, 50 );
                                        break;

                                    case SDLK_y:
                                        bigButtons[8].keyPressed( keyDown );
                                        Mix_ExpireChannel( 8, 50 );
                                        break;

                                    case SDLK_u:
                                        bigButtons[9].keyPressed( keyDown );
                                        Mix_ExpireChannel( 9, 50 );
                                        break;

                                    case SDLK_i:
                                        bigButtons[10].keyPressed( keyDown );
                                        Mix_ExpireChannel( 10, 50 );
                                        break;

                                    case SDLK_o:
                                        bigButtons[11].keyPressed( keyDown );
                                        Mix_ExpireChannel( 11, 50 );
                                        break;

                                    case SDLK_p:
                                        bigButtons[12].keyPressed( keyDown );
                                        Mix_ExpireChannel( 12, 50 );
                                        break;

                                    case SDLK_LEFTBRACKET:
                                        bigButtons[13].keyPressed( keyDown );
                                        Mix_ExpireChannel( 13, 50 );
                                        break;

                                    case SDLK_RIGHTBRACKET:
                                        bigButtons[14].keyPressed( keyDown );
                                        Mix_ExpireChannel( 14, 50 );
                                        break;

                                    case SDLK_s:
                                        smallButtons[0].keyPressed( keyDown );
                                        Mix_ExpireChannel( 15, 50 );
                                        break;

                                    case SDLK_d:
                                        smallButtons[1].keyPressed( keyDown );
                                        Mix_ExpireChannel( 16, 50 );
                                        break;

                                    case SDLK_g:
                                        smallButtons[2].keyPressed( keyDown );
                                        Mix_ExpireChannel( 17, 50 );
                                        break;

                                    case SDLK_h:
                                        smallButtons[3].keyPressed( keyDown );
                                        Mix_ExpireChannel( 18, 50 );
                                        break;

                                    case SDLK_j:
                                        smallButtons[4].keyPressed( keyDown );
                                        Mix_ExpireChannel( 19, 50 );
                                        break;

                                    case SDLK_6:
                                        smallButtons[5].keyPressed( keyDown );
                                        Mix_ExpireChannel( 20, 50 );
                                        break;

                                    case SDLK_7:
                                        smallButtons[6].keyPressed( keyDown );
                                        Mix_ExpireChannel( 21, 50 );
                                        break;

                                    case SDLK_9:
                                        smallButtons[7].keyPressed( keyDown );
                                        Mix_ExpireChannel( 22, 50 );
                                        break;

                                    case SDLK_0:
                                        smallButtons[8].keyPressed( keyDown );
                                        Mix_ExpireChannel( 23, 50 );
                                        break;

                                    case SDLK_MINUS:
                                        smallButtons[9].keyPressed( keyDown );
                                        Mix_ExpireChannel( 24, 50 );
                                        break;
                                    }
                                }

                            }

                            //Clear screen
                            SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
                            SDL_RenderClear( renderer );

                            //Render textures to screen
                            backgroundTexture.render( 0, 0, &spriteClip[ BUTTON_BACKGROUND ] );

                            for( int i = 0; i < totalBigButtons; i++ )
                            {
                                bigButtons[ i ].render();
                            }
                            for( int i = 0; i < totalSmallButtons; i++ )
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
    frontLayerTexture.destroy();
    backgroundTexture.destroy();
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );
    Mix_FreeChunk( c4 );
    Mix_FreeChunk( d4 );
    Mix_FreeChunk( e4 );
    Mix_FreeChunk( f4 );
    Mix_FreeChunk( g4 );
    Mix_FreeChunk( a4 );
    Mix_FreeChunk( h4 );
    Mix_FreeChunk( c5 );
    Mix_FreeChunk( d5 );
    Mix_FreeChunk( e5 );
    Mix_FreeChunk( f5 );
    Mix_FreeChunk( g5 );
    Mix_FreeChunk( a5 );
    Mix_FreeChunk( h5 );
    Mix_FreeChunk( c6 );
    Mix_FreeChunk( cis4 );
    Mix_FreeChunk( dis4 );
    Mix_FreeChunk( fis4 );
    Mix_FreeChunk( gis4 );
    Mix_FreeChunk( ais4 );
    Mix_FreeChunk( cis5 );
    Mix_FreeChunk( dis5 );
    Mix_FreeChunk( fis5 );
    Mix_FreeChunk( gis5 );
    Mix_FreeChunk( ais5 );
    c4 = NULL;
    d4 = NULL;
    e4 = NULL;
    f4 = NULL;
    g4 = NULL;
    a4 = NULL;
    h4 = NULL;
    c5 = NULL;
    d5 = NULL;
    e5 = NULL;
    f5 = NULL;
    g5 = NULL;
    a5 = NULL;
    h5 = NULL;
    c6 = NULL;
    cis4 = NULL;
    dis4 = NULL;
    fis4 = NULL;
    gis4 = NULL;
    ais4 = NULL;
    cis5 = NULL;
    dis5 = NULL;
    fis5 = NULL;
    gis5 = NULL;
    ais5 = NULL;
    window = NULL;
    renderer = NULL;
    IMG_Quit();
    SDL_Quit();
    Mix_Quit();

    return 0;
}
