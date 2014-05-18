/*
   Copyright 2012-2014 Pedro A. Hortas (pah@ucodev.org)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <stdint.h>
#include <ctype.h>

#include <sys/types.h>

#include "archdefs.h"
#include "config.h"

struct config config = { { 0, { [ 0 ... HW_STOR_MAX - 1 ] = NULL  } } };

static void _config_scan_storage(const char *path) {
	char tmp_path[_POSIX_PATH_MAX];
	struct dirent *dent;
	unsigned int stor_id;
	DIR *dp;

	/* Craft storage path */
	snprintf(tmp_path, sizeof(tmp_path) - 1, "%s/storage", path);

	/* Open storage directory */
	if (!(dp = opendir(tmp_path))) {
		printf("Unable to read directory '%s': %m\n", path);
		exit(EXIT_FAILURE);
	}

	/* Scan storage directory entries */
	while ((dent = readdir(dp))) {
		/* Search for storage file */
		if (strstr(dent->d_name, "storage")) {
			/* Validate file format */
			if (!isdigit(dent->d_name[0])) {
				printf("Invalid storage format: %s\n", dent->d_name);
				exit(EXIT_FAILURE);
			}

			/* Validate and extract storage ID */
			if ((stor_id = atoi(dent->d_name)) >= HW_STOR_MAX) {
				printf("Invalid storage ID (%d) for storage %s\n", stor_id, dent->d_name);
				exit(EXIT_FAILURE);
			}

			/* Load storage information */
			if (!(config.vm.stor[stor_id] = malloc(strlen(tmp_path) + strlen(dent->d_name) + 2))) {
				printf("Unable to load storage configuration: %m\n");
				exit(EXIT_FAILURE);
			}

			sprintf(config.vm.stor[stor_id], "%s/%s", tmp_path, dent->d_name);
		}
	}

	/* We're done... close directory pointer */
	closedir(dp);
}

static void _config_scan_ram(const char *path) {
	char tmp_path[_POSIX_PATH_MAX];
	char ramval[32];
	FILE *fp;

	/* Craft temporary path */
	sprintf(tmp_path, "%s/ram", path);

	/* Open RAM configuration file */
	if (!(fp = fopen(tmp_path, "r"))) {
		printf("RAM configuration not found: %m\n");
		exit(EXIT_FAILURE);
	}

	/* Read RAM configuration file contents */
	if (!fgets(ramval, sizeof(ramval) - 1, fp)) {
		printf("RAM Configuration file is empty.\n");
		exit(EXIT_FAILURE);
	}

	/* Close file pointer */
	fclose(fp);

	/* Load RAM configuration */
	if (!(config.vm.ram = atoi(ramval))) {
		puts("RAM value cannot be 0.\n");
		exit(EXIT_FAILURE);
	}
}

void config_init(const char *path) {
	memset(&config, 0, sizeof(struct config));

	_config_scan_storage(path);
	_config_scan_ram(path);
}

void config_destroy(void) {
	int i;

	for (i = 0; i < HW_STOR_MAX; i++) {
		if (config.vm.stor[i])
			free(config.vm.stor[i]);
	}
}

