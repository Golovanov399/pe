#pragma once

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>

using std::array, std::tuple, std::swap;
using std::forward_as_tuple;

template <typename T, int dim> struct Tensor;

template <typename T, int dim, int idx, class D = void>
struct TensorView {
	const array<int, dim>& shape;
	const array<int, dim>& subsizes;
	T* data;

	TensorView<T, dim, idx + 1> operator [](int i) {
		return {shape, subsizes, data + subsizes[idx] * i};
	}

	TensorView<T, dim, idx + 1> back() {
		return operator [](shape[idx] - 1);
	}
};

template <typename T, int dim, int idx>
struct TensorView<T, dim, idx, std::enable_if_t<idx + 1 == dim>> {
	const array<int, dim>& shape;
	const array<int, dim>& subsizes;
	T* data;

	T& operator [](int i) {
		return data[i];
	}

	T& back() {
		return operator [](shape[idx] - 1);
	}
};

template <int... I>
struct indices {
	using next = indices<I..., sizeof...(I)>;

	template <typename... T>
	static array<std::remove_cvref_t<std::tuple_element_t<0, tuple<T...>>>, sizeof...(I)> of(tuple<T...>&& t) {
		return {get<I>(t)...};
	}
};

template <int N>
struct build_indices {
	using type = typename build_indices<N - 1>::type::next;
};

template <>
struct build_indices<0> {
	using type = indices<>;
};

template <typename T, int dim>
struct Tensor {
	static_assert(dim > 0);
	array<int, dim> shape;
	array<int, dim> subsizes;
	int sz;
	T* data;

	template <typename... Int>
	explicit Tensor(Int... args) {
		T val = T();
		if constexpr (sizeof...(args) == dim + 1) {
			val = get<dim>(forward_as_tuple(args...));
			shape = build_indices<dim>::type::of(forward_as_tuple(args...));
		} else {
			static_assert(sizeof...(args) == dim);
			shape = {args...};
		}
		subsizes.fill(1);
		for (int i = dim - 2; i >= 0; --i) {
			subsizes[i] = subsizes[i + 1] * shape[i + 1];
		}
		sz = subsizes[0] * shape[0];
		data = new T[sz];
		this->fill(val);
	}

	Tensor(const Tensor& ot): shape(ot.shape), subsizes(ot.subsizes), sz(ot.sz), data(new T[sz]) {
		memcpy(data, ot.data, sizeof(T) * sz);
	}

	Tensor& operator =(Tensor&& ot) noexcept {
		shape.swap(ot.shape);
		subsizes.swap(ot.subsizes);
		swap(sz, ot.sz);
		swap(data, ot.data);
		return *this;
	}

	Tensor(Tensor&& ot) {
		*this = move(ot);
	}

	~Tensor() {
		delete[] data;
	}

	void fill(const T& val) {
		std::fill(data, data + sz, val);
	}

	TensorView<T, dim, 0> view() {
		return {shape, subsizes, data};
	}

	TensorView<const T, dim, 0> const_view() const {
		return {shape, subsizes, data};
	}

	auto operator [](int idx) {
		return view()[idx];
	}

	auto back() {
		return view().back();
	}

	auto operator [](int idx) const {
		return const_view()[idx];
	}

	auto back() const {
		return const_view().back();
	}

	T& operator [](array<int, dim> ids) {
		int i = 0;
		for (int j = 0; j < dim; ++j) {
			i += ids[j] * subsizes[j];
		}
		return data[i];
	}

	const T& operator [](array<int, dim> ids) const {
		int i = 0;
		for (int j = 0; j < dim; ++j) {
			i += ids[j] * subsizes[j];
		}
		return data[i];
	}
};
