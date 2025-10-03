#pragma once

template <class T, class R>
struct VisitorBase {
    virtual ~VisitorBase()                                     = default;
    [[nodiscard]] virtual auto visit(const T& expr) const -> R = 0;
};
