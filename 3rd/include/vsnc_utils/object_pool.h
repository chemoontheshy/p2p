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
		/// �����
		/// </summary>
		template <typename _Ty>
		class ObjectPool
		{
		public:

			/// <summary>����Ĺ���ָ������</summary>
			using obj_type = _Ty;
			/// <summary>Ԫ�ؿռ俪��������</summary>
			using _alloc_type = std::function<obj_type()>;

		private:

			/// <summary>��������</summary>
			using __ctnr_type = std::list<obj_type>;

		public:

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="alloc">Ԫ�ؿռ俪����</param>
			/// <param name="sz">����ش�С</param>
			explicit ObjectPool(_alloc_type alloc, const std::size_t sz = 1) noexcept :
				m_pfnAllocator(alloc), m_uSize(sz) {}

			/// <summary>
			/// ɾ���Ŀ������캯��
			/// </summary>
			ObjectPool(const ObjectPool<_Ty>&) = delete;

			/// <summary>
			/// ��Ĭ����������
			/// </summary>
			virtual ~ObjectPool() = default;

			/// <summary>
			/// �ж϶�����Ƿ�Ϊ��
			/// </summary>
			/// <returns>Ϊ�շ���true�����򷵻�false</returns>
			bool              Empty() const noexcept { return m_iContainer.empty(); }

			/// <summary>
			/// ��ȡ����Ԫ�ظ���
			/// </summary>
			/// <returns>����Ԫ�ظ���</returns>
			std::size_t       Size() const noexcept { return m_iContainer.size(); }

			/// <summary>
			/// ��ȡ������е�ͷ�ڵ�
			/// </summary>
			/// <returns>������е�ͷ�ڵ�</returns>
			virtual obj_type& Front() noexcept;

			/// <summary>
			/// �Ƴ�����ص�ͷ�ڵ�
			/// </summary>
			virtual void      Pop() noexcept;

			/// <summary>
			/// �ڶ����β���������
			/// </summary>
			/// <param name="obj">������Ķ���</param>
			virtual void      Push(const obj_type& obj);

			/// <summary>
			/// �ڶ����β���������
			/// </summary>
			/// <param name="obj">������Ķ���</param>
			virtual void      Push(obj_type&& obj) { Push(obj); }

		protected:

			/// <summary>
			/// ����һ����Ԫ��
			/// </summary>
			/// <returns>��Ԫ��</returns>
			obj_type          _Construct() { return m_pfnAllocator(); }

		private:

			/// <summary>Ԫ�ؿռ俪����</summary>
			_alloc_type       m_pfnAllocator;
			/// <summary>����ش�С</summary>
			const std::size_t m_uSize;

			/// <summary>������ȡ�������߳���</summary>
			std::mutex        m_iMutex;
			/// <summary>��������</summary>
			__ctnr_type       m_iContainer;
		};

		/// <summary>
		/// ��ȡ������е�ͷ�ڵ�
		/// </summary>
		/// <returns>������е�ͷ�ڵ�</returns>
		template<typename _Ty>
		inline typename ObjectPool<_Ty>::obj_type& ObjectPool<_Ty>::Front() noexcept
		{
			std::lock_guard<std::mutex> lock(m_iMutex);
			return m_iContainer.front();
		}

		/// <summary>
		/// �Ƴ�������е�ͷ�ڵ�
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
		/// �ڶ����β���������
		/// </summary>
		/// <param name="obj">������Ķ���</param>
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
		/// ����Ӧ����أ�����Front�������ܷ��ض���
		/// </summary>
		template <typename _Obj>
		class AdaptiveObjectPool final : public ObjectPool<_Obj>
		{
		public:

			/// <summary>������</summary>
			using base_type = ObjectPool<_Obj>;

			/// <summary>����Ĺ���ָ������</summary>
			using typename base_type::obj_type;
			/// <summary>�ռ俪��������</summary>
			using typename base_type::_alloc_type;

		public:

			/// <summary>���캯��</summary>
			using ObjectPool<_Obj>::ObjectPool;

			/// <summary>
			/// <para>��ȡ������е�ͷ�ڵ�</para>
			/// <para>��������û�ж�������һ���µģ���ӵ���ͷ��������</para>
			/// </summary>
			/// <returns>������е�ͷ�ڵ�</returns>
			obj_type& Front() noexcept override;
		};

		/// <summary>
		/// <para>��ȡ������е�ͷ�ڵ�</para>
		/// <para>��������û�ж�������һ���µģ���ӵ���ͷ��������</para>
		/// </summary>
		/// <returns>������е�ͷ�ڵ�</returns>
		template<typename _Obj>
		inline typename AdaptiveObjectPool<_Obj>::obj_type& AdaptiveObjectPool<_Obj>::Front() noexcept
		{
			if (base_type::Empty()) {
				base_type::Push(base_type::_Construct());
			}
			return base_type::Front();
		}


		/// <summary>
		/// ӵ�й̶�����������Ķ����
		/// </summary>
		template <typename _Obj>
		class FixedObjectPool final : public ObjectPool<_Obj>
		{
		public:

			/// <summary>Ԫ�ؿռ俪��������</summary>
			using typename ObjectPool<_Obj>::_alloc_type;

		public:

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="alloc">Ԫ�ؿռ俪����</param>
			/// <param name="sz">����ش�С</param>
			explicit FixedObjectPool(_alloc_type alloc, const std::size_t sz = 1);
		};

		/// <summary>
		/// ���캯��
		/// </summary>
		/// <param name="alloc">Ԫ�ؿռ俪����</param>
		/// <param name="sz">����ش�С</param>
		template<typename _Obj>
		inline FixedObjectPool<_Obj>::FixedObjectPool(_alloc_type alloc, const std::size_t sz) :
			ObjectPool<_Obj>(alloc, sz)
		{
			for (std::size_t cnt = 0; cnt < sz; ++cnt) {
				ObjectPool<_Obj>::Push(ObjectPool<_Obj>::_Construct());
			}
		}


		/// <summary>
		/// ӵ�й̶���������Ķ���أ�����Ϊ��ʱ������
		/// </summary>
		template <typename _Obj>
		class BlockedObjectPool
		{
			/// <summary>������������</summary>
			using __cond_var = std::condition_variable;
			/// <summary>�������</summary>
			using __obj_list = std::list<_Obj>;

		public:

			/// <summary>��������</summary>
			using obj_type = _Obj;

		public:

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="max">�������ֵ</param>
			/// <param name="none">�������Ϊ��ʱ���صĿն���</param>
			BlockedObjectPool(const std::size_t max, obj_type none);

			/// <summary>
			/// �ж������Ƿ�Ϊ��
			/// </summary>
			/// <returns>Ϊ�շ���true�����򷵻�false</returns>
			bool              Empty() const noexcept { return m_iContainer.empty(); }

			/// <summary>
			/// ��ȡ����Ԫ�ظ���
			/// </summary>
			/// <returns>����Ԫ�ظ���</returns>
			std::size_t       Size() const noexcept { return m_iContainer.size(); }

			/// <summary>
			/// �Ӷ�����л��һ������
			/// </summary>
			/// <returns>��õĶ���</returns>
			obj_type&         Front() noexcept;

			/// <summary>
			/// �Ƴ�����ص�ͷ�ڵ�
			/// </summary>
			void              Pop() noexcept;

			/// <summary>
			/// �黹һ�����󵽶����
			/// </summary>
			/// <param name="obj">���黹�Ķ���</param>
			void              Push(const obj_type& obj);

			/// <summary>
			/// �黹һ�����󵽶����
			/// </summary>
			/// <param name="obj">���黹�Ķ���</param>
			void              Push(obj_type&& obj) { Push(obj); }

			/// <summary>
			/// ��GetObject����������ʱ���ú������Դ���������״̬
			/// </summary>
			void              Stop() noexcept;

		private:

			/// <summary>����״ָ̬ʾ</summary>
			bool              m_bRun;
			/// <summary>���Ԫ����</summary>
			const std::size_t m_uMaxNums;
			/// <summary>�������Ϊ��ʱ���صĿն���</summary>
			obj_type          m_iNone;

			/// <summary>���µ�Ԫ�ر�����ʱ�������ź���</summary>
			__cond_var        m_iPushSignal;
			/// <summary>���µ�Ԫ�ر�����ʱ�������ź���</summary>
			__cond_var        m_iPopSignal;
			/// <summary>���Ա���������е��߳���</summary>
			std::mutex        m_iMutex;
			/// <summary>�������</summary>
			__obj_list        m_iContainer;
		};

		/// <summary>
		/// ���캯��
		/// </summary>
		/// <param name="max">�������ֵ</param>
		/// <param name="none">�������Ϊ��ʱ���صĿն���</param>
		template<typename _Obj>
		inline BlockedObjectPool<_Obj>::BlockedObjectPool(const std::size_t max, obj_type none) :
			m_bRun(true),
			m_uMaxNums(max),
			m_iNone(none)
		{
		}

		/// <summary>
		/// �黹һ�����󵽶����
		/// </summary>
		/// <param name="obj">���黹�Ķ���</param>
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
		/// �Ƴ�����ص�ͷ�ڵ�
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
		/// �黹һ�����󵽶����
		/// </summary>
		/// <param name="obj">���黹�Ķ���</param>
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
		/// ��GetObject����������ʱ���ú������Դ���������״̬
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

