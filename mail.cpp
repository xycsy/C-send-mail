#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>
#include <libesmtp.h>
#include "clientlog.h"
#include "clientbasefunc.h"

int send_mail(char buf[1024]) {
	//SMTP会话
    smtp_session_t session;
	//SMTP消息结构
    smtp_message_t message;
    struct sigaction sa;
	//SMTP状态
    const smtp_status_t *status;
	//发送缓冲区
	//FILE *fp;

	// 创建SMTP会话
    if((session = smtp_create_session()) == NULL) {
		TRACELOG( LOG_INFO, "创建SMTP会话失败" );
        return 1;
    }
	// 从SMTP会话中接受消息，判断是否成功
    if((message = smtp_add_message(session)) == NULL) {
		TRACELOG( LOG_INFO, "从SMTP接收数据失败" );
        return 1;
    }

    sa.sa_handler = SIG_IGN;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction (SIGPIPE, &sa, NULL);

	//设置SMTP服务器
    smtp_set_server (session, "127.0.0.1:25");
	//发件人
    smtp_set_reverse_path (message, "test@test.com");
	//收件人
    smtp_set_header (message, "To", NULL, NULL);
	//主题
    smtp_set_header (message, "Subject", "TFS报警信息---系统邮件,请勿回复");
	//邮件设置 ??
    smtp_set_header_option (message, "Subject", Hdr_OVERRIDE, 1);
/*
	//发送文件
    if ((fp = fopen ("test-mail.eml", "r")) == NULL) {
        fprintf (stderr, "can't open mail file: %s\n", strerror (errno));
        return (1);
    }
    smtp_set_message_fp(message, fp);
*/
	//发送文本
	smtp_set_message_str(message, buf);
	//收件人
	int i = 0;
	while(*g_mail_list[i++]) {
    	smtp_add_recipient(message, g_mail_list[i-1]);
	}
	
	//发送邮件
    if (!smtp_start_session(session)) {
		TRACELOG( LOG_INFO, "发送邮件失败" );
    } else {
		//获取发送状态
        status = smtp_message_transfer_status(message);
        TRACELOG(LOG_DEBUG, "%d %s", status->code, (status->text != NULL) ? status->text : "\n");
    }
	//结束SMTP会话
    smtp_destroy_session(session);
/*
    if(fp != NULL) {
        fclose(fp);
    }
*/
    return 0;
}

void check_tfs_service() {
	NULL;
}

void *mail_pthread(void*) {
	char *buf = "\r\n邮件内容\r\n";
	while(1) {
		check_tfs_service();
		send_mail(buf);
		printf("send mail!!\n");
		sleep(15);
	}
}
