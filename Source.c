#if defined _MSC_VER //if compiling fon visual studio
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


typedef struct TreeNode {
	struct TreeNode* parentPtr;
	struct TreeNode* childPtr;
	struct TreeNode* siblingPtr;
	int depth;
	char name[64];
	char nodeType;

}Node;

Node *root, *cwd;
char line[128];
char command[16], pathname[64];
char dname[64], bname[64];


void initialize() {
	root = (Node*)malloc(sizeof(Node));
	root->parentPtr = NULL;
	root->siblingPtr = NULL;
	root->childPtr = NULL;
	root->depth = 0;
	root->nodeType = 'D';
	char* rootName = "/";
	strcpy(root->name, rootName);
	cwd = root;
}

void mkdirForReal(char* path) {
	Node* dir = (Node*)malloc(sizeof(Node));
	strcpy(dir->name, path);
	dir->nodeType = 'D';
	dir->depth = cwd->depth + 1;
	dir->childPtr = NULL;
	dir->siblingPtr = NULL;
	dir->parentPtr = cwd;
	if (cwd->childPtr) {
		cwd = cwd->childPtr;
		while (cwd->siblingPtr) {
			cwd = cwd->siblingPtr;
		}
		cwd->siblingPtr = dir;
	}
	else {
		cwd->childPtr = dir;
	}
	cwd = dir;
}


void mkdir() {
	Node* currentPosition = cwd;
	if (pathname[0] == '/') { //go to root
		char temp[64];
		strcpy(temp, pathname + 1);
		strcpy(pathname, temp);
		cwd = root;
	}
	char* token;
	token = strtok(pathname, "/");
	while (token != NULL) {
		mkdirForReal(token);
		token = strtok(NULL, "/");
	}
	cwd = currentPosition;
}


bool cdForReal(char* path) {
	if (strcmp(path, "..") == 0) {
		if (cwd->parentPtr != NULL) {
			cwd = cwd->parentPtr;
			return true;
		}
	}
	else {
		Node* content = cwd->childPtr;
		while (content) {
			if ((strcmp(content->name, path) == 0) && content->nodeType == 'D') {
				cwd = content;
				return true;
			}
			else
				content = content->siblingPtr;
		}
	}
	return false;
}

void cd() {
	if (strlen(pathname) == 0 || pathname[0] == '/') {
		cwd = root;
	}
	char* token;
	bool found = true;
	token = strtok(pathname, "/");
	while (token != NULL && found) {
		found = cdForReal(token);
		token = strtok(NULL, "/");
	}
}

void ls() {
	Node* content = cwd->childPtr;
	while (content) {
		printf("%s\n", content->name);
		content = content->siblingPtr;
	}
}

void pwd() {
	int depth = cwd->depth;
	char** paths = (char*)calloc(4, 64);
	int index = depth;
	while (cwd->parentPtr) {
		paths[--index] = cwd->name;
		cwd = cwd->parentPtr;
	}
	for (int i = 0; i < depth; ++i) {
		printf("/%s", paths[i]);
	}
	printf("\n");
}

void creat() {
	Node* currentPosition = cwd;
	char* pos = strrchr(pathname, '/');
	char file[64];
	if (pos != NULL) {
		char file2[64];
		strcpy(file, pos + 1);
		int length = strlen(pathname) - strlen(file) - 1;
		strncpy(file2, pathname, length);
		file2[length] = '\0';
		strcpy(pathname, file2);
		cd();
	}
	Node* fileNode = (Node*)malloc(sizeof(Node));
	printf("file name = %s\n", file);
	strcpy(fileNode->name, file);
	fileNode->nodeType = 'F';
	fileNode->childPtr = NULL;
	fileNode->siblingPtr = NULL;
	fileNode->parentPtr = cwd;
	if (!cwd->childPtr) {
		cwd->childPtr = fileNode;
	}
	else {
		cwd = cwd->childPtr;
		while (cwd->siblingPtr) {
			cwd = cwd->siblingPtr;
		}
		cwd->siblingPtr = fileNode;
	}
	cwd = currentPosition;
}

void rmdir() {
	/*cd();
	cwd->parentPtr;*/
}

void rm(char* pathname) {

}

void menu() {
	printf("\n\n\n*****************************************************************************\n");
	printf("welcome to the linux file tree simulator\nHere are the valid commands:\n");
	printf("mkdir [pathname]: make a new directory for a given pathname\n");
	printf("rmdir [pathname]: remove directory, if it is empty\n");
	printf("cd [pathname]: change CWD to pathname, or to / if no pathname\n");
	printf("ls [pathname]: list the directory contents of pathname or CWD\n");
	printf("pwd: print the (absolute) pathname of CWD\n");
	printf("creat [pathname]: create a FILE node\n");
	printf("rm [pathname]: remove the FILE node\n");
	printf("menu: to see this help screen again\n");
	printf("quit: well you can read\n");
	printf("*****************************************************************************\n\n\n");

}
void quit() {
	printf("See you later\n");
	exit(0);
}


int functionIndex(char* commandName) {
	if (strcmp("mkdir", commandName) == 0) return 0;
	if (strcmp("rmdir", commandName) == 0) return 1;
	if (strcmp("cd", commandName) == 0) return 2;
	if (strcmp("ls", commandName) == 0) return 3;
	if (strcmp("pwd", commandName) == 0) return 4;
	if (strcmp("creat", commandName) == 0) return 5;
	if (strcmp("rm", commandName) == 0) return 6;
	if (strcmp("menu", commandName) == 0) return 7;
	if (strcmp("quit", commandName) == 0) return 8;
	return 7;
}

int main() {
	initialize();
	//declare an array of function pointers
	void (*functionTable[])() = { mkdir, rmdir, cd, ls, pwd, creat, rm, menu, quit };
	for (;;) {
		printf("Input a command: ");
		fgets(line, sizeof(line), stdin);
		line[sizeof(line) - 1] = '\0';
		sscanf(line, "%s %s", command, pathname);
		functionTable[functionIndex(command)]();
		memset(pathname, 0, 64); //clear pathname
	}
}