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
    view.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableMultisample = GLKViewDrawableMultisample4X;

    [EAGLContext setCurrentContext:view.context];

    Vec2i dims = {view.frame.size.width, view.frame.size.height};
   SetWindowDimensions(dims);
    dims.x *= view.contentScaleFactor;
    dims.y *= view.contentScaleFactor;
    SetFramebufferDimensions(dims);

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

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // only deal with the first touch
    UITouch* t = touches.allObjects.firstObject;
    CGPoint loc = [t locationInView:self.view];
    TouchCallback(loc.x, loc.y, true);
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    UITouch* t = touches.allObjects.firstObject;
    CGPoint loc = [t locationInView:self.view];
    TouchCallback(loc.x, loc.y, false);
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    UITouch* t = touches.allObjects.firstObject;
    CGPoint loc = [t locationInView:self.view];
    TouchMoveCallback(loc.x, loc.y);
}

@end
