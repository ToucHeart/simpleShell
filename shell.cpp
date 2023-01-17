#include<vector>
#include<sstream>
#include<string>
#include <cstdlib>
#include <unistd.h>
#include<iostream>
#include<sys/wait.h>
using namespace std;

static char dir[255];

static const bool SUCCESS = true, FAIL = false;

static void printPrompt(bool succeedLast)
{
	if (succeedLast)
		printf("\033[1;32;47m-> ");
	else
		printf("\033[1;31;47m-> ");
	printf("\033[1;32;47m%s\033[0m$ ", getcwd(dir, 255));
	fflush(stdout);
}

static bool inputCmd(vector<string> &args, char **&argv)
{
	string cmd;
	getline(cin, cmd);
	decltype(cmd.size()) size = cmd.size() - 1;
	while (cmd[size] == ' ')
		size--;
	cmd.resize(size + 1);
	if (cmd.empty())
		return false;
	stringstream ss(cmd);
	string str;
	while (ss.good())
	{
		ss >> str;
		args.push_back(str);
		str.clear();
	}
	try
	{
		argv = new char *[args.size() + 1];
	}
	catch (const bad_alloc &e)
	{
		cerr << "bad_alloc: " << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	for (decltype(args.size()) i = 0; i < args.size(); ++i)
	{
		argv[i] = const_cast<char *>(args[i].c_str());
	}
	argv[args.size()] = nullptr;
	return true;
}

static bool changeDir(vector<string> &args)
{
	static string lastDir;
	if (args.size() == 1)
	{
		args.emplace_back(getenv("HOME"));    //搜索字符串指定的环境变量的值,如果该环境变量不存在，则返回 NULL。
	}
	if (args[1] == "-")
	{
		if (lastDir.empty())
			return true;
		else
			args[1] = lastDir;
	}
	else if (args[1][0] == '~')
	{
		args[1] = string(getenv("HOME")) + args[1].substr(1);
	}
	lastDir = string(getcwd(dir, 255));
	if (chdir(args[1].c_str()) < 0)
	{
		perror(args[1].c_str());
		return false;
	}
	return true;
}

static bool builtins(vector<string> &args, bool &success)//if matches builtins,returns true,otherwise returns false
{
	if (args[0] == "cd")
	{
		if (!changeDir(args))
		{
			success = FAIL;
		}
		return true;
	}
	else if (args[0] == "exit")
		exit(EXIT_SUCCESS);
	return false;
}

static void forkAndExec(char **argv, bool &success)
{
	int status;
	pid_t child_pid = fork();
	if (child_pid < 0)
	{
		perror("Fork failed");
		success = FAIL;
		exit(EXIT_FAILURE);
	}
	else if (child_pid == 0)
	{
		signal(SIGINT, SIG_DFL);//Sets the default behaviour for the signal.
		/* Never returns if the call is successful */
		if (execvp(argv[0], argv) < 0)
		{
			perror(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		waitpid(child_pid, &status, WUNTRACED);
		if ((WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) || WIFSIGNALED(status))
		{
			success = FAIL;
		}
	}
}

static void freeMemory(vector<string> &args, char **&argv)
{
	args.clear();
	if (argv)
	{
		delete[] argv;
	}
}

static void resetRunStatus(bool &s)
{
	s = SUCCESS;
}

int main()
{
	char **argv = nullptr;
	vector<string> args;
	signal(SIGINT, SIG_IGN);//Ignores the signal.
	bool lastCmdRunStatus = SUCCESS;
	while (true)
	{
		printPrompt(lastCmdRunStatus);
		resetRunStatus(lastCmdRunStatus);
		if (!inputCmd(args, argv))//null input
			continue;
		if (!builtins(args, lastCmdRunStatus))//if not builtins, run execvp
			forkAndExec(argv, lastCmdRunStatus);
		freeMemory(args, argv);
	}
}