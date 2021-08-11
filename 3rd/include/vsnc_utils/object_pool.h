#ifndef __VSNC_UTILS_OBJECT_POOL_H__
#define __VSNC_UTILS_OBJECT_POOL_H__


#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <list>
#include <algorithm>


#include <iostream>


namespace vsnc
{

	namespace utils
	{


		/// <summary>
		/// 对象池
		/// </summary>
		template <typename _Ty>
		class ObjectPool
		{
		public:

			/// <summary>对象的共享指针类型</summary>
			using obj_type = _Ty;
			/// <summary>元素空间开辟器类型</summary>
			using _alloc_type = std::function<obj_type()>;

		private:

			/// <summary>容器类型</summary>
			using __ctnr_type = std::list<obj_type>;

		public:

			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="alloc">元素空间开辟器</param>
			/// <param name="sz">对象池大小</param>
			explicit ObjectPool(_alloc_type alloc, const std::size_t sz = 1) noexcept :
				m_pfnAllocator(alloc), m_uSize(sz) {}

			/// <summary>
			/// 删除的拷贝构造函数
			/// </summary>
			ObjectPool(const ObjectPool<_Ty>&) = delete;

			/// <summary>
			/// 虚默认析构函数
			/// </summary>
			virtual ~ObjectPool() = default;

			/// <summary>
			/// 判断对象池是否为空
			/// </summary>
			/// <returns>为空返回true，否则返回false</returns>
			bool              Empty() const noexcept { return m_iContainer.empty(); }

			/// <summary>
			/// 获取池中元素个数
			/// </summary>
			/// <returns>池中元素个数</returns>
			std::size_t       Size() const noexcept { return m_iContainer.size(); }

			/// <summary>
			/// 获取对象池中的头节点
			/// </summary>
			/// <returns>对象池中的头节点</returns>
			virtual obj_type& Front() noexcept;

			/// <summary>
			/// 移除对象池的头节点
			/// </summary>
			virtual void      Pop() noexcept;

			/// <summary>
			/// 在对象池尾部推入对象
			/// </summary>
			/// <param name="obj">待推入的对象</param>
			virtual void      Push(const obj_type& obj);

			/// <summary>
			/// 在对象池尾部推入对象
			/// </summary>
			/// <param name="obj">待推入的对象</param>
			virtual void      Push(obj_type&& obj) { Push(obj); }

		protected:

			/// <summary>
			/// 构造一个新元素
			/// </summary>
			/// <returns>新元素</returns>
			obj_type          _Construct() { return m_pfnAllocator(); }

		private:

			/// <summary>元素空间开辟器</summary>
			_alloc_type       m_pfnAllocator;
			/// <summary>对象池大小</summary>
			const std::size_t m_uSize;

			/// <summary>保护存取操作的线程锁</summary>
			std::mutex        m_iMutex;
			/// <summary>对象容器</summary>
			__ctnr_type       m_iContainer;
		};

		/// <summary>
		/// 获取对象池中的头节点
		/// </summary>
		/// <returns>对象池中的头节点</returns>
		template<typename _Ty>
		inline typename ObjectPool<_Ty>::obj_type& ObjectPool<_Ty>::Front() noexcept
		{
			std::lock_guard<std::mutex> lock(m_iMutex);
			return m_iContainer.front();
		}

		/// <summary>
		/// 移除对象池中的头节点
		/// </summary>
		template<typename _Ty>
		inline void ObjectPool<_Ty>::Pop() noexcept
		{
			if (!m_iContainer.empty()) {
				std::lock_guard<std::mutex> lock(m_iMutex);
				m_iContainer.pop_front();
			}
		}

		/// <summary>
		/// 在对象池尾部推入对象
		/// </summary>
		/// <param name="obj">待推入的对象</param>
		template<typename _Ty>
		inline void ObjectPool<_Ty>::Push(const obj_type& obj)
		{
			if (m_iContainer.size() < m_uSize) {
				std::lock_guard<std::mutex> lock(m_iMutex);
				if (m_iContainer.end() == std::find(m_iContainer.begin(), m_iContainer.end(), obj)) {
					m_iContainer.push_back(obj);
				}
			}
		}


		/// <summary>
		/// 自适应对象池，它的Front函数总能返回对象
		/// </summary>
		template <typename _Obj>
		class AdaptiveObjectPool final : public ObjectPool<_Obj>
		{
		public:

			/// <summary>基类型</summary>
			using base_type = ObjectPool<_Obj>;

			/// <summary>对象的共享指针类型</summary>
			using typename base_type::obj_type;
			/// <summary>空间开辟器类型</summary>
			using typename base_type::_alloc_type;

		public:

			/// <summary>构造函数</summary>
			using ObjectPool<_Obj>::ObjectPool;

			/// <summary>
			/// <para>获取对象池中的头节点</para>
			/// <para>若容器中没有对象，则构造一个新的，添加到其头部并返回</para>
			/// </summary>
			/// <returns>对象池中的头节点</returns>
			obj_type& Front() noexcept override;
		};

		/// <summary>
		/// <para>获取对象池中的头节点</para>
		/// <para>若容器中没有对象，则构造一个新的，添加到其头部并返回</para>
		/// </summary>
		/// <returns>对象池中的头节点</returns>
		template<typename _Obj>
		inline typename AdaptiveObjectPool<_Obj>::obj_type& AdaptiveObjectPool<_Obj>::Front() noexcept
		{
			if (base_type::Empty()) {
				base_type::Push(base_type::_Construct());
			}
			return base_type::Front();
		}


		/// <summary>
		/// 拥有固定个数个对象的对象池
		/// </summary>
		template <typename _Obj>
		class FixedObjectPool final : public ObjectPool<_Obj>
		{
		public:

			/// <summary>元素空间开辟器类型</summary>
			using typename ObjectPool<_Obj>::_alloc_type;

		public:

			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="alloc">元素空间开辟器</param>
			/// <param name="sz">对象池大小</param>
			explicit FixedObjectPool(_alloc_type alloc, const std::size_t sz = 1);
		};

		/// <summary>
		/// 构造函数
		/// </summary>
		/// <param name="alloc">元素空间开辟器</param>
		/// <param name="sz">对象池大小</param>
		template<typename _Obj>
		inline FixedObjectPool<_Obj>::FixedObjectPool(_alloc_type alloc, const std::size_t sz) :
			ObjectPool<_Obj>(alloc, sz)
		{
			for (std::size_t cnt = 0; cnt < sz; ++cnt) {
				ObjectPool<_Obj>::Push(ObjectPool<_Obj>::_Construct());
			}
		}


		/// <summary>
		/// 拥有固定对象个数的对象池，当其为空时将阻塞
		/// </summary>
		template <typename _Obj>
		class BlockedObjectPool
		{
			/// <summary>条件变量类型</summary>
			using __cond_var = std::condition_variable;
			/// <summary>对象队列</summary>
			using __obj_list = std::list<_Obj>;

		public:

			/// <summary>对象类型</summary>
			using obj_type = _Obj;

		public:

			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="max">容量最大值</param>
			/// <param name="none">当对象池为空时返回的空对象</param>
			BlockedObjectPool(const std::size_t max, obj_type none);

			/// <summary>
			/// 判断容器是否为空
			/// </summary>
			/// <returns>为空返回true，否则返回false</returns>
			bool              Empty() const noexcept { return m_iContainer.empty(); }

			/// <summary>
			/// 获取池中元素个数
			/// </summary>
			/// <returns>池中元素个数</returns>
			std::size_t       Size() const noexcept { return m_iContainer.size(); }

			/// <summary>
			/// 从对象池中获得一个对象
			/// </summary>
			/// <returns>获得的对象</returns>
			obj_type&         Front() noexcept;

			/// <summary>
			/// 移除对象池的头节点
			/// </summary>
			void              Pop() noexcept;

			/// <summary>
			/// 归还一个对象到对象池
			/// </summary>
			/// <param name="obj">待归还的对象</param>
			void              Push(const obj_type& obj);

			/// <summary>
			/// 归还一个对象到对象池
			/// </summary>
			/// <param name="obj">待归还的对象</param>
			void              Push(obj_type&& obj) { Push(obj); }

			/// <summary>
			/// 当GetObject是阻塞函数时，该函数可以打破其阻塞状态
			/// </summary>
			void              Stop() noexcept;

		private:

			/// <summary>运行状态指示</summary>
			bool              m_bRun;
			/// <summary>最大元素数</summary>
			const std::size_t m_uMaxNums;
			/// <summary>当对象池为空时返回的空对象</summary>
			obj_type          m_iNone;

			/// <summary>有新的元素被推入时触发的信号量</summary>
			__cond_var        m_iPushSignal;
			/// <summary>有新的元素被弹出时触发的信号量</summary>
			__cond_var        m_iPopSignal;
			/// <summary>用以保护对象队列的线程锁</summary>
			std::mutex        m_iMutex;
			/// <summary>对象队列</summary>
			__obj_list        m_iContainer;
		};

		/// <summary>
		/// 构造函数
		/// </summary>
		/// <param name="max">容量最大值</param>
		/// <param name="none">当对象池为空时返回的空对象</param>
		template<typename _Obj>
		inline BlockedObjectPool<_Obj>::BlockedObjectPool(const std::size_t max, obj_type none) :
			m_bRun(true),
			m_uMaxNums(max),
			m_iNone(none)
		{
		}

		/// <summary>
		/// 归还一个对象到对象池
		/// </summary>
		/// <param name="obj">待归还的对象</param>
		template<typename _Obj>
		inline typename BlockedObjectPool<_Obj>::obj_type& BlockedObjectPool<_Obj>::Front() noexcept
		{
			std::unique_lock<std::mutex> lock(m_iMutex);
			while (m_bRun && m_iContainer.empty()) {
				m_iPushSignal.wait(lock);
			}
			if (m_bRun) {
				return m_iContainer.front();
			}
			return m_iNone;
		}

		/// <summary>
		/// 移除对象池的头节点
		/// </summary>
		template<typename _Obj>
		inline void BlockedObjectPool<_Obj>::Pop() noexcept
		{
			if (m_iContainer.empty()) {
				return;
			}
			std::unique_lock<std::mutex> lock(m_iMutex);
			m_iContainer.pop_front();
			lock.unlock();
			m_iPopSignal.notify_one();
		}

		/// <summary>
		/// 归还一个对象到对象池
		/// </summary>
		/// <param name="obj">待归还的对象</param>
		template<typename _Obj>
		inline void BlockedObjectPool<_Obj>::Push(const obj_type& obj)
		{
			std::unique_lock<std::mutex> lock(m_iMutex);
			auto iter = std::find(m_iContainer.begin(), m_iContainer.end(), obj);
			if (iter != m_iContainer.end()) {
				return;
			}
			while (m_bRun && (m_iContainer.size() >= m_uMaxNums)) {
				m_iPopSignal.wait(lock);
			}
			if (!m_bRun) {
				return;
			}
			m_iContainer.push_back(obj);
			lock.unlock();
			m_iPushSignal.notify_one();
		}

		/// <summary>
		/// 当GetObject是阻塞函数时，该函数可以打破其阻塞状态
		/// </summary>
		template<typename _Obj>
		inline void BlockedObjectPool<_Obj>::Stop() noexcept
		{
			m_bRun = false;
			m_iPushSignal.notify_all();
			m_iPopSignal.notify_all();
		}


	}

}


#endif // !__VSNC_UTILS_OBJECT_POOL_H__

