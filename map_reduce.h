#pragma once
#include <stdint.h>
#include <vector>
#include <functional>

template <typename T, typename R> R reduce(const T* data, const size_t length, std::function<R(const R initialValue, const T item, const size_t index)>& lambda, R initialValue) {
	for (size_t index = 0; index < length; index++)
	{
		const T item = T[index];
		initialValue = lambda(initialValue, T, index);
	}
	return initialValue;
}

template <typename R>
using MapResult = std::vector<R>;

template <typename T, typename R, typename F> MapResult<R>* map(const T* data, const size_t length, F lambda) {
	return reduce(
		data,
		length,
		[lambda](MapResult<R>* result, const T item, const size_t index) -> MapResult<R>*{
			(*result)[index] = lambda(item, index);
			return result;
		},
		new MapResult(length)
	);
}