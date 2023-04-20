#pragma once

#include <mutex>
#include <deque>

template<typename T>
class TSQueue
{
public:
	inline TSQueue();
	inline ~TSQueue();

	const T& front();
	const T& back();

	void push_back(const T& item);
	void push_front(const T& item);

	bool empty();
	void clear();
	size_t size();

	T pop_front();
	T pop_back();

protected:
	std::mutex					m_Mutex;
	std::deque<T>				m_Container;
};

// =============================================================================
template<typename T>
inline TSQueue<T>::TSQueue()
{
}

// =============================================================================
template<typename T>
inline TSQueue<T>::~TSQueue()
{
	clear();
}

// =============================================================================
template<typename T>
inline const T& TSQueue<T>::front()
{
	std::scoped_lock lock(m_Mutex);
	return m_Container.front();
}

// =============================================================================
template<typename T>
inline const T& TSQueue<T>::back()
{
	std::scoped_lock lock(m_Mutex);
	return m_Container.back();
}

// =============================================================================
template<typename T>
inline void TSQueue<T>::push_back(const T& item)
{
	std::scoped_lock lock(m_Mutex);
	m_Container.push_back(std::move(item));
}

// =============================================================================
template<typename T>
inline void TSQueue<T>::push_front(const T& item)
{
	std::scoped_lock lock(m_Mutex);
	m_Container.push_front(std::move(item));
}

// =============================================================================
template<typename T>
inline bool TSQueue<T>::empty()
{
	std::scoped_lock lock(m_Mutex);
	return m_Container.empty();
}

// =============================================================================
template<typename T>
inline void TSQueue<T>::clear()
{
	std::scoped_lock lock(m_Mutex);
	m_Container.clear();
}

// =============================================================================
template<typename T>
inline size_t TSQueue<T>::size()
{
	std::scoped_lock lock(m_Mutex);
	return m_Container.size();
}

// =============================================================================
template<typename T>
inline T TSQueue<T>::pop_front()
{
	std::scoped_lock lock(m_Mutex);
	auto t = std::move(m_Container.front());
	m_Container.pop_front();

	return t;
}

// =============================================================================
template<typename T>
inline T TSQueue<T>::pop_back()
{
	std::scoped_lock lock(m_Mutex);
	auto t = std::move(m_Container.back());
	m_Container.pop_back();

	return t;
}