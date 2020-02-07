#include "filezilla_linux.h"
#include "filezilla.h"
#include "main.h"

#include "log.h"

int load_filezilla_paths(char *filezilla_sitemanager_path, char *filezilla_recentservers_path) {
	char *home = getenv("HOME");
	snprintf(filezilla_recentservers_path, MAX_PATH_SIZE, "%s/.config/filezilla/recentservers.xml", home);
	snprintf(filezilla_sitemanager_path, MAX_PATH_SIZE, "%s/.config/filezilla/sitemanager.xml", home);

	return 1;
}
