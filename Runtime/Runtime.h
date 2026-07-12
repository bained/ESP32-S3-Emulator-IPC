#pragma once

#include <functional>

namespace arduino {

class Runtime {
public:
    using AppFunc = std::function<void()>;

    Runtime();

    // Register custom functions to bypass default setup() / loop() linked functions
    void setApplicationFunctions(AppFunc setupFunc, AppFunc loopFunc);

    // Starts the main lifecycle execution loop
    void run();

private:
    AppFunc m_setupFunc;
    AppFunc m_loopFunc;
};

} // namespace arduino
