#ifndef __VSNC_UTILS_UTILS_H__
#define __VSNC_UTILS_UTILS_H__


#include <sstream> // std::stringstream, std::fixed
#include <iomanip> // std::setprecision
#include <chrono>
#include <thread> // std::this_thread
#include <cstring>


//#ifdef _WIN32
//#ifdef _WINDLL
//#define VSNC_UTILS_PORT __declspec(dllexport)
//#else
//#define VSNC_UTILS_PORT __declspec(dllimport)
//#endif // WIN32_DLL
//#else
//#define VSNC_UTILS_PORT
//#endif // _WIN32


namespace vsnc
{

	namespace utils
	{

		/// <summary>
		/// ������תΪ������ֵ
		/// </summary>
		/// <param name="value">����������ֵ</param>
		/// <returns>������ֵ</returns>
		template <typename _Ty>
		inline bool        __b(_Ty&& value) noexcept { return static_cast<bool>(value); }

		/// <summary>
		/// ������תΪuint8_t��ֵ
		/// </summary>
		/// <param name="value">����������ֵ</param>
		/// <returns>uint8_t��ֵ</returns>
		template <typename _Ty>
		inline uint8_t     __u8(_Ty&& value) noexcept { return static_cast<uint8_t>(value); }

		/// <summary>
		/// ������תΪ��������
		/// </summary>
		/// <param name="value">����������ֵ</param>
		/// <returns>��������</returns>
		template <typename _Ty>
		inline int         __n(_Ty&& value) noexcept { return static_cast<int>(value); }

		/// <summary>
		/// ������תΪ����������
		/// </summary>
		/// <param name="value">����������ֵ</param>
		/// <returns>����������</returns>
		template <typename _Ty>
		inline int64_t     __n64(_Ty&& value) noexcept { return static_cast<int64_t>(value); }

		/// <summary>
		/// ������תΪstd::size_t������
		/// </summary>
		/// <param name="value">����������ֵ</param>
		/// <returns>std::size_t������</returns>
		template <typename _Ty>
		inline std::size_t __sz(_Ty&& value) noexcept { return static_cast<std::size_t>(value); }

		/// <summary>
		/// ������תΪ�����ȸ�����������
		/// </summary>
		/// <param name="value">����������ֵ</param>
		/// <returns>�����ȸ���������</returns>
		template <typename _Ty>
		inline float       __f(_Ty&& value) noexcept { return static_cast<float>(value); }

		/// <summary>
		/// ������תΪ˫���ȸ�����������
		/// </summary>
		/// <param name="value">����������ֵ</param>
		/// <returns>˫���ȸ���������</returns>
		template <typename _Ty>
		inline double      __d(_Ty&& value) noexcept { return static_cast<double>(value); }

		/// <summary>
		/// �����ڴ�
		/// </summary>
		/// <typeparam name="_Ty">�ڴ�����</typeparam>
		/// <param name="var">��Ӧ���͵ı���</param>
		template <typename _Ty>
		inline void        __zero_mem(_Ty&& var) noexcept { memset(&var, 0, sizeof(_Ty)); }

		/// <summary>
		/// �����������̶�����תΪ�ַ��������㲹0
		/// </summary>
		/// <param name="value">��ת�����ַ���</param>
		/// <param name="precision">С������λ��</param>
		/// <returns>�̶����ȵ��ַ���</returns>
		template <typename _Ty>
		std::string        __to_string_with_precision(_Ty&& value, const unsigned int precision = 2);

		/// <summary>
		/// ��ȡ�Ժ���Ϊ��λ��UTCʱ��
		/// </summary>
		/// <returns>�Ժ���Ϊ��λ��UTCʱ��</returns>
		inline int64_t     __utc() noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

		/// <summary>
		/// ��ǰ�߳�����Ϊ��λ��ʱ
		/// </summary>
		/// <param name="val">�ӳٵ�����</param>
		inline void        __sleep_seconds(const int val) noexcept { std::this_thread::sleep_for(std::chrono::seconds(val)); }

		/// <summary>
		/// ��ǰ�߳��Ժ���Ϊ��λ��ʱ
		/// </summary>
		/// <param name="val">�ӳٵĺ�����</param>
		inline void        __sleep_milliseconds(const int val) noexcept { std::this_thread::sleep_for(std::chrono::milliseconds(val)); }

		/// <summary>
		/// ��ȡ�������UUID
		/// </summary>
		/// <returns>�������UUID</returns>
		std::string        __get_uuid() noexcept;

	}

}


/// <summary>
/// �����������̶�����תΪ�ַ��������㲹0
/// </summary>
/// <param name="value">��ת�����ַ���</param>
/// <param name="precision">С������λ��</param>
/// <returns>�̶����ȵ��ַ���</returns>
template<typename _Ty>
std::string vsnc::utils::__to_string_with_precision(_Ty&& value, const unsigned int precision)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << value;
	return ss.str();
}


#endif // !__VSNC_UTILS_UTILS_H__

