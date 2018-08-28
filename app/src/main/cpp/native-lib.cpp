#include <jni.h>
#include <string>
#include "native-lib.h"


//通过文件读写，进行socket通信
char const *PATH = "/data/data/com.example.lixuecheng.socketprocess/my.sock";
int m_child;
const char *uid;
void child_listen_msg();

extern "C" JNIEXPORT jstring

JNICALL
Java_com_example_lixuecheng_socketprocess_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_lixuecheng_socketprocess_WatchDog_creatSocket(JNIEnv *env, jobject instance,
                                                               jstring uid_) {
    uid = env->GetStringUTFChars(uid_, 0);

    //开启双进程
    // TODO
    pid_t pid = fork();
    if (pid < 0) {
        //开启失败
    } else if (pid == 0) {
        //子进程 守护进程  WTF执行两遍？
        child_do_work();
    } else if (pid > 0) {
        //父进程
    }

    env->ReleaseStringUTFChars(uid_, uid);
}


void child_do_work() {
    //开启socket
    if (child_create_channel()) {
        child_listen_msg();
    }
}

/**
 *
 * 创建服务端socket
 * @return
 */
int child_create_channel() {
    int listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    //socket可以跨进程，文件端口读写  linux文件系统  ip+端口 实际上指明文件
    unlink(PATH);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_LOCAL;
//    addr.sun_data = PATH; 不能够直接赋值
    strcpy(addr.sun_path, PATH);

    if (bind(listenfd, (const sockaddr *) (&addr), sizeof(sockaddr_un)) < 0) {
        LOGE("绑定错误");
    }
    int connfd = 0;
    //能够同时连接五个客户端
    listen(listenfd, 5);
    //死循环保证连接成功
    while (1) {
        //返回客户端地址 accept是阻塞式函数
        if((connfd = accept(listenfd,NULL, NULL)) < 0) {
            //有两种情况
            if(errno == EINTR) {
                continue;
            } else {
                LOGE("读取错误");
                return 0;
            }
        }
        m_child = connfd;
        LOGE("APK 父进程连接上了 %d", m_child);
        break;
    }
    return 1;
}

/**
 * 服务端读取信息
 * 客户端
 */
void child_listen_msg() {
    fd_set rfds;
    struct timeval timeval1{3,0};
    while(1) {
        //清空内容
        FD_ZERO(&rfds);
        FD_SET(m_child, &rfds);
        int r = select(m_child+1, &rfds, NULL, NULL,&timeval1);
//        LOGE("读取消息前 %d", r);
        if (r>0) {
            //缓冲区
            char pkg[256] = {0};
            //保证所读取的信息来自指定的apk客户端
            if(FD_ISSET(m_child, &rfds)) {
                //阻塞式函数
                LOGE("读取消息后 %d", r);
                read(m_child,pkg, sizeof(pkg));
                LOGE("在这里===%s", uid);
                //不在阻塞，开启服务
                execlp("am","am","startservice", "--user", uid,
                "com.example.lixuecheng.socketprocess/com.example.lixuecheng.socketprocess.ProcessService",
                       (char *)NULL);
                break;
            }
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_lixuecheng_socketprocess_WatchDog_connectMonitor(JNIEnv *env, jobject instance) {

    // TODO
    //客户端进程调用
    int socked;
    struct sockaddr_un addr;
    while(1) {
        LOGE("客户端父进程开始连接");
        socked = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (socked < 0) {
            LOGE("连接失败");
            return;
        }
        memset(&addr, 0, sizeof(sockaddr_un));
        addr.sun_family = AF_LOCAL;
//    addr.sun_data = PATH; 不能够直接赋值
        strcpy(addr.sun_path, PATH);
        if(connect(socked, (const sockaddr *)(&addr), sizeof(sockaddr_un)) < 0){
            LOGE("连接失败");
            close(socked);
            sleep(1);
            continue;
        }
        LOGE("连接成功");
        break;
    }

}