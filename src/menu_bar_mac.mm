#import <Cocoa/Cocoa.h>

#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>

#include "menu_bar.hpp"
#include "events.hpp"

@interface MenuHandler : NSObject
@property(nonatomic, assign) void* cppInstance;
- (void)onMenuClick:(id)sender;
@end

class MenuBar : public IMenuBar {
public:
    MenuBar(SDL_Window* window): IMenuBar(window) {
        SDL_PropertiesID props = SDL_GetWindowProperties(window);

        this->nsWindow = (__bridge NSWindow*)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);

        this->mainMenu = [[NSMenu alloc] init];
        [NSApp setMainMenu:this->mainMenu];

        this->handler = [[MenuHandler alloc] init];
        this->handler.cppInstance = this;
    }

    int32_t addMenu(const int32_t id, const char* title) override {
        NSString* nsTitle = [NSString stringWithUTF8String:title];

        NSMenuItem* menu = [[NSMenuItem alloc] initWithTitle:nsTitle
                                                      action:nil
                                               keyEquivalent:@""];
        [this->mainMenu addItem:menu];

        NSMenu* subMenu = [[NSMenu alloc] initWithTitle:nsTitle];
        [menu setSubmenu:subMenu];

        this->menuIds[id] = subMenu;
        return id;
    }

    int32_t addItem(const int32_t id, int32_t parentMenuId, const char* title) override {
        auto it = this->menuIds.find(parentMenuId);
        if (it == this->menuIds.end()) {
            return 0;
        }

        NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithUTF8String:title]
                                                      action:@selector(onMenuClick:)
                                               keyEquivalent:@""];

        [item setTag:id];
        [item setTarget:this->handler];

        [it->second addItem:item];

        this->menuItemIds[id] = item;
        return id;
    }

    int32_t addItem(const int32_t id, int32_t parentMenuId, const char* title, const char* icon) override {
        const int32_t itemId = this->addItem(id, parentMenuId, title);

        this->setItemIcon(itemId, icon);

        return itemId;
    }

    int32_t addSubMenu(const int32_t id, const int32_t parentMenuId, const char* title) override {
        auto it = this->menuIds.find(parentMenuId);
        if (it == this->menuIds.end()) {
            return 0;
        }

        NSString* nsTitle = [NSString stringWithUTF8String:title];

        NSMenuItem* menu = [[NSMenuItem alloc] initWithTitle:nsTitle
                                                      action:nil
                                               keyEquivalent:@""];
        [it->second addItem:menu];

        NSMenu* subMenu = [[NSMenu alloc] initWithTitle:[NSString stringWithUTF8String:title]];
        [menu setSubmenu:subMenu];

        this->menuIds[id] = subMenu;
        return id;
    }

    int32_t addSubMenu(const int32_t id, const int32_t parentMenuId, const char* title, const char* icon) override {
        auto it = this->menuIds.find(parentMenuId);
        if (it == this->menuIds.end()) {
            return 0;
        }

        NSString* nsTitle = [NSString stringWithUTF8String:title];

        NSMenuItem* menu = [[NSMenuItem alloc] initWithTitle:nsTitle
                                                      action:nil
                                               keyEquivalent:@""];
        [it->second addItem:menu];

        NSMenu* subMenu = [[NSMenu alloc] initWithTitle:[NSString stringWithUTF8String:title]];
        [menu setSubmenu:subMenu];

        this->menuIds[id] = subMenu;
        this->menuItemIds[id] = menu;

        this->setItemIcon(id, icon);

        return id;
    }

    void addSeparator(int32_t parentMenuId) override {
        auto it = this->menuIds.find(parentMenuId);
        if (it == this->menuIds.end()) {
            return;
        }

        [it->second addItem:[NSMenuItem separatorItem]];
    }

    void setItemIcon(int32_t id, const char* icon) override {
        auto it = this->menuItemIds.find(id);
        if (it == this->menuItemIds.end()) {
            return;
        }

        NSString* symbolName = [NSString stringWithUTF8String:icon];
        NSImage* image = [NSImage imageWithSystemSymbolName:symbolName
                                   accessibilityDescription:nil];

        [it->second setImage:image];
    }

    void handleMenuClick(int32_t itemId) override {
        SDL_Event event = Events::CreateSweepMinerEvent(Events::MENU_EVENT, itemId);
        SDL_PushEvent(&event);
    }

private:
    NSWindow* nsWindow;
    NSMenu* mainMenu;
    MenuHandler* handler;
    std::unordered_map<int32_t, NSMenu*> menuIds;
    std::unordered_map<int32_t, NSMenuItem*> menuItemIds;
};

std::unique_ptr<IMenuBar> CreateMenuBar(SDL_Window* window) {
    return std::make_unique<MenuBar>(window);
}

@implementation MenuHandler

- (void)onMenuClick:(id)sender {
    auto item = static_cast<NSMenuItem*>(sender);
    NSInteger tag = [item tag];

    auto bar = static_cast<MenuBar*>(self.cppInstance);

    bar->handleMenuClick(static_cast<int32_t>(tag));
}

@end
