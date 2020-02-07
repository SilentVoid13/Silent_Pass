#include "filezilla_win.h"
#include "filezilla.h"
#include "main.h"

#include "log.h"

int load_filezilla_paths(char *filezilla_sitemanager_path, char *filezilla_recentservers_path) {
	SHGetSpecialFolderPath(0, filezilla_sitemanager_path, CSIDL_APPDATA, FALSE); 
	SHGetSpecialFolderPath(0, filezilla_recentservers_path, CSIDL_APPDATA, FALSE); 
	strcat(filezilla_sitemanager_path, "\\FileZilla\\sitemanager.xml");
	strcat(filezilla_recentservers_path, "\\FileZilla\\recentservers.xml");

	return 1;
}
