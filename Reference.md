[父进程查看子进程状态](https://www.cnblogs.com/gwyy/p/8611398.html)
```c++
void f()
{
	/*
	 * 当子进程正常退出的时候， wait返回子进程的 pid， 并且 WIFEXITED(status) 为真， WEXITSTATUS(status)获得返回码。
	 * 当程序异常退出的时候， WIFSIGNALED(status) 为真，可用 WTERMSIG(status) 返回相应的信号代码。
	 */
	if (WIFEXITED(status))
		printf("child exited normal exit status=%d\n", WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("child exited abnormal signal number=%d\n", WTERMSIG(status));
	else if (WIFSTOPPED(status))
		printf("child stoped signal number=%d\n", WSTOPSIG(status));
}
```