//
// Created by silentvoid on 4/13/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "git_linux.h"
#include "git.h"
#include "main.h"

void load_git_paths(char *git_root_path, char *git_config_path, char *git_xdg_path) {
    char *home = getenv("HOME");
    char *xdg_path = getenv("XDG_CONFIG_HOME");

    snprintf(git_root_path, MAX_PATH_SIZE, "%s/.git-credentials", home);
    snprintf(git_config_path, MAX_PATH_SIZE, "%s/.config/git/credentials", home);

    if(xdg_path != NULL)
        snprintf(git_xdg_path, MAX_PATH_SIZE, "%s/.config/git/credentials", xdg_path);
    else
        git_xdg_path[0] = '\0';
}
