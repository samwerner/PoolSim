#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <iostream>


class NotRegisteredException : public std::exception {
public:
  explicit NotRegisteredException(const std::string& name);
  virtual char const* what() const throw();
private:
  std::string name;
};


// XXX: not sure how to use variadic templates for CreatableN
// compiler seemed to have trouble deducing types when I tried
template <typename Base, typename T>
class Creatable0 {
public:
  static std::unique_ptr<Base> create() {
    return std::unique_ptr<T>(new T);
  }
};

template <typename Base, typename T, typename Arg>
class Creatable1 {
public:
  static std::unique_ptr<Base> create(Arg arg) {
    return std::unique_ptr<T>(new T(arg));
  }
};


template <typename T, typename CreateMethod>
class Factory {
public:
  Factory() = delete;

  static std::vector<std::string> registered() {
    std::vector<std::string> names;
    for (auto kv : get_methods()) {
      names.push_back(kv.first);
    }
    return names;
  }

  static bool register_class(const std::string& name, CreateMethod create_method) {
    auto it = get_methods().find(name);
    if (it == get_methods().end()) {
      get_methods()[name] = create_method;
      return true;
    }
    return false;
  }

  template<typename... Ts>
  static std::unique_ptr<T> create(const std::string& name, Ts... args) {
    auto it = get_methods().find(name);
    if (it != get_methods().end()) {
        return it->second(args...);
    }
    throw NotRegisteredException(name);
  }

private:
  static std::map<std::string, CreateMethod>& get_methods() {
    static std::map<std::string, CreateMethod> methods;
    return methods;
  }
};

#define MAKE_FACTORY(factory_name, name, ...) \
  using factory_name = Factory<name, std::unique_ptr<name>(*)(__VA_ARGS__)>;

#define REGISTER(base, clazz, name) \
  volatile bool base ## _ ## clazz ## _registered = \
    Factory<base, decltype(&clazz::create)>::register_class(name, clazz::create);
