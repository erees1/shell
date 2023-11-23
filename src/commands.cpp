#include "commands.h"
#include <iostream>
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
    args_.push_back(argument); // Add the argument to args vector
    return 0;
}

char **ForkingCommand::args_pointer_array() {
    for (int i = 0; i < args_.size(); i++) {
        if (i == 0) {
            filepath_ = args_[i];
            filename_ = GetFilenameFromPath(filepath_);
            execvp_args_.push_back(&filename_[0]);
        } else {
            execvp_args_.push_back(&args_[i][0]);
        }
    }
    execvp_args_.push_back(nullptr);
    return execvp_args_.data();
};

int ForkingCommand::Execute(int fdin, int fdout) {
    dup2(fdin, 0); // Set fdin to be stdin of the next command
    close(fdin);

    dup2(fdout, 1);
    close(fdout);

    int ret = fork();
    if (ret == 0) {
        // child
        execvp(&filepath_[0], this->args_pointer_array());
        perror("execvp");
    } else if (ret < 0) {
        // parent
        perror("fork failed");
        _exit(1);
        ;
    }
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
        std::cerr << "cd: expected either 0 or 1 argument" << std::endl;
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
            std::cerr << "cd: no such directory: " << argument_ << std::endl;
            return 1;
        }
        ret = chdir(&argument_[0]);
    }
    return 0;
};