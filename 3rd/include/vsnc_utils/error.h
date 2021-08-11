#ifndef __VSNC_UTILS_ERROR_H__
#define __VSNC_UTILS_ERROR_H__


#include <string>
#include <functional>
#include <sstream>
#include <iostream>
#include <stdexcept>


#include <cstring>
#include <cerrno>


namespace vsnc
{

	namespace utils
	{


		/// <summary>
		/// <para>��ȡ��ʽ����Ĵ�����Ϣ</para>
		/// <para>��ʽ�������Ϣ��ʽ���ң�[err]file:line: msg</para>
		/// </summary>
		/// <param name="path">���������ļ�</param>
		/// <param name="line">������������</param>
		/// <param name="msg">������Ϣ�ַ���</param>
		/// <param name="err">������</param>
		/// <returns>��ʽ����Ĵ�����Ϣ�ַ���</returns>
		template <typename _Err = int>
		std::string __fmt_err(const std::string path, const int line, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// ������Ϊ��ʱ�׳��쳣
		/// </summary>
		/// <param name="path">�ж������ļ�</param>
		/// <param name="line">�ж���������</param>
		/// <param name="cond">�ж�����</param>
		/// <param name="msg">������Ϣ</param>
		/// <param name="err">������</param>
		template <typename _Err = int>
		void        __throw_if(const std::string path, const int line, const bool cond, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// ��ֵΪ0ʱ�׳��쳣
		/// </summary>
		/// <typeparam name="_Val">ֵ������</typeparam>
		/// <param name="path">�ж������ļ�</param>
		/// <param name="line">�ж���������</param>
		/// <param name="val">���жϵı���</param>
		/// <param name="name">���жϵı�����</param>
		template <typename _Val>
		void        __throw_if_zero(const std::string path, const int line, _Val&& val, const std::string name);

		/// <summary>
		/// ��ָ��Ϊ��ʱ�׳��쳣
		/// </summary>
		/// <param name="path">ָ�������ļ�</param>
		/// <param name="line">ָ����������</param>
		/// <param name="ptr">���жϵ�ָ��</param>
		/// <param name="name">���жϵ�ָ����</param>
		template <typename _Ptr>
		void        __throw_if_nullptr(const std::string path, const int line, _Ptr&& ptr, const std::string name);

		/// <summary>
		/// ��������С��0ʱ�׳��쳣
		/// </summary>
		/// <param name="path">�ж������ļ�</param>
		/// <param name="line">�ж���������</param>
		/// <param name="err">������</param>
		/// <param name="name">���жϱ�����</param>
		template <typename _Err>
		void        __throw_if_negative(const std::string path, const int line, const _Err err, const std::string name);

		/// <summary>���������贫���ļ���������������ĳЩ��Ϣ</summary>
#define fmt_err(msg)           vsnc::utils::__fmt_err           (__FILE__, __LINE__, msg)
#define throw_if(cond)         vsnc::utils::__throw_if          (__FILE__, __LINE__, cond, #cond)
#define throw_if_zero(val)     vsnc::utils::__throw_if_zero     (__FILE__, __LINE__, val, #val)
#define throw_if_nullptr(ptr)  vsnc::utils::__throw_if_nullptr  (__FILE__, __LINE__, ptr, #ptr)
#define throw_if_negative(err) vsnc::utils::__throw_if_negative (__FILE__, __LINE__, err, #err)

		/// <summary>
		/// <para>һ����ʽ��������Ϣ���࣬����ͨ���������ȡ������Ϣ</para>
		/// <para>�û���Ҫ����һ��ͨ���������ȡ������Ϣ�ĺ���</para>
		/// <para>�������_TyΪ�������ͣ�����Ϊ��������ʱ��Ƶ��ʹ�ÿ��������������</para>
		/// </summary>
		template <typename _Ty>
		class FormattedError
		{
		public:

			/// <summary>����������</summary>
			using err_type = _Ty;
			/// <summary>ͨ���������ȡ������Ϣ�ĺ�������</summary>
			using func_type = std::function<std::string(const err_type)>;

		public:

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="func">ͨ���������ȡ������Ϣ�ĺ���</param>
			explicit FormattedError(func_type func) : m_pfnErrStr(func) {}

			/// <summary>
			/// <para>��ȡ��ʽ����Ĵ�����Ϣ</para>
			/// <para>��ʽ�������Ϣ��ʽ���ң�[errcode]file:line: msg</para>
			/// </summary>
			/// <param name="path">���������ļ�</param>
			/// <param name="line">������������</param>
			/// <param name="err">������</param>
			/// <returns>��ʽ����Ĵ�����Ϣ�ַ���</returns>
			std::string operator()(const std::string path, const int line, const err_type err)
			{
				return __fmt_err(path, line, m_pfnErrStr(err), err);
			}

		private:

			/// <summary>ͨ���������ȡ������Ϣ�ĺ���</summary>
			func_type m_pfnErrStr;
		};


	}

}


/// <summary>
/// <para>��ȡ��ʽ����Ĵ�����Ϣ</para>
/// <para>��ʽ�������Ϣ��ʽ���ң�[err]file:line: msg</para>
/// </summary>
/// <param name="path">���������ļ�</param>
/// <param name="line">������������</param>
/// <param name="msg">������Ϣ�ַ���</param>
/// <param name="err">������</param>
/// <returns>��ʽ����Ĵ�����Ϣ�ַ���</returns>
template<typename _Err>
inline std::string vsnc::utils::__fmt_err(const std::string path, const int line, const std::string msg, const _Err& err)
{
	auto file_name = [](const char* const path) -> decltype(auto) {
#ifdef _WIN32
		return (strrchr((path), '\\') ? (strrchr((path), '\\') + 1) : (path));
#else
		return (strrchr((path), '/') ? (strrchr((path), '/') + 1) : (path));
#endif // _WIN32
	};

	std::ostringstream os;
	os << "[" << err << "]" << file_name(path.c_str()) << ":" << line << ": " << msg;
#ifdef DEBUG
	std::cerr << os.str() << std::endl;
#endif // DEBUG
	return os.str();
}


/// <summary>
/// ������Ϊ��ʱ�׳��쳣
/// </summary>
/// <param name="path">�ж������ļ�</param>
/// <param name="line">�ж���������</param>
/// <param name="cond">�ж�����</param>
/// <param name="msg">������Ϣ</param>
/// <param name="err">������</param>
template<typename _Err>
void vsnc::utils::__throw_if(const std::string path, const int line, const bool cond, const std::string msg, const _Err& err)
{
	if (cond) {
		throw std::runtime_error(__fmt_err(path, line, msg, err));
	}
}


/// <summary>
/// ��ֵΪ0ʱ�׳��쳣
/// </summary>
/// <typeparam name="_Val">ֵ������</typeparam>
/// <param name="path">�ж������ļ�</param>
/// <param name="line">�ж���������</param>
/// <param name="val">���жϵı���</param>
/// <param name="name">���жϵı�����</param>
template<typename _Val>
void vsnc::utils::__throw_if_zero(const std::string path, const int line, _Val&& val, const std::string name)
{
	__throw_if(path, line, (0 == val), (name + " is zero"));
}


/// <summary>
/// ��ָ��Ϊ��ʱ�׳��쳣
/// </summary>
/// <param name="path">ָ�������ļ�</param>
/// <param name="line">ָ����������</param>
/// <param name="ptr">���жϵ�ָ��</param>
/// <param name="name">���жϵ�ָ����</param>
template<typename _Ptr>
inline void vsnc::utils::__throw_if_nullptr(const std::string path, const int line, _Ptr&& ptr, const std::string name)
{
	__throw_if(path, line, (nullptr == ptr), (name + " is null pointer"));
}


/// <summary>
/// ��������С��0ʱ�׳��쳣
/// </summary>
/// <param name="path">�ж������ļ�</param>
/// <param name="line">�ж���������</param>
/// <param name="err">������</param>
/// <param name="name">���жϱ�����</param>
template<typename _Err>
void vsnc::utils::__throw_if_negative(const std::string path, const int line, const _Err err, const std::string name)
{
	__throw_if(path, line, (0 > err), name + " is negative", err);
}


#endif // !__VSNC_UTILS_ERROR_H__

