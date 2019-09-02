#import "ViewController.h"

#include "dungen.h"

#include "infrastructure/game.h"
#include "infrastructure/rendering.h"

@interface ViewController ()
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self->gameTime = 0.0;

    GLKView *view = (GLKView*)self.view;
    view.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
    [EAGLContext setCurrentContext:view.context];
    InitializeDunGen("_Root");
}

- (void)update {
    self->gameTime += self.timeSinceLastUpdate;
    SetTime(self->gameTime);
    
    GameTick();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    Render();
}


@end
