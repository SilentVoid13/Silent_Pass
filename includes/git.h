//
// Created by silentvoid on 4/13/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PASS_GIT_H
#define SILENT_PASS_GIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_git_paths(char *git_root_path, char *git_config_path, char *git_xdg_path);
int parse_git_credentials(char *path, const char *output_file);

int dump_git(const char *output_file);

#endif //SILENT_PASS_GIT_H
