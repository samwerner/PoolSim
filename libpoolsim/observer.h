#pragma once

#include <memory>
#include <vector>

namespace poolsim {

template <typename T>
class Observer {
public:
    virtual void process(const T& value) = 0;
};



template <typename T>
class Observable {
public:
    void add_observer(std::shared_ptr<Observer<T>> observer);
    void notify(const T& value);
private:
    std::vector<std::shared_ptr<Observer<T>>> observers;
};


template <typename T>
void Observable<T>::add_observer(std::shared_ptr<Observer<T>> observer) {
    observers.push_back(observer);
}

template <typename T>
void Observable<T>::notify(const T& value) {
    for (auto observer : observers) {
        observer->process(value);
    }
}

}
