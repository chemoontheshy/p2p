#ifndef __VSNC_UTILS_MEMORY_H__
#define __VSNC_UTILS_MEMORY_H__


#include <string>
#include <fstream>


#include "utils.h"


namespace vsnc
{

	namespace utils
	{


		/// <summary>
		/// �ڴ�����
		/// </summary>
		template <typename _Ty>
		class Memory
		{
		public:

			/// <summary>���ݵ�ָ������</summary>
			using ptr_type = _Ty*;

		public:

			/// <summary>
			/// ��Ĭ����������
			/// </summary>
			virtual ~Memory() = default;

			/// <summary>
			/// ��ȡ����
			/// </summary>
			/// <returns>����ͷָ��</returns>
			virtual const ptr_type Data() const noexcept = 0;

			/// <summary>
			/// ��ȡ����
			/// </summary>
			/// <returns>����ͷָ��</returns>
			virtual ptr_type       Data() noexcept { return const_cast<ptr_type>((const_cast<const Memory*>(this)->Data())); }

			/// <summary>
			/// ��ȡ���ݳ���
			/// </summary>
			/// <returns>���ݳ���</returns>
			virtual std::size_t    Length() const noexcept = 0;

			/// <summary>
			/// ��Կ�ָ��Ĳ������������
			/// </summary>
			/// <param name="">��ָ��</param>
			/// <returns>�������ݷǿշ���true�����򷵻�false</returns>
			virtual bool           operator!=(const nullptr_t) const noexcept { return (nullptr != Data()); }

			/// <summary>
			/// �Կ�ָ���������������
			/// </summary>
			/// <param name="">��ָ��</param>
			/// <returns>��������Ϊ�շ���true�����򷵻�false</returns>
			virtual bool           operator==(const nullptr_t) const noexcept { return !(this->operator!=(nullptr)); }
		};

		/// <summary>
		/// ��Կ�ָ��Ĳ������������
		/// </summary>
		/// <param name="">��ָ��</param>
		/// <param name="rhs">����ʽ</param>
		/// <returns>�������ݷǿշ���true�����򷵻�false</returns>
		template <typename _Ty>
		inline bool operator!=(const nullptr_t, const Memory<_Ty>& rhs) { return (rhs != nullptr); }

		/// <summary>
		/// �Կ�ָ���������������
		/// </summary>
		/// <param name="">��ָ��</param>
		/// <param name="rhs">����ʽ</param>
		/// <returns>��������Ϊ�շ���true�����򷵻�false</returns>
		template <typename _Ty>
		inline bool operator==(const nullptr_t, const Memory<_Ty>& rhs) { return (rhs == nullptr); }


		/// <summary>
		/// �����ڴ�����
		/// </summary>
		template <typename _Ty>
		class BasicMemory final : public Memory<_Ty>
		{
		public:

			/// <summary>���ݵ�ָ������</summary>
			using typename Memory<_Ty>::ptr_type;

		public:

			/// <summary>
			/// Ĭ�Ϲ��캯��
			/// </summary>
			BasicMemory() noexcept { Clear(); }

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="p">����ͷָ��</param>
			/// <param name="sz">���ݳ���</param>
			BasicMemory(ptr_type const p, const std::size_t sz) :
				m_pPointer(p), m_uSize(sz) {}

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="mem">һ���ڴ�</param>
			BasicMemory(const Memory<_Ty>& mem) :
				m_pPointer(mem.Data()), m_uSize(mem.Length()) {}

			/// <summary>
			/// ��ȡ����
			/// </summary>
			/// <returns>����ͷָ��</returns>
			const ptr_type    Data() const noexcept override { return m_pPointer; }

			/// <summary>
			/// ��ȡ���ݳ���
			/// </summary>
			/// <returns>���ݳ���</returns>
			std::size_t       Length() const noexcept override { return m_uSize; }

			/// <summary>
			/// �����������
			/// </summary>
			void              Clear() noexcept;

			/// <summary>
			/// ���������
			/// </summary>
			/// <param name="rhs">����ʽ</param>
			/// <returns>���������</returns>
			BasicMemory<_Ty>& operator=(const Memory<_Ty>& rhs) noexcept;

		private:

			/// <summary>�洢��ָ��</summary>
			ptr_type          m_pPointer;
			/// <summary>�洢ָ����ָ���ݳ���</summary>
			std::size_t       m_uSize;
		};

		/// <summary>
		/// �����������
		/// </summary>
		template<typename _Ty>
		inline void BasicMemory<_Ty>::Clear() noexcept
		{
			m_pPointer = nullptr;
			m_uSize = 0;
		}

		/// <summary>
		/// ���������
		/// </summary>
		/// <param name="rhs">����ʽ</param>
		/// <returns>���������</returns>
		template<typename _Ty>
		inline BasicMemory<_Ty>& BasicMemory<_Ty>::operator=(const Memory<_Ty>& rhs) noexcept
		{
			if (this == &rhs) {
				return *this;
			}
			m_pPointer = rhs.Data();
			m_uSize = rhs.Length();

			return *this;
		}


		/// <summary>
		/// �Զ����Ƹ�ʽ���ڴ�����д���ļ�
		/// </summary>
		class MemoryWriter
		{
		public:

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="path">��д���ڴ����ݵ��ļ�·��</path>
			explicit MemoryWriter(const std::string path);

			/// <summary>
			/// ��������
			/// </summary>
			~MemoryWriter();

			/// <summary>
			/// ������������أ�������д���ļ�
			/// </summary>
			/// <param name="mem">��д�������</param>
			/// <returns>��������ã���֧����������</returns>
			template <typename _Ty>
			MemoryWriter& operator<<(const Memory<_Ty>& mem);

		private:

			/// <summary>�ļ����</summary>
			std::fstream  m_hFile;
		};

		/// <summary>
		/// ������������أ�������д���ļ�
		/// </summary>
		/// <param name="mem">��д�������</param>
		/// <returns>��������ã���֧����������</returns>
		template<typename _Ty>
		inline MemoryWriter& MemoryWriter::operator<<(const Memory<_Ty>& mem)
		{
			m_hFile.write(reinterpret_cast<const char*>(mem.Data()), mem.Length() * sizeof(_Ty));
			return (*this);
		}


	}

}


#endif // !__VSNC_UTILS_MEMORY_H__

