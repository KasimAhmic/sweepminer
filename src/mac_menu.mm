#import <Cocoa/Cocoa.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>

void InitMenu(SDL_Window* sdlWindow) {
    @autoreleasepool {
        // Get NSWindow from SDL3
        SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
        NSWindow* nsWindow = (__bridge NSWindow*)SDL_GetPointerProperty(
            props,
            SDL_PROP_WINDOW_COCOA_WINDOW_POINTER,
            nullptr
        );

        // Main menu bar
        NSMenu* menubar = [[NSMenu alloc] init];
        [NSApp setMainMenu:menubar];

        {
                    // App menu (the one named after your app)
                    NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
                    [menubar addItem:appMenuItem];

                    NSMenu* appMenu = [[NSMenu alloc] initWithTitle:@"App"];

                    NSString* appName = [[NSProcessInfo processInfo] processName];

                    // About
                    NSMenuItem* aboutItem =
                        [[NSMenuItem alloc] initWithTitle:[@"About " stringByAppendingString:appName]
                                                   action:@selector(orderFrontStandardAboutPanel:)
                                            keyEquivalent:@""];
                    [appMenu addItem:aboutItem];

                    [appMenu addItem:[NSMenuItem separatorItem]];

                    // Hide
                    NSMenuItem* hideItem =
                        [[NSMenuItem alloc] initWithTitle:[@"Hide " stringByAppendingString:appName]
                                                   action:@selector(hide:)
                                            keyEquivalent:@"h"];
                    [appMenu addItem:hideItem];

                    // Quit
                    NSMenuItem* quitItem =
                        [[NSMenuItem alloc] initWithTitle:[@"Quit " stringByAppendingString:appName]
                                                   action:@selector(terminate:)
                                            keyEquivalent:@"q"];
                    [appMenu addItem:quitItem];

                    [appMenuItem setSubmenu:appMenu];
        }

        NSMenuItem* gameMenuItem = [[NSMenuItem alloc] init];
        [menubar addItem:gameMenuItem];

        NSMenu* gameMenu = [[NSMenu alloc] initWithTitle:@"Game"];

        NSMenuItem* newGameMenuItem =
            [[NSMenuItem alloc] initWithTitle:@"New"
                                        action:@selector(performMiniaturize:)
                                  keyEquivalent:@"F2"];

        NSMenuItem* newBeginnerGameMenuItem =
            [[NSMenuItem alloc] initWithTitle:@"Beginner"
                                        action:@selector(performMiniaturize:)
                                  keyEquivalent:@"F2"];

        NSMenuItem* newIntermediateGameMenuItem =
            [[NSMenuItem alloc] initWithTitle:@"Intermediate"
                                        action:@selector(performMiniaturize:)
                                  keyEquivalent:@""];

        NSMenuItem* newExpertGameMenuItem =
            [[NSMenuItem alloc] initWithTitle:@"Expert"
                                        action:@selector(performMiniaturize:)
                                  keyEquivalent:@""];

        NSMenuItem* newCustomGameMenuItem =
            [[NSMenuItem alloc] initWithTitle:@"Custom..."
                                        action:@selector(performMiniaturize:)
                                  keyEquivalent:@""];

        [gameMenu addItem:newGameMenuItem];

        [gameMenu addItem:[NSMenuItem separatorItem]];

        [gameMenu addItem:newBeginnerGameMenuItem];
        [gameMenu addItem:newIntermediateGameMenuItem];
        [gameMenu addItem:newExpertGameMenuItem];
        [gameMenu addItem:newCustomGameMenuItem];

        [gameMenu addItem:[NSMenuItem separatorItem]];

        [gameMenuItem setSubmenu:gameMenu];
    }
}
