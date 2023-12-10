#include "commands.h"
#include "shell.h"
#include <iostream>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

std::string GetFilenameFromPath(const std::string &path) {
    // Find the last occurrence of '/'
    size_t pos = path.find_last_of('/');

    if (pos == std::string::npos) {
        // No '/' found, the whole string is a filename
        return path;
    } else {
        // Return the substring after the last '/'
        return path.substr(pos + 1);
    }
}

int ForkingCommand::AddArgument(std::string argument) {
    if (args_.size() == 0) {
        // The first argument is the filepath
        filepath_ = argument;
        filename_ = GetFilenameFromPath(filepath_);
    }
    args_.push_back(argument); // Add the argument to args vector
    return 0;
}

char **ForkingCommand::args_pointer_array() {
    for (int i = 0; i < args_.size(); i++) {
        if (i == 0) {
            execvp_args_.push_back(&filename_[0]);
        } else {
            execvp_args_.push_back(&args_[i][0]);
        }
    }
    execvp_args_.push_back(nullptr);
    return execvp_args_.data();
};

int ForkingCommand::Execute(int fdin, int fdout) {
    // As we will modify stdin and stdout to be fdin and fdout, before forking
    // we need to save the original stdin and stdout so that we can restore them
    int tmpin = dup(0);
    int tmpout = dup(1);

    // If fdin is provided and input_file_ is not empty, then fdin is ignored
    if (fdin != 0) {
        dup2(fdin, 0);
        close(fdin);
    }
    if (fdout != 1) {
        dup2(fdout, 1);
        close(fdout);
    }

    int ret = fork();
    if (ret == 0) {
        // child
        execvp(&filepath_[0], this->args_pointer_array());
        perror("shell");
        _exit(1);
    } else if (ret < 0) {
        // parent
        perror("fork failed");
        _exit(1);
        ;
    }

    dup2(tmpin, 0);
    dup2(tmpout, 1);
    close(tmpin);
    close(tmpout);
    return ret;
};

bool IsDir(std::string dir) {
    struct stat sb;
    if (stat(dir.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    }
    return false;
}

int CdCommand::AddArgument(std::string argument) {
    if (!argument_.empty()) {
        Error("cd: expected either 0 or 1 arguments");
        return 1;
    } else {
        argument_ = argument;
        return 0;
    }
};

int CdCommand::Execute(int fdin, int fdout) {
    int ret = 0;
    if (argument_.empty()) {
        // No argument, go to home directory
        ret = chdir(getenv("HOME"));
    } else {
        if (!IsDir(argument_)) {
            Error("cd: no such directory: " + argument_);
            return 1;
        }
        ret = chdir(&argument_[0]);
    }
    return 0;
};
