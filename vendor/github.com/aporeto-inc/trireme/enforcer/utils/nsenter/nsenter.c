// +build linux !darwin

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define STRBUF_SIZE     128
void nsexec(void) {

  int fd = 0;
  char path[STRBUF_SIZE];
  char msg[STRBUF_SIZE];
  char mountpoint[STRBUF_SIZE] = {0};
  char *str = getenv("CONTAINER_PID");
  char *proc_mountpoint = getenv("APORETO_ENV_PROC_MOUNTPOINT");
  if(str == NULL){
    // We are not running as remote enforcer
    setenv("APORETO_ENV_NSENTER_LOGS", "no container pid", 1);
    return;
  }
  if(proc_mountpoint == NULL){
    strncpy(mountpoint, "/proc", strlen("/proc"));
  }else{
    strncpy(mountpoint, proc_mountpoint, STRBUF_SIZE);
  }
  // Setup proc symlink
  snprintf(path, sizeof(path), "%s/%s/ns/net", mountpoint, str);

  // Setup FD to symlink
  fd = open(path, O_RDONLY);
  if(fd < 0) {
    snprintf(msg, sizeof(msg), "path:%s fd:%d", path, fd);
    setenv("APORETO_ENV_NSENTER_ERROR_STATE",strerror(-ENOENT), 1);
    setenv("APORETO_ENV_NSENTER_LOGS", path, 1);
    return;
  }

  // Set namespace
  int retval = setns(fd,0);
  snprintf(msg, sizeof(msg), "path:%s fd:%d retval:%d", path, fd, retval);
  setenv("APORETO_ENV_NSENTER_LOGS",msg,1);
  if(retval < 0){
    setenv("APORET_ENV_NSENTER_ERROR_STATE",strerror(errno),1);
  }
}
