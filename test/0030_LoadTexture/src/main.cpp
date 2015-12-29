//
//  main.c
//  OSX
//
//  Created by Panutat Tejasen on 10/14/2558 BE.
//  Copyright © 2558 Jimmy Software Co.,Ltd. All rights reserved.
//

#include <iostream>
#include <string>
#include <SDL.h>

#include "GUI_utils.h"

static SDL_Texture *texture = 0;

void
render(SDL_Renderer *renderer)
{
    /* fill background in with red */
    SDL_SetRenderDrawColor(renderer, 0x64, 0x95, 0xED, 255); // Corn flower blue
    SDL_RenderClear(renderer);
    
    //Draw the texture
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    
    /* update screen */
    SDL_RenderPresent(renderer);
    

}

int
main(int argc, char *argv[])
{
    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fatalError("Could not initialize SDL");
    }
    
    
#if __IPHONEOS__ || __ANDROID__
    int landscape = 0;
    int modes = SDL_GetNumDisplayModes(0);
    int sx = 0, sy = 0;
    logOut( "Modes available: %i\n", modes );
    
    for (int i = 0; i < modes; i++)
    {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        logOut( "Mode #%i: %i, %i\n", i+1, mode.w, mode.h );
        if (landscape ? mode.w > sx : mode.h > sy)
        {
            sx = mode.w;
            sy = mode.h;
        }
    }
#else
    int sx = 800, sy = 600;
#endif
    printf("picked: %d %d\n", sx, sy);
    
    //Now create a window with title "SDL" at 0, 0 on the screen with w:800 h:600 and show it
    SDL_Window *window = SDL_CreateWindow("SDL", 0, 0, sx, sy,
                              SDL_WINDOW_OPENGL );
    if (window == NULL){
        fatalError("SDL_CreateRenderer Error");
    }
    SDL_GetWindowSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    GUI_log("given: %d %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    //Create a renderer that will draw to the window, -1 specifies that we want to load whichever
    //video driver supports the flags we're passing
    //Flags: SDL_RENDERER_ACCELERATED: We want to use hardware accelerated rendering
    //SDL_RENDERER_PRESENTVSYNC: We want the renderer's present function (update screen) to be
    //synchornized with the monitor's refresh rate
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL){
        fatalError("SDL_CreateRenderer Error");
    }
    
    /* load the bmp */
    SDL_Surface *bmp_surface;
    std::string imagePath = GUI_getResourcePath() + "icon.bmp";
    //imagePath = "data/icon.bmp";
    bmp_surface = SDL_LoadBMP(imagePath.c_str());
    if (bmp_surface == NULL) {
        fatalError("could not load bmp");
    }
    /* set white to transparent on the happyface */
    SDL_SetColorKey(bmp_surface, 1,
                    SDL_MapRGB(bmp_surface->format, 255, 255, 255));
    
    /* convert RGBA surface to texture */
    texture = SDL_CreateTextureFromSurface(renderer, bmp_surface);
    if (texture == 0) {
        fatalError("could not create texture");
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    /* free up allocated memory */
    SDL_FreeSurface(bmp_surface);
    
    /* main loop */
    Uint32 startFrame;
    Uint32 endFrame;
    int delay;
    int done = 0;
    
    while (!done) {
        startFrame = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = 1;
            }
        }
        render(renderer);
        endFrame = SDL_GetTicks();
        
        /* figure out how much time we have left, and then sleep */
        delay = MILLESECONDS_PER_FRAME - (endFrame - startFrame);
        if (delay < 0) {
            delay = 0;
        } else if (delay > MILLESECONDS_PER_FRAME) {
            delay = MILLESECONDS_PER_FRAME;
        }
        SDL_Delay(delay);
    }
    
    //Clean up our objects and quit
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    /* shutdown SDL */
    SDL_Quit();
    
    return 0;
    
}