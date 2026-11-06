#include <eclipse.hpp>
#include <modules/gui/gui.hpp>
#include <modules/config/config.hpp>

namespace eclipse::api {
using namespace geode::prelude;

template <config::SupportedType T>
void createGetConfigListener() {
    events::RequestConfigValueEvent<T>::listeners().push_back([](events::RequestConfigValueEvent<T>& e) {
        auto useInternal = e.getUseInternal();
        auto res = useInternal ? config::get<T>(e.getKey()) : config::getTemp<T>(e.getKey());
        if (!res) return;
        e.setValue(res.unwrap());
        return;
    });
}

template <config::SupportedType T>
void createSetConfigListener() {
    events::SetConfigValueEvent<T>::listeners().push_back([](events::SetConfigValueEvent<T>& e) {
        if (e.getUseInternal()) {
            config::set<T>(e.getKey(), e.getValue());
        } else {
            config::setTemp<T>(e.getKey(), e.getValue());
        }
        return;
    });
}

$execute {
    events::CreateMenuTabEvent::listeners().push_back([](events::CreateMenuTabEvent& e) {
        gui::MenuTab::find(e.getName());
        return;
    });

    /* Components */
    events::AddLabelEvent::listeners().push_back([](events::AddLabelEvent& e) {
        auto tab = gui::MenuTab::find(e.getTabName());
        auto label = tab->addLabel(e.getTitle());
        e.setUniqueID(label->getUID());
        return;
    });
    events::AddToggleEvent::listeners().push_back([](events::AddToggleEvent& e) {
        auto tab = gui::MenuTab::find(e.getTabName());
        auto toggle = tab->addToggle(e.getTitle(), e.getID());
        toggle->callback([callback = std::get<0>(e.getCallbacks())](bool value) {
            std::invoke(callback, value);
        })->handleKeybinds()->disableSaving();
        e.setUniqueID(toggle->getUID());
        return;
    });
    events::AddButtonEvent::listeners().push_back([](events::AddButtonEvent& e) {
        auto tab = gui::MenuTab::find(e.getTabName());
        auto button = tab->addButton(e.getTitle());
        button->callback([callback = std::get<0>(e.getCallbacks())] {
            std::invoke(callback);
        });
        e.setUniqueID(button->getUID());
        return;
    });

    /* Component Descriptions */
    events::SetComponentDescriptionEvent::listeners().push_back([](events::SetComponentDescriptionEvent& e) {
        auto component = gui::Component::find(e.getID());
        if (!component) return;
        component->setDescription(e.getDescription());
        return;
    });

    /* Config */
    createGetConfigListener<bool>();
    createGetConfigListener<int>();
    createGetConfigListener<float>();
    createGetConfigListener<std::string>();
    createSetConfigListener<bool>();
    createSetConfigListener<int>();
    createSetConfigListener<float>();
    createSetConfigListener<std::string>();

}

}
