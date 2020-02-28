#include "IpcService.h"
#include <uv.h>
#include <iostream>
#include "IpcChannel.h"

namespace IpcService 
{
	typedef struct 
	{
		uv_write_t req;
		uv_buf_t buf;
	} write_req_t;

	uv_loop_t ipc_loop;
	uv_stream_t* stream = nullptr;
    
	void MemoryAlloc(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
	{
		buf->base = (char*)malloc(suggested_size);
		buf->len = suggested_size;
	}

	void OnClose(uv_handle_t * peer)
	{
		free(peer);
	}

	void AfterShutdown(uv_shutdown_t * req, int status)
	{
		uv_close((uv_handle_t*)req->handle, OnClose);
		free(req);
	}

	void AfterWrite(uv_write_t * req, int status)
	{
		write_req_t *wr;
		wr = (write_req_t*)req;
//		free(wr->buf.base);
		free(wr);
		if (0 == status)
			return;
		std::cout << "uv_write_error: " << uv_err_name(status) << " " << uv_strerror(status) << std::endl;
	}

	void Write(const std::string& message)
	{
//        std::cout<<"************************************\n";
//        std::cout<<"write threadID: "<<std::this_thread::get_id()<<"write content: "<<message<<std::endl;
//        std::cout<<"************************************\n";
		write_req_t *wr;
		wr = (write_req_t*)malloc(sizeof *wr);
		wr->buf = uv_buf_init(const_cast<char*>(message.c_str()), (unsigned int)message.length());
		if (uv_write(&wr->req, stream, &wr->buf, 1, AfterWrite))
		{
			std::cout << "write failed\n";
		}
	}
    
	void AfterRead(uv_stream_t * handle, ssize_t nread, const uv_buf_t * buf)
	{
		if (nread < 0)
		{
            uv_shutdown_t *sreq;
			free(buf->base);
			sreq = (uv_shutdown_t*)malloc(sizeof* sreq);
			uv_shutdown(sreq, handle, AfterShutdown);
			return;
		}
		if (0 == nread)
		{
			/*Everything OK, but nothing read*/
			free(buf->base);
			return;
		}
		IpcChannel::GetInstance().Read(std::string(buf->base, nread));
//		Write(std::string(buf->base, buf->len));
	}

	void OnConnection(uv_stream_t * server, int status)
	{
		if (status != 0)
		{
			std::cout << "Connect error " << uv_err_name(status) << std::endl;
			return;
		}
//		uv_stream_t *stream;
		int r;
		stream = (uv_stream_t*)malloc(sizeof(uv_pipe_t));
		r = uv_pipe_init(&ipc_loop, (uv_pipe_t*)stream, 0);
		//		stream->data = server;
		r = uv_accept(server, stream);
		if (r)
		{
			std::cout << "accept error" << uv_strerror(r) << std::endl;
		}
		r = uv_read_start(stream, MemoryAlloc, AfterRead);
	}
    
    void StopLoop(uv_async_t* handle)
    {
        std::cout<<"loop count "<<uv_loop_alive(&ipc_loop)<<std::endl;
        uv_loop_close(&ipc_loop);
        uv_stop(&ipc_loop);
        if(!uv_is_closing((uv_handle_t*)stream))
            uv_close((uv_handle_t*)stream, nullptr);
        std::cout<<"loop count "<<uv_loop_alive(&ipc_loop)<<std::endl;
    }

	void StartIpcService(const std::string& pipe_name , void* async)
	{
        std::cout<<"ipcStart threadID: "<<std::this_thread::get_id()<<std::endl;
        uv_pipe_t pipe_server;
		uv_loop_init(&ipc_loop);
        std::string tmp_pipe_name = "";
#ifdef _WIN32
        tmp_pipe_name ="\\\\?\\pipe\\" + pipe_name;
#else
        tmp_pipe_name = pipe_name;
#endif
        
        uv_async_init(&ipc_loop, (uv_async_t*)async, StopLoop);
        
		std::cout << "start service\n";
		int r;
		r = uv_pipe_init(&ipc_loop, &pipe_server, 0);
		if (r)
		{
			std::cout << "uv_pipe_init " << uv_strerror(r) << "\n";
			return;
		}
		r = uv_pipe_bind(&pipe_server, tmp_pipe_name.c_str());
		if (r)
		{
			std::cout << "uv_pipe_bind: " << uv_strerror(r) << std::endl;
			return;
		}
		r = uv_listen((uv_stream_t*)&pipe_server, 1, OnConnection);
		if (r)
		{
			std::cout << "listen error " << uv_err_name(r) << std::endl;
		}
		uv_run(&ipc_loop, UV_RUN_DEFAULT);
	}

}
