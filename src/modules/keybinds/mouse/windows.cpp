#include <Geode/platform/platform.hpp>
#ifdef GEODE_IS_WINDOWS
#include <modules/keybinds/manager.hpp>

namespace eclipse::keybinds {

    Keys convertMouseKey(int button) {
        switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                return Keys::MouseLeft;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return Keys::MouseRight;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return Keys::MouseMiddle;
            case GLFW_MOUSE_BUTTON_4:
                return Keys::MouseButton4;
            case GLFW_MOUSE_BUTTON_5:
                return Keys::MouseButton5;
            default:
                return Keys::None;
        }
    }

    // Mouse-button hook removed: CCEGLView::onGLFWMouseCallBack is inline on 2.2091 Windows and cannot be modified.

}

#endif