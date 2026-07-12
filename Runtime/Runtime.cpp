#include "Runtime.h"
#include "../Core/Core.h"

namespace arduino {

Runtime::Runtime() {
    // Default to the standard globally linked Arduino setup and loop functions
    m_setupFunc = []() { ::setup(); };
    m_loopFunc = []() { ::loop(); };
}

void Runtime::setApplicationFunctions(AppFunc setupFunc, AppFunc loopFunc) {
    if (setupFunc) m_setupFunc = setupFunc;
    if (loopFunc) m_loopFunc = loopFunc;
}

void Runtime::run() {
    if (m_setupFunc) {
        m_setupFunc();
    }
    
    while (true) {
        if (m_loopFunc) {
            m_loopFunc();
        }
    }
}

} // namespace arduino
