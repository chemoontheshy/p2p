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
		/// 内存类型
		/// </summary>
		template <typename _Ty>
		class Memory
		{
		public:

			/// <summary>数据的指针类型</summary>
			using ptr_type = _Ty*;

		public:

			/// <summary>
			/// 虚默认析构函数
			/// </summary>
			virtual ~Memory() = default;

			/// <summary>
			/// 获取数据
			/// </summary>
			/// <returns>数据头指针</returns>
			virtual const ptr_type Data() const noexcept = 0;

			/// <summary>
			/// 获取数据
			/// </summary>
			/// <returns>数据头指针</returns>
			virtual ptr_type       Data() noexcept { return const_cast<ptr_type>((const_cast<const Memory*>(this)->Data())); }

			/// <summary>
			/// 获取数据长度
			/// </summary>
			/// <returns>数据长度</returns>
			virtual std::size_t    Length() const noexcept = 0;

			/// <summary>
			/// 针对空指针的不等运算符重载
			/// </summary>
			/// <param name="">空指针</param>
			/// <returns>所存数据非空返回true，否则返回false</returns>
			virtual bool           operator!=(const nullptr_t) const noexcept { return (nullptr != Data()); }

			/// <summary>
			/// 对空指针的相等运算符重载
			/// </summary>
			/// <param name="">空指针</param>
			/// <returns>所存数据为空返回true，否则返回false</returns>
			virtual bool           operator==(const nullptr_t) const noexcept { return !(this->operator!=(nullptr)); }
		};

		/// <summary>
		/// 针对空指针的不等运算符重载
		/// </summary>
		/// <param name="">空指针</param>
		/// <param name="rhs">右手式</param>
		/// <returns>所存数据非空返回true，否则返回false</returns>
		template <typename _Ty>
		inline bool operator!=(const nullptr_t, const Memory<_Ty>& rhs) { return (rhs != nullptr); }

		/// <summary>
		/// 对空指针的相等运算符重载
		/// </summary>
		/// <param name="">空指针</param>
		/// <param name="rhs">右手式</param>
		/// <returns>所存数据为空返回true，否则返回false</returns>
		template <typename _Ty>
		inline bool operator==(const nullptr_t, const Memory<_Ty>& rhs) { return (rhs == nullptr); }


		/// <summary>
		/// 基本内存类型
		/// </summary>
		template <typename _Ty>
		class BasicMemory final : public Memory<_Ty>
		{
		public:

			/// <summary>数据的指针类型</summary>
			using typename Memory<_Ty>::ptr_type;

		public:

			/// <summary>
			/// 默认构造函数
			/// </summary>
			BasicMemory() noexcept { Clear(); }

			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="p">数据头指针</param>
			/// <param name="sz">数据长度</param>
			BasicMemory(ptr_type const p, const std::size_t sz) :
				m_pPointer(p), m_uSize(sz) {}

			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="mem">一段内存</param>
			BasicMemory(const Memory<_Ty>& mem) :
				m_pPointer(mem.Data()), m_uSize(mem.Length()) {}

			/// <summary>
			/// 获取数据
			/// </summary>
			/// <returns>数据头指针</returns>
			const ptr_type    Data() const noexcept override { return m_pPointer; }

			/// <summary>
			/// 获取数据长度
			/// </summary>
			/// <returns>数据长度</returns>
			std::size_t       Length() const noexcept override { return m_uSize; }

			/// <summary>
			/// 清空所存数据
			/// </summary>
			void              Clear() noexcept;

			/// <summary>
			/// 拷贝运算符
			/// </summary>
			/// <param name="rhs">右手式</param>
			/// <returns>自身的引用</returns>
			BasicMemory<_Ty>& operator=(const Memory<_Ty>& rhs) noexcept;

		private:

			/// <summary>存储的指针</summary>
			ptr_type          m_pPointer;
			/// <summary>存储指针所指数据长度</summary>
			std::size_t       m_uSize;
		};

		/// <summary>
		/// 清空所存数据
		/// </summary>
		template<typename _Ty>
		inline void BasicMemory<_Ty>::Clear() noexcept
		{
			m_pPointer = nullptr;
			m_uSize = 0;
		}

		/// <summary>
		/// 拷贝运算符
		/// </summary>
		/// <param name="rhs">右手式</param>
		/// <returns>自身的引用</returns>
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
		/// 以二进制格式将内存数据写入文件
		/// </summary>
		class MemoryWriter
		{
		public:

			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="path">待写入内存数据的文件路径</path>
			explicit MemoryWriter(const std::string path);

			/// <summary>
			/// 析构函数
			/// </summary>
			~MemoryWriter();

			/// <summary>
			/// 左移运算符重载，将数据写入文件
			/// </summary>
			/// <param name="mem">待写入的数据</param>
			/// <returns>自身的引用，以支持连续调用</returns>
			template <typename _Ty>
			MemoryWriter& operator<<(const Memory<_Ty>& mem);

		private:

			/// <summary>文件句柄</summary>
			std::fstream  m_hFile;
		};

		/// <summary>
		/// 左移运算符重载，将数据写入文件
		/// </summary>
		/// <param name="mem">待写入的数据</param>
		/// <returns>自身的引用，以支持连续调用</returns>
		template<typename _Ty>
		inline MemoryWriter& MemoryWriter::operator<<(const Memory<_Ty>& mem)
		{
			m_hFile.write(reinterpret_cast<const char*>(mem.Data()), mem.Length() * sizeof(_Ty));
			return (*this);
		}


	}

}


#endif // !__VSNC_UTILS_MEMORY_H__

