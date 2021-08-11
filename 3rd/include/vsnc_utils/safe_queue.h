#ifndef __VSNC_UTILS_SAFE_QUEUE_H__
#define __VSNC_UTILS_SAFE_QUEUE_H__


#include <queue>
#include <mutex>
#include <stdexcept>


#include "error.h"


namespace vsnc
{

	namespace utils
	{


		/// <summary>
		/// <para>线程安全的队列</para>
		/// <para>仅提供存取操作</para>
		/// </summary>
		template<typename T>
		class SafeQueue
		{
		public:

			/// <summary>
			/// 默认构造函数
			/// </summary>
			SafeQueue() = default;

			/// <summary>
			/// 类中使用了互斥锁，因此删除拷贝构造函数
			/// </summary>
			SafeQueue(const SafeQueue<T>&) = delete;

			/// <summary>
			/// 获取队列尾部数据的引用
			/// </summary>
			/// <returns>队列尾部数据的引用</returns>
			T&             back();

			/// <summary>
			/// 获取队列头部数据的引用
			/// </summary>
			/// <returns>队列头部数据的引用</returns>
			T&             front();

			/// <summary>
			/// 弹出队列头部的数据
			/// </summary>
			void           pop();

			/// <summary>
			/// 在队列尾部插入数据
			/// </summary>
			/// <param name="timestamp">待插入数据的引用</param>
			void           push(const T& value);

			/// <summary>
			/// 在队列尾部插入数据
			/// </summary>
			/// <param name="timestamp">待插入数据的右值引用</param>
			void           push(const T&& value);

			/// <summary>
			/// 判断队列是否为空
			/// </summary>
			/// <returns>为空返回非零值，否则返回零值</returns>
			bool           empty() const { return m_queue.empty(); }

		private:

			/// <summary>
			/// 安全执行队列操作
			/// </summary>
			template<typename Func, typename... Args>
			decltype(auto) __safe_queueopt(Func fn, Args&&... args);

		private:

			/// <summary>队列</summary>
			std::queue<T>  m_queue;
			/// <summary>确保队列安全的互斥锁</summary>
			std::mutex     m_mutex;
		};


		/// <summary>
		/// 安全执行队列操作
		/// </summary>
		template<typename T>
		template<typename Func, typename... Args>
		decltype(auto) SafeQueue<T>::__safe_queueopt(Func fn, Args&&... args)
		{
			if (m_queue.empty()) {
				throw std::runtime_error(fmt_err("queue is empty"));
			}
			std::unique_lock<std::mutex> lock(m_mutex);
			return fn(std::forward<Args>(args)...);
		}


		/// <summary>
		/// 获取队列尾部数据的引用
		/// </summary>
		/// <returns>队列尾部数据的引用</returns>
		template<typename T>
		inline T& SafeQueue<T>::back()
		{
			return __safe_queueopt([this]() -> decltype(auto) { return m_queue.back(); });
		}


		/// <summary>
		/// 获取队列头部数据的引用
		/// </summary>
		/// <returns>队列头部数据的引用</returns>
		template<typename T>
		inline T& SafeQueue<T>::front()
		{
			return __safe_queueopt([this]() -> decltype(auto) { return m_queue.front(); });
		}


		/// <summary>
		/// 弹出队列头部的数据
		/// </summary>
		template<typename T>
		inline void SafeQueue<T>::pop()
		{
			__safe_queueopt([this]() { m_queue.pop(); });
		}

		/// <summary>
		/// 在队列尾部插入数据
		/// </summary>
		/// <param name="timestamp">待插入数据的引用</param>
		template<typename T>
		inline void SafeQueue<T>::push(const T& value)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_queue.push(value);
		}


		/// <summary>
		/// 在队列尾部插入数据
		/// </summary>
		/// <param name="timestamp">待插入数据的右值引用</param>
		template<typename T>
		inline void SafeQueue<T>::push(const T&& value)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_queue.push(std::move(value));
		}


	}

}


#endif // !__VSNC_UTILS_SAFE_QUEUE_H__

