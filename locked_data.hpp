#pragma once

#include <mutex>

template <typename T>
class locked_data {
	T& data;
	std::unique_lock<std::mutex> lock;

public:
	locked_data(T& data, std::unique_lock<std::mutex>&& lock)
		: data(data), lock(std::move(lock))
	{ }

	void release()
	{
		lock.unlock();
	}

	T& get_data()
	{
		[[unlikely]] if (!lock.owns_lock())
			throw;

		return data;
	}

	locked_data(const locked_data<T>&) = delete;
	locked_data(locked_data<T>&&) = default;
};