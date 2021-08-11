#ifndef __CLIENT_H__
#define __CLIENT_H__


#ifdef __cplusplus

#include <memory>
#include <string>


#include <vsnc_utils/memory.h>

#endif // __cplusplus


#include <stdint.h>


#include "defines.h"


#ifdef _WIN32
#ifndef __SSIZE_T__
#define __SSIZE_T__
#ifdef __cplusplus
using ssize_t = int;
#else
typedef int ssize_t;
#endif // __cplusplus
#endif // !__SSIZE_T__
#else
#include <sys/types.h>
#endif // _WIN32


#ifdef __cplusplus
namespace vsnc
{

	namespace p2p
	{
#endif // __cplusplus


		/// <summary>客户端状态</summary>
		typedef enum
#ifdef __cplusplus
			class
#endif
			vsnc_p2p_state
#ifdef __cplusplus
			: int8_t
#endif
		{
			OFFLINE    = -2, // 离线
			FREE       =  0, // 在线但空闲
			REQUESTING =  1, // 请求连接
			CONNECTING =  2, // 尝试建立连接
			CONNECTED  =  3, // 已连接
		} vsnc_p2p_state;


#ifdef __cplusplus
		/// <summary>发送端实现类的前向声明</summary>
		class ClientImpl;


		/// <summary>
		/// 客户端
		/// </summary>
		class __VSNC_P2P_PORT Client
		{
			/// <summary>客户端实现类类型</summary>
			using __impl_type = std::unique_ptr<ClientImpl>;

		public:

			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="seqno">客户端序列号</param>
			/// <param name="srv_ip">服务器IP</param>
			/// <param name="srv_port">服务器端口</param>
			/// <param name="port">监听的端口号</param>
			Client(const uint64_t seqno, const std::string& srv_ip, const uint16_t srv_port, const uint16_t port = 0);

			/// <summary>
			/// 析构函数
			/// </summary>
			~Client() noexcept;

			/// <summary>
			/// 获取客户端状态
			/// </summary>
			/// <returns>客户端状态</returns>
			vsnc_p2p_state GetState() const noexcept;

			/// <summary>
			/// <para>向指定终端发起连接</para>
			/// <para>这是一个非阻塞的异步函数，调用后客户端状态变为REQUESTING，用户随后应循环调用GetState函数获取实时状态</para>
			/// <para>后续若状态变为FREE则代表连接失败，变为CONNECTED则代表连接成功（在此之前会经历CONNECTING），超时时间固定为5秒</para>
			/// </summary>
			/// <param name="seqno">对端序列号</param>
			void           Connect(const uint64_t seqno) noexcept;

			/// <summary>
			/// <para>关闭客户端，在程序结束时使用，打破Receive函数的阻塞状态</para>
			/// <para>析构函数调用了该函数，所以如果该类的对象在堆中，则可无需手动调用该函数，它将在手动释放内存资源时被自动调用</para>
			/// </summary>
			void           Close() noexcept;

			/// <summary>
			/// 发送数据
			/// </summary>
			/// <param name="mem">待发送的数据</param>
			/// <param name="ts">数据时间戳</param>
			/// <returns>成功返回发送的字节数，失败返回-1</returns>
			ssize_t        Send(const utils::Memory<char>& mem, const int64_t ts) const noexcept;

			/// <summary>
			/// 接收数据
			/// </summary>
			/// <param name="mem">接收缓存区</param>
			/// <param name="ts">接收到的数据时间戳</param>
			/// <param name="timeout">以毫秒为单位的超时时间，小于0则阻塞</param>
			/// <returns>成功返回接收到的字节数，失败返回-1，超时返回-2</returns>
			ssize_t        Receive(utils::Memory<char>& mem, int64_t& ts, const int64_t timeout = -1) noexcept;

		private:

			/// <summary>客户端实现类</summary>
			__impl_type    m_upImpl;
		};


	}

}
#endif // __cplusplus


#ifdef _WIN32
#define __CDECL __cdecl
#else
#define __CDECL /*__attribute__((cdecl))*/
#endif // _WIN32


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	/// <summary>P2P客户端句柄</summary>
	typedef void* vsnc_p2p_handler;

	/// <summary>
	/// <para>创建P2P客户端句柄</para>
	/// <para>程序结束时需调用p2p_destroy_handler函数释放该句柄</para>
	/// </summary>
	/// <param name="seqno">客户端序列号</param>
	/// <param name="srv_ip">服务器IP</param>
	/// <param name="srv_port">服务器端口</param>
	/// <param name="port">监听的端口号</param>
	/// <returns>创建好的P2P客户端句柄</returns>
	__VSNC_P2P_PORT vsnc_p2p_handler __CDECL vsnc_p2p_create_handler(const uint64_t seqno, const char* srv_ip, const uint16_t srv_port, const uint16_t port) ;

	/// <summary>
	/// 获取P2P客户端状态
	/// </summary>
	/// <param name="hdl">P2P客户端句柄</param>
	/// <returns>P2P客户端的状态</returns>
	__VSNC_P2P_PORT
#ifdef __cplusplus
		vsnc::p2p::
#endif // __cplusplus
		vsnc_p2p_state               __CDECL vsnc_p2p_get_state(const vsnc_p2p_handler hdl);

	/// <summary>
	/// <para>向指定终端发起连接</para>
	/// <para>这是一个非阻塞的异步函数，调用后客户端状态变为REQUESTING，用户随后应循环调用GetState函数获取实时状态</para>
	/// <para>后续若状态变为FREE则代表连接失败，变为CONNECTED则代表连接成功（在此之前会经历CONNECTING），超时时间固定为5秒</para>
	/// </summary>
	/// <param name="hdl">P2P客户端句柄</param>
	/// <param name="seqno">对端序列号</param>
	__VSNC_P2P_PORT void             __CDECL vsnc_p2p_connect(vsnc_p2p_handler hdl, const uint64_t seqno);

	/// <summary>
	/// 发送数据
	/// </summary>
	/// <param name="hdl">P2P客户端句柄</param>
	/// <param name="head">数据头指针</param>
	/// <param name="len">数据长度</param>
	/// <param name="ts">时间戳</param>
	/// <returns>成功返回发送的字节数，失败返回-1</returns>
	__VSNC_P2P_PORT ssize_t          __CDECL vsnc_p2p_send(const vsnc_p2p_handler hdl, const char* const head, const size_t len, const int64_t ts);

	/// <summary>
	/// 接收数据
	/// </summary>
	/// <param name="hdl">P2P客户端句柄</param>
	/// <param name="head">数据缓存区头指针</param>
	/// <param name="len">数据缓存区长度</param>
	/// <param name="ts">时间戳</param>
	/// <param name="timeout">以毫秒为单位的超时时间，小于0则阻塞</param>
	/// <returns>成功返回接收到的字节数，失败返回-1，超时返回-2</returns>
	__VSNC_P2P_PORT ssize_t          __CDECL vsnc_p2p_receive(vsnc_p2p_handler hdl, char* const head, const size_t len, int64_t* ts, const int64_t timeout);

	/// <summary>
	/// 释放句柄资源
	/// </summary>
	/// <param name="hdl">待释放的句柄</param>
	__VSNC_P2P_PORT void             __CDECL vsnc_p2p_destroy_handler(vsnc_p2p_handler hdl);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __CLIENT_H__

