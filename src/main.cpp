#include <Geode/loader/Setting.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <modules/config/config.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>
#include <modules/gui/float-button.hpp>
#include <modules/gui/theming/manager.hpp>
#include <modules/debug/trace.hpp>

using namespace eclipse;

static bool s_isInitialized = false;

static void toggleMenu() {
    gui::Engine::get()->toggle();
    config::save();
    gui::ThemeManager::get()->saveTheme();
}

class $modify(EclipseButtonMLHook, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        if (s_isInitialized) return true;

        // Initialize the GUI engine.
        gui::Engine::get()->init();

        #ifdef GEODE_IS_MOBILE
        // This will create the floating button and keep it across scenes
        gui::FloatingButton::get()->setCallback([]{ toggleMenu(); });
        #endif

        // Toggle the menu when the "menu.toggle" keybind is pressed
        keybinds::Manager::get()->addListener("menu.toggle", [](bool down) {
            if (down) toggleMenu();
        });

        config::setIfEmpty("menu.toggleKey", "tab");
        hack::Hack::lateInitializeHacks();

        s_isInitialized = true;

        return true;
    }

};

class HackUpdater : public cocos2d::CCObject {
public:
    static HackUpdater* get() {
        static HackUpdater instance;
        return &instance;
    }

    void update(float dt) override {
        for (const auto& hack : hack::Hack::getHacks())
            hack->update();

        keybinds::Manager::get()->update();
    }
};

$on_mod(Loaded) {
    // Load the configuration file.
    config::load();

    // Initialize the hacks.
    hack::Hack::initializeHacks();

    // Load keybinds
    keybinds::Manager::get()->init();

    gui::ThemeManager::get();

    // Add "Interface" tab to edit theme settings
    {
        using namespace gui;
        auto tab = MenuTab::find("Interface");
        std::vector<std::string> themeNames = {};
        for (ThemeMeta theme : ThemeManager::get()->listAvailableThemes()) {
            themeNames.push_back(theme.name);
        }
        if (!themeNames.empty()) {
            auto themeCombo = tab->addCombo("Theme", "themeIndex", themeNames, 0);
            themeCombo->callback([](int value) {
                ThemeManager::get()->loadTheme(ThemeManager::get()->listAvailableThemes()[value].path);
                ThemeManager::get()->setUIScale(config::getTemp<float>("uiScale", 1.f));
            });
        }
        tab->addInputFloat("UI Scale", "uiScale", 0.75f, 2.f, "x%.3f")
            ->callback([](float value) {
                ThemeManager::get()->setUIScale(value);
            })->disableSaving();

        auto accentColor = tab->addColorComponent("Accent Color", "accent", true);
        accentColor->callback([](const Color& color) {
            ThemeManager::get()->applyAccentColor(color);
        })->disableSaving();

        auto backgroundColor = tab->addColorComponent("Background Color", "background", true);
        backgroundColor->callback([](const Color& color) {
            ThemeManager::get()->applyBackgroundColor(color);
        })->disableSaving();

        auto searchInput = tab->addInputText("Search", "search");
        searchInput->callback([](std::string input) {
            static bool hasSearched = false;

            if (input.empty())
            {
                if (hasSearched)
                {
                    for (auto& tab : Engine::get()->getTabs())
                    {
                        tab->setSearchedFor(false);

                        for (auto& component : tab->getComponents())
                            component->setSearchedFor(false);
                    }

                    hasSearched = false;
                }
            }
            else
            {
                hasSearched = true;

                for (auto& tab : Engine::get()->getTabs())
                {
                    bool hasFoundComponent = false;

                    for (auto& component : tab->getComponents())
                    {
                        if (utils::matchesStringFuzzy(component->getTitle(), input))
                        {
                            component->setSearchedFor(true);
                            hasFoundComponent = true;
                        }
                        else
                            component->setSearchedFor(false);
                    }

                    tab->setSearchedFor(hasFoundComponent);
                }
            }
        })->disableSaving();
    }

    // Schedule hack updates
    cocos2d::CCScheduler::get()->scheduleSelector(
        schedule_selector(HackUpdater::update),
        HackUpdater::get(), 0.f, false
    );
}