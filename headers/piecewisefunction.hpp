#pragma once

#include "sequence.hpp"
#include "exceptions.h"
#include "arraysequence.hpp"
#include "listsequence.hpp"
#include <cstddef>
#include "complex.hpp"
#include "fragment.hpp"
#include <memory>
#include <string>
#include <sstream>

using std::abs;

template <typename T, template <typename> class Container = MutableArraySequence>
class PieceWiseFunction {
private:
    static_assert(
        !is_correct_sequence<Container>::value,
        "This class is not allowed as container for PieceWiseFunction"
    );

    static_assert(
        std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, Complex>,
        "PieceWiseFunction supports only int, double, and Complex types"
    );
    
    std::unique_ptr<Sequence<Fragment<T>>> fragments;

public:
    PieceWiseFunction();

    void FurtherSpecification(const Fragment<T>& frag);
    bool IsMonotonic() const;
    bool IsContinuous() const;
    Fragment<T> GetFragment(size_t index) const;
    size_t CountFragments() const;

    auto Estimation(double point) const;

    std::string toString() const;
};

template <typename T, template <typename> class Container>
PieceWiseFunction<T, Container>::PieceWiseFunction() 
    : fragments(std::make_unique<Container<Fragment<T>>>()) {}

template <typename T, template <typename> class Container>
void PieceWiseFunction<T, Container>::FurtherSpecification(const Fragment<T>& frag) {
    if (!fragments) throw LogicErrorException("Контейнер не инициализирован");
    std::unique_ptr<Sequence<Fragment<T>>> newSeq(fragments->EmptyClone());
    bool inserted = false;
    for (const auto& item : *fragments) {
        if (frag.isIntersecting(item) && !inserted) {
            if (frag.IsInteriorPoint(item.GetBegin())) {
                if (frag.IsInteriorPoint(item.GetEnd())) {
                    continue;
                } else {
                    Fragment<T> newFrag = item;
                    newFrag.Pruning(frag.GetEnd(), item.GetEnd());
                    newSeq->Append(frag);
                    newSeq->Append(newFrag);
                    inserted = true;
                }
            } else if (frag.IsInteriorPoint(item.GetEnd())) {
                Fragment<T> newFrag = item;
                newFrag.Pruning(item.GetBegin(), frag.GetBegin());
                newSeq->Append(newFrag);
            } else {
                Fragment<T> newFrag1 = item;
                Fragment<T> newFrag2 = item;
                newFrag1.Pruning(item.GetBegin(), frag.GetBegin());
                newFrag2.Pruning(frag.GetEnd(), item.GetEnd());
                newSeq->Append(newFrag1);
                newSeq->Append(frag);
                newSeq->Append(newFrag2);
                inserted = true;
            }
        } else if (frag.GetBegin() < item.GetBegin() && !inserted) {
            newSeq->Append(frag);
            newSeq->Append(item);
            inserted = true;
        } else {
            newSeq->Append(item);
        }
    }
    if(!inserted) newSeq->Append(frag);

    fragments = std::move(newSeq);
}

template <typename T, template <typename> class Container>
bool PieceWiseFunction<T, Container>::IsMonotonic() const {
    if constexpr (std::is_same_v<T, Complex>) {
        throw LogicErrorException("IsMonotonic не определён для типа Complex");
    } else {
        if (fragments->GetLength() == 0) return true;
        bool increasing = false;
        bool directionSet = false;
        constexpr int NUM_CHECKS = 100;
        for (const auto& current : *fragments) {
            double step = (current.GetEnd() - current.GetBegin()) / (NUM_CHECKS - 1);
            for (int i = 0; i < NUM_CHECKS; ++i) {
                double x = current.GetBegin() + step * i;
                double der = current.DerivativeAt(x);
                if (der < 0) {
                    if (directionSet && increasing) {
                        return false;
                    } else if (!directionSet) {
                        directionSet = true;
                        increasing = false;
                    }
                } else if (der > 0) {
                    if (directionSet && !increasing) {
                        return false;
                    } else if (!directionSet) {
                        directionSet = true;
                        increasing = true;
                    }
                } 
            }
        }
        Fragment<T> prev = fragments->Get(0);
        auto it = fragments->begin();
        ++it;
        for (; it != fragments->end(); ++it) {
            Fragment<T> current = *it;
            auto begVal = current.Estimation(current.GetBegin());
            auto endVal = prev.Estimation(prev.GetEnd());
            if ((begVal > endVal && !increasing
                    || begVal < endVal && increasing)
                        && directionSet) {
                return false;
            } else if (begVal > endVal) {
                increasing = true;
                directionSet = true;
            } else if (begVal < endVal) {
                increasing = false;
                directionSet = true;
            }
            prev = *it;
        }
        return true;
    }
}

template <typename T, template <typename> class Container>
bool PieceWiseFunction<T, Container>::IsContinuous() const {
    if (fragments->GetLength() == 0 || fragments->GetLength() == 1)
        return true;
    constexpr double EPS = 1e-9;
    size_t n = fragments->GetLength();
    Fragment<T> prev = fragments->Get(0);
    auto it = fragments->begin();
    ++it;
    for (;it != fragments->end(); ++it) {
        Fragment<T> current = *it;
        double leftX = prev.GetEnd();
        double rightX = current.GetBegin();
        if (abs(leftX - rightX) > EPS) {
            return false;
        }
        auto leftVal  = prev.Estimation(leftX);
        auto rightVal = current.Estimation(rightX);
        if (abs(leftVal - rightVal) > EPS) {
            return false;
        }
        prev = *it;
    }
    return true;
}

template <typename T, template <typename> class Container>
size_t PieceWiseFunction<T, Container>::CountFragments() const {
    return fragments->GetLength();
}

template <typename T, template <typename> class Container>
Fragment<T> PieceWiseFunction<T, Container>::GetFragment(size_t index) const {
    return fragments->Get(index);
}

template <typename T, template <typename> class Container>
auto PieceWiseFunction<T, Container>::Estimation(double point) const {
    for (const auto& frag : *fragments) {
        if (frag.IsInteriorPoint(point)) 
            return frag.Estimation(point);
    }
    throw LogicErrorException("Функция не определена в этой точке");
}

template <typename T, template <typename> class Container>
std::string PieceWiseFunction<T, Container>::toString() const {
    if (!fragments || fragments->GetLength() == 0) {
        return "Function is not defined (no fragments).";
    }
    std::ostringstream oss;
    bool first = true;
    for (const auto& frag : *fragments) {
        if (!first) oss << "\n";
        oss << frag.toString();
        first = false;
    }
    return oss.str();
}