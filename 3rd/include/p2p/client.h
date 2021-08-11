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


		/// <summary>�ͻ���״̬</summary>
		typedef enum
#ifdef __cplusplus
			class
#endif
			vsnc_p2p_state
#ifdef __cplusplus
			: int8_t
#endif
		{
			OFFLINE    = -2, // ����
			FREE       =  0, // ���ߵ�����
			REQUESTING =  1, // ��������
			CONNECTING =  2, // ���Խ�������
			CONNECTED  =  3, // ������
		} vsnc_p2p_state;


#ifdef __cplusplus
		/// <summary>���Ͷ�ʵ�����ǰ������</summary>
		class ClientImpl;


		/// <summary>
		/// �ͻ���
		/// </summary>
		class __VSNC_P2P_PORT Client
		{
			/// <summary>�ͻ���ʵ��������</summary>
			using __impl_type = std::unique_ptr<ClientImpl>;

		public:

			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="seqno">�ͻ������к�</param>
			/// <param name="srv_ip">������IP</param>
			/// <param name="srv_port">�������˿�</param>
			/// <param name="port">�����Ķ˿ں�</param>
			Client(const uint64_t seqno, const std::string& srv_ip, const uint16_t srv_port, const uint16_t port = 0);

			/// <summary>
			/// ��������
			/// </summary>
			~Client() noexcept;

			/// <summary>
			/// ��ȡ�ͻ���״̬
			/// </summary>
			/// <returns>�ͻ���״̬</returns>
			vsnc_p2p_state GetState() const noexcept;

			/// <summary>
			/// <para>��ָ���ն˷�������</para>
			/// <para>����һ�����������첽���������ú�ͻ���״̬��ΪREQUESTING���û����Ӧѭ������GetState������ȡʵʱ״̬</para>
			/// <para>������״̬��ΪFREE���������ʧ�ܣ���ΪCONNECTED��������ӳɹ����ڴ�֮ǰ�ᾭ��CONNECTING������ʱʱ��̶�Ϊ5��</para>
			/// </summary>
			/// <param name="seqno">�Զ����к�</param>
			void           Connect(const uint64_t seqno) noexcept;

			/// <summary>
			/// <para>�رտͻ��ˣ��ڳ������ʱʹ�ã�����Receive����������״̬</para>
			/// <para>�������������˸ú����������������Ķ����ڶ��У���������ֶ����øú������������ֶ��ͷ��ڴ���Դʱ���Զ�����</para>
			/// </summary>
			void           Close() noexcept;

			/// <summary>
			/// ��������
			/// </summary>
			/// <param name="mem">�����͵�����</param>
			/// <param name="ts">����ʱ���</param>
			/// <returns>�ɹ����ط��͵��ֽ�����ʧ�ܷ���-1</returns>
			ssize_t        Send(const utils::Memory<char>& mem, const int64_t ts) const noexcept;

			/// <summary>
			/// ��������
			/// </summary>
			/// <param name="mem">���ջ�����</param>
			/// <param name="ts">���յ�������ʱ���</param>
			/// <param name="timeout">�Ժ���Ϊ��λ�ĳ�ʱʱ�䣬С��0������</param>
			/// <returns>�ɹ����ؽ��յ����ֽ�����ʧ�ܷ���-1����ʱ����-2</returns>
			ssize_t        Receive(utils::Memory<char>& mem, int64_t& ts, const int64_t timeout = -1) noexcept;

		private:

			/// <summary>�ͻ���ʵ����</summary>
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

	/// <summary>P2P�ͻ��˾��</summary>
	typedef void* vsnc_p2p_handler;

	/// <summary>
	/// <para>����P2P�ͻ��˾��</para>
	/// <para>�������ʱ�����p2p_destroy_handler�����ͷŸþ��</para>
	/// </summary>
	/// <param name="seqno">�ͻ������к�</param>
	/// <param name="srv_ip">������IP</param>
	/// <param name="srv_port">�������˿�</param>
	/// <param name="port">�����Ķ˿ں�</param>
	/// <returns>�����õ�P2P�ͻ��˾��</returns>
	__VSNC_P2P_PORT vsnc_p2p_handler __CDECL vsnc_p2p_create_handler(const uint64_t seqno, const char* srv_ip, const uint16_t srv_port, const uint16_t port) ;

	/// <summary>
	/// ��ȡP2P�ͻ���״̬
	/// </summary>
	/// <param name="hdl">P2P�ͻ��˾��</param>
	/// <returns>P2P�ͻ��˵�״̬</returns>
	__VSNC_P2P_PORT
#ifdef __cplusplus
		vsnc::p2p::
#endif // __cplusplus
		vsnc_p2p_state               __CDECL vsnc_p2p_get_state(const vsnc_p2p_handler hdl);

	/// <summary>
	/// <para>��ָ���ն˷�������</para>
	/// <para>����һ�����������첽���������ú�ͻ���״̬��ΪREQUESTING���û����Ӧѭ������GetState������ȡʵʱ״̬</para>
	/// <para>������״̬��ΪFREE���������ʧ�ܣ���ΪCONNECTED��������ӳɹ����ڴ�֮ǰ�ᾭ��CONNECTING������ʱʱ��̶�Ϊ5��</para>
	/// </summary>
	/// <param name="hdl">P2P�ͻ��˾��</param>
	/// <param name="seqno">�Զ����к�</param>
	__VSNC_P2P_PORT void             __CDECL vsnc_p2p_connect(vsnc_p2p_handler hdl, const uint64_t seqno);

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="hdl">P2P�ͻ��˾��</param>
	/// <param name="head">����ͷָ��</param>
	/// <param name="len">���ݳ���</param>
	/// <param name="ts">ʱ���</param>
	/// <returns>�ɹ����ط��͵��ֽ�����ʧ�ܷ���-1</returns>
	__VSNC_P2P_PORT ssize_t          __CDECL vsnc_p2p_send(const vsnc_p2p_handler hdl, const char* const head, const size_t len, const int64_t ts);

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="hdl">P2P�ͻ��˾��</param>
	/// <param name="head">���ݻ�����ͷָ��</param>
	/// <param name="len">���ݻ���������</param>
	/// <param name="ts">ʱ���</param>
	/// <param name="timeout">�Ժ���Ϊ��λ�ĳ�ʱʱ�䣬С��0������</param>
	/// <returns>�ɹ����ؽ��յ����ֽ�����ʧ�ܷ���-1����ʱ����-2</returns>
	__VSNC_P2P_PORT ssize_t          __CDECL vsnc_p2p_receive(vsnc_p2p_handler hdl, char* const head, const size_t len, int64_t* ts, const int64_t timeout);

	/// <summary>
	/// �ͷž����Դ
	/// </summary>
	/// <param name="hdl">���ͷŵľ��</param>
	__VSNC_P2P_PORT void             __CDECL vsnc_p2p_destroy_handler(vsnc_p2p_handler hdl);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __CLIENT_H__

