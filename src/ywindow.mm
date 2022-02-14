#include "bgfx/platform.h"

#if BX_PLATFORM_IOS
#include "SDL.h"
#include "SDL_syswm.h"
#include "video/uikit/SDL_uikitview.h"

@interface SDL_openglview : SDL_uikitview

@end

@implementation SDL_openglview
+ (Class)layerClass
{
    return [CAMetalLayer class];
}
@end

void* YapreSDLGetNwh(SDL_SysWMinfo wmi, SDL_Window *window){
    SDL_openglview* view = [[SDL_openglview alloc] initWithFrame: CGRectMake(0,0,0,0) ];
    
    // This function will automatically remove the default view created by SDL and
    // and attach our newly created view
    [view setSDLWindow: window];
    
    float scale = 1.0f;
    if ([ [UIScreen mainScreen] respondsToSelector:@selector(nativeScale)]) {
        scale = [UIScreen mainScreen].nativeScale;
    } else
    {
        scale = [UIScreen mainScreen].scale;
    }
    [view setContentScaleFactor: scale];
    
    return wmi.info.uikit.window.rootViewController.view.layer;
}

#endif
