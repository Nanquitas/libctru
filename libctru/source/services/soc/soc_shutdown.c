#include "soc_common.h"
#include <errno.h>
#include <sys/socket.h>
#include <3ds/ipc.h>

int shutdown(int sockfd, int shutdown_type)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	sockfd = soc_get_fd(sockfd);
	if(sockfd < 0) {
		errno = -sockfd;
		return -1;
	}

	cmdbuf[0] = IPC_MakeHeader(0xC,2,2); // 0xC0082
	cmdbuf[1] = (u32)sockfd;
	cmdbuf[2] = (u32)shutdown_type;
	cmdbuf[3] = IPC_Desc_CurProcessId();

	ret = svcSendSyncRequest(SOCU_handle);
	if(ret != 0) {
		errno = SYNC_ERROR;
		return ret;
	}

	ret = (int)cmdbuf[1];
	if(ret == 0)
		ret = _net_convert_error(cmdbuf[2]);

	if(ret < 0) {
		errno = -ret;
		return -1;
	}

	return 0;
}
