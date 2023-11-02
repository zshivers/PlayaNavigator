#pragma once

template <typename T>
struct MaybeValid {
  MaybeValid() : valid(false), value{} {}
  MaybeValid(bool valid, T value) : valid(valid), value(value) {}
  bool valid = false;
  T value = {};
};

template <typename T>
MaybeValid<T> MakeValid(T value) {
  return MaybeValid<T>{true, value};
}

template <typename T>
MaybeValid<T> MakeInvalid(T value) {
  return MaybeValid<T>{false, value};
}