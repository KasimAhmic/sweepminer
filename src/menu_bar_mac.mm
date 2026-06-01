#if SWEEPMINER_PLATFORM_MACOS

#include <unordered_map>

#import <Cocoa/Cocoa.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>

#include "menu_bar.hpp"

@interface MenuHandler : NSObject
@property(nonatomic, assign) void* cppInstance;
- (void)onMenuClick:(id)sender;
@end

class MenuBar : public IMenuBar {
public:
    MenuBar(SDL_Window* window, const uint32_t menuEventId): IMenuBar(window, menuEventId) {
        SDL_PropertiesID props = SDL_GetWindowProperties(window);

        this->nsWindow = (__bridge NSWindow*)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);

        this->mainMenu = [[NSMenu alloc] init];
        [NSApp setMainMenu:this->mainMenu];

        this->handler = [[MenuHandler alloc] init];
        this->handler.cppInstance = this;
    }

    void addMenu(const int32_t id, const char* title) override {
        NSString* nsTitle = [NSString stringWithUTF8String:title];

        NSMenuItem* menu = [[NSMenuItem alloc] initWithTitle:nsTitle
                                                      action:nil
                                               keyEquivalent:@""];
        [this->mainMenu addItem:menu];

        NSMenu* subMenu = [[NSMenu alloc] initWithTitle:nsTitle];
        [menu setSubmenu:subMenu];

        this->menuIds[id] = subMenu;
    }

    void addItem(const int32_t id, int32_t parentMenuId, const char* title) override {
        auto it = this->menuIds.find(parentMenuId);
        if (it == this->menuIds.end()) {
            return;
        }

        NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithUTF8String:title]
                                                      action:@selector(onMenuClick:)
                                               keyEquivalent:@""];

        [item setTag:id];
        [item setTarget:this->handler];

        [it->second addItem:item];

        this->menuItemIds[id] = item;
    }

    void addSubMenu(const int32_t id, const int32_t parentMenuId, const char* title) override {
        auto it = this->menuIds.find(parentMenuId);
        if (it == this->menuIds.end()) {
            return;
        }

        NSString* nsTitle = [NSString stringWithUTF8String:title];

        NSMenuItem* menu = [[NSMenuItem alloc] initWithTitle:nsTitle
                                                      action:nil
                                               keyEquivalent:@""];
        [it->second addItem:menu];

        NSMenu* subMenu = [[NSMenu alloc] initWithTitle:[NSString stringWithUTF8String:title]];
        [menu setSubmenu:subMenu];

        this->menuIds[id] = subMenu;
    }

    void addSeparator(int32_t parentMenuId) override {
        auto it = this->menuIds.find(parentMenuId);
        if (it == this->menuIds.end()) {
            return;
        }

        [it->second addItem:[NSMenuItem separatorItem]];
    }

    void dispatchMenuClick(const int32_t itemId) const {
        this->handleMenuClick(itemId);
    }

private:
    NSWindow* nsWindow;
    NSMenu* mainMenu;
    MenuHandler* handler;
    std::unordered_map<int32_t, NSMenu*> menuIds;
    std::unordered_map<int32_t, NSMenuItem*> menuItemIds;
};

std::unique_ptr<IMenuBar> CreateMenuBar(SDL_Window* window, const uint32_t menuEventId) {
    return std::make_unique<MenuBar>(window, menuEventId);
}

@implementation MenuHandler

- (void)onMenuClick:(id)sender {
    auto item = static_cast<NSMenuItem*>(sender);
    NSInteger tag = [item tag];

    auto menuBar = static_cast<MenuBar*>(self.cppInstance);

    menuBar->dispatchMenuClick(static_cast<int32_t>(tag));
}

@end

#endif
